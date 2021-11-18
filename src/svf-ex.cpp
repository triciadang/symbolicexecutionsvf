//===- svf-ex.cpp -- A driver example of SVF-------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // A driver program of SVF including usages of SVF APIs
 //
 // Author: Yulei Sui,
 */

#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SABER/LeakChecker.h"
#include "SVF-FE/PAGBuilder.h"
#include <z3++.h>
#include <iostream>
#include <fstream>


using namespace SVF;
using namespace llvm;
using namespace std;
using namespace z3;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));

int findRootOfFunction(ICFG* icfg, string fun_name);
int findRoot(ICFG* icfg, const string& function_name);

ofstream MyFile;


int findRoot(ICFG* icfg, const string& function_name, const string& point_type){
    for (int i = 0; i < icfg->getTotalNodeNum(); i++){
        ICFGNode* iNode = icfg->getICFGNode(i);
        string nodeString =  iNode->toString();
        string s1 = "fun: " + function_name;
        string s2 = point_type;  // can only be either Entry or Exit
        if (nodeString.find(s1)!= string::npos && nodeString.find(s2) != string::npos){
            return i;
        }
    }
    return -1;
}


bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


/**
 * Accepts a reference to a list of ICFGNode objects, extracts constraints from each node, run an SMT solver on it, and
 * returns whether or not is the path feasible.
 *
 * @param nodeList  Reference to a list of ICFGNode objects
 * @return          Boolean value denoting the feasibility of the path
 */
bool checkPathFeasibility(std::list<const ICFGNode*>& nodeList) {
    // TODO: Extract constraints from nodes
    // TODO: Feed it to SMT solver
    // TODO: Update output to return based on output of SMT solver
    context c;
    std::list<expr> expr_list;
    std::list<expr> constraints;
    std::map<string, unsigned long> expr_str_to_idx_map;

    for (const ICFGNode* vNode : nodeList) {
        SVF::ICFGNode::PAGEdgeList pagEdges = vNode->getPAGEdges();

        //for each PAGEdge in ICFGNode.getPAGEdges()
        for (const PAGEdge* pagEdge : pagEdges) {
            const Value *valuea = pagEdge->getValue();
            const Instruction *valueb = pagEdge->getInst();

            string opcodeName = valueb->getOpcodeName();
            string beforeEqualOperand = valuea->getName().begin();
            string loadOperand = valueb->operand_values().begin()->getName().data();
            string s1 = vNode->toString();

            //alloca things

            if (opcodeName.compare("alloca") == 0) {
                MyFile << "O1 == " << beforeEqualOperand << endl;
                if (expr_str_to_idx_map.find(beforeEqualOperand) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[beforeEqualOperand] = expr_list.size();
                    expr_list.push_back(c.int_const(beforeEqualOperand.c_str()));
                }
            }

            if (opcodeName.compare("load") == 0) {
                int equalsPos = s1.find(" = ");
                int blankPos = s1.find("   ");
                string firstVar = s1.substr(blankPos + 3, equalsPos - blankPos - 3);
                MyFile << firstVar << " = " << loadOperand << endl;
                if (expr_str_to_idx_map.find(firstVar) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[firstVar] = expr_list.size();
                    expr_list.push_back(c.int_const(firstVar.c_str()));
                }

                int loadOperandIdx = expr_str_to_idx_map[loadOperand];
                auto load_expr_it = expr_list.begin();
                std::advance(load_expr_it, loadOperandIdx);
                expr load_expr = *load_expr_it;

                int firstVarIdx = expr_str_to_idx_map[firstVar];
                auto first_var_it = expr_list.begin();
                std::advance(first_var_it, firstVarIdx);
                expr first_var = *first_var_it;

                constraints.push_back(first_var == load_expr);
            }

            //icmp things
            int posOfOpcode = s1.find("icmp");
            //see if icmp is in
            if (posOfOpcode != string::npos) {
                cout << "lol" << s1 << "\n";
                int posOfI32 = s1.find("i32 ");

                string compareOperator = s1.substr(posOfOpcode + 5,posOfI32-posOfOpcode-6);
//                cout << compareOperator << endl;
                string compareSign;

                string delimiter = ",";
                int posOfDelimiter = s1.find(delimiter);

                string firstOperand = s1.substr(posOfI32 + 4,posOfDelimiter-posOfI32-4);
//                cout << firstOperand << endl;

                s1.erase(0,posOfDelimiter+2);

                posOfDelimiter = s1.find(delimiter);
                string secondOperand = s1.substr(0,posOfDelimiter);
//                cout << secondOperand << endl;

                int firstOperandIdx = expr_str_to_idx_map[firstOperand];
                auto first_expr_it = expr_list.begin();
                std::advance(first_expr_it, firstOperandIdx);
                expr first_expr = *first_expr_it;

                expr second_expr = c.int_const("whoo");
                if (!is_number(secondOperand)) {
                    int secondOperandIdx = expr_str_to_idx_map[secondOperand];
                    auto second_expr_it = expr_list.begin();
                    std::advance(second_expr_it, secondOperandIdx);
                    second_expr = *second_expr_it;
                }

                if (compareOperator.compare("eq") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr == second_expr);
                    } else {
                        constraints.push_back(first_expr == stoi(secondOperand));
                    }
                    compareSign =  "==";
                } else if (compareOperator.compare("ne") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr != second_expr);
                    } else {
                        constraints.push_back(first_expr != stoi(secondOperand));
                    }
                    compareSign =  "!=";
                }
                else if (compareOperator.compare("ugt") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr > second_expr);
                    } else {
                        constraints.push_back(first_expr > stoi(secondOperand));
                    }
                    compareSign =  ">";
                }
                else if (compareOperator.compare("uge") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr >= second_expr);
                    } else {
                        constraints.push_back(first_expr >= stoi(secondOperand));
                    }
                    compareSign =  ">=";
                }
                else if (compareOperator.compare("ult") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr < second_expr);
                    } else {
                        constraints.push_back(first_expr < stoi(secondOperand));
                    }
                    compareSign =  "<";
                }
                else if (compareOperator.compare("ule") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr <= second_expr);
                    } else {
                        constraints.push_back(first_expr <= stoi(secondOperand));
                    }
                    compareSign =  "<=";
                }
                else if (compareOperator.compare("sgt") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr > second_expr);
                    } else {
                        constraints.push_back(first_expr > stoi(secondOperand));
                    }
                    compareSign =  ">";
                }
                else if (compareOperator.compare("sge") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr >= second_expr);
                    } else {
                        constraints.push_back(first_expr >= stoi(secondOperand));
                    }
                    compareSign =  ">=";
                }
                else if (compareOperator.compare("slt") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr < second_expr);
                    } else {
                        constraints.push_back(first_expr < stoi(secondOperand));
                    }
                    compareSign =  "<";
                }
                else if (compareOperator.compare("sle") == 0) {
                    if (!is_number(secondOperand)) {
                        constraints.push_back(first_expr <= second_expr);
                    } else {
                        constraints.push_back(first_expr <= stoi(secondOperand));
                    }
                    compareSign = "<=";
                }
                else{
                    cout << "Invalid compare operator.";
                    compareSign = "";
                }
                MyFile << firstOperand << " " << compareSign << " " << secondOperand << endl;
            }
        }
    }
//    for (const auto & it : expr_str_to_idx_map) {
//        cout << it.first << " " << it.second << "\n";
//    }
//    for (const auto & curr_expr : expr_list) {
//        cout << curr_expr << "\n";
//    }

    solver s(c);
    for (const auto & curr_expr : constraints) {
        s.add(curr_expr);
//        cout << curr_expr << "\n";
    }

    switch(s.check()) {
        case z3::sat:
            return true;
        case z3::unsat:
            return false;
        case z3::unknown:
            return false;
        default:
            return false;
    }
}


/**
 * Accepts the ICFG along with function root ID and target node ID to return a boolean value representing whether or not
 * is the target node reachable.
 * TODO: Add support for getting inputs needed to reach the target node.
 *
 * @param icfg          ICFG graph from svf-tools
 * @param functionRoot  Node ID of the function root
 * @param targetNode    Node ID of the target node
 * @return              Boolean value denoting the reachability of the target node.
 */
bool checkReachabilityForFunction(ICFG* icfg, int functionRoot, int targetNode) {
    const ICFGNode* iNode = icfg->getICFGNode(functionRoot);
    std::list<const ICFGNode*> worklist;  // FILOList
    std::list<const ICFGNode*> emptyList;
    std::list<std::list<const ICFGNode*>> pathList;  // FILOList, keeps track of how we got to the current node
    worklist.push_back(iNode);
    pathList.push_back(emptyList);

    while (!worklist.empty()) {
        const ICFGNode* vNode = worklist.back();
        worklist.pop_back();
        std::list<const ICFGNode*> currPathList = pathList.back();
        pathList.pop_back();
        if ((int) vNode->getId() != functionRoot) {
            cout << vNode->toString() << "==================\n";
        }

        for (auto it = vNode->OutEdgeBegin(); it != vNode->OutEdgeEnd(); ++it) {
            ICFGEdge* edge = *it;
            ICFGNode* succNode = edge->getDstNode();
            if (succNode->getId() == targetNode) {
                cout << currPathList.size() << "\n";  // Path leading to the target node
                if (checkPathFeasibility(currPathList)) {
                    return true;
                }
                continue;
            }
            worklist.push_back(succNode);
            std::list<const ICFGNode*> newPathList = std::list<const ICFGNode*>(currPathList);
            newPathList.push_back(succNode);
            pathList.push_back(newPathList);
//            cout << succNode->getId() << "\n";
        }
    }
    return false;
}


/*!
 * An example to query/collect all successor nodes from a ICFGNode (iNode) along control-flow graph (ICFG)
 */
//void traverseOnICFG(ICFG* icfg, const Instruction* inst){
void traverseOnICFG(ICFG* icfg, int functionRoot){
    ICFGNode* iNode = icfg->getICFGNode(functionRoot);
	FIFOWorkList<const ICFGNode*> worklist;
	std::set<const ICFGNode*> visited;
	worklist.push(iNode);

	/// Traverse along VFG
	//for each ICFGNode ∈ ICFGPath do
	while (!worklist.empty()) {
		const ICFGNode* vNode = worklist.pop();
        if ((int)(vNode->getId()) != functionRoot) {
            cout << vNode->toString() << "==================\n";
            SVF::ICFGNode::PAGEdgeList pagEdges = vNode->getPAGEdges();

            //for each PAGEdge in ICFGNode.getPAGEdges()
            for (auto pagEdge : pagEdges) {
                //auto pagIterator = pagEdges.begin();
                //std::advance(pagIterator, 0);
                const Value *valuea = pagEdge->getValue();
                const Instruction *valueb = pagEdge->getInst();

                cout << valuea->getName().begin() << "\n";
                cout << valueb->getOpcodeName() << "\n";
                cout << valueb->operand_values().begin()->getName().data() << "\n";
                cout << next(valueb->operand_values().begin())->getName().data() << "\n";

                string opcodeName = valueb->getOpcodeName();
                string beforeEqualOperand = valuea->getName().begin();
                string loadoperand = valueb->operand_values().begin()->getName().data();

                //alloca things
                string alloca = "alloca";

                if (opcodeName.compare(alloca) == 0) {
                    MyFile << "O1 == " << beforeEqualOperand << endl;
                }

                //load things
                string load = "load";
                if (opcodeName.compare(load) == 0) {
                    MyFile << loadoperand << " == o" << endl;
                }


                //icmp things
                string s1 = vNode->toString();
                string s2 = "icmp";
                string s3 = "!";

                int posOfOpcode = s1.find(s2);
                //see if icmp is in
                if (posOfOpcode != string::npos) {
                    string s4 = "i32 ";
                    int posOfI32 = s1.find(s4);

                    string compareOperator = s1.substr(posOfOpcode + 5,posOfI32-posOfOpcode-6);
                    cout << compareOperator << endl;
                    string compareSign;

                    if (compareOperator.compare("eq") == 0) {
                        compareSign =  "==";
                    }
                    else if (compareOperator.compare("ne") == 0) {
                        compareSign =  "!=";
                    }
                    else if (compareOperator.compare("ugt") == 0) {
                        compareSign =  ">";
                    }
                    else if (compareOperator.compare("uge") == 0) {
                        compareSign =  ">=";
                    }
                    else if (compareOperator.compare("ult") == 0) {
                        compareSign =  "<";
                    }
                    else if (compareOperator.compare("ule") == 0) {
                        compareSign =  "<=";
                    }
                    else if (compareOperator.compare("sgt") == 0) {
                        compareSign =  ">";
                    }
                    else if (compareOperator.compare("sge") == 0) {
                        compareSign =  ">=";
                    }
                    else if (compareOperator.compare("slt") == 0) {
                        compareSign =  "<";
                    }
                    else if (compareOperator.compare("sle") == 0) {
                        compareSign = "<=";
                    }
                    else{
                        cout << "Invalid compare operator.";
                        compareSign = "";
                    }

                    string delimiter = ",";
                    int posOfDelimiter = s1.find(delimiter);

                    string firstOperand = s1.substr(posOfI32 + 4,posOfDelimiter-posOfI32-4);
                    cout << firstOperand << endl;

                    s1.erase(0,posOfDelimiter+2);

                    posOfDelimiter = s1.find(",");
                    string secondOperand = s1.substr(0,posOfDelimiter);
                    cout << secondOperand << endl;

                    MyFile << firstOperand << " " << compareSign << " " << secondOperand << endl;
                }
            }
        }


		for (ICFGNode::const_iterator it = vNode->OutEdgeBegin(), eit =
				vNode->OutEdgeEnd(); it != eit; ++it) {
			ICFGEdge* edge = *it;
			ICFGNode* succNode = edge->getDstNode();
			if (visited.find(succNode) == visited.end()) {
				visited.insert(succNode);
				worklist.push(succNode);
			}
		}
	}
}

int main(int argc, char ** argv) {
    //file to write mathematical constraints taken from the ICFG
    MyFile.open("constraints.txt");

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Whole Program Points-to Analysis\n");

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    svfModule->buildSymbolTableInfo();
    
	/// Build Program Assignment Graph (PAG)
		PAGBuilder builder;
		PAG *pag = builder.build(svfModule);

		/// ICFG
		ICFG *icfg = pag->getICFG();
        int functionRoot = findRoot(icfg,"foo", "Entry");
		icfg->dump("icfg");
        int functionExit = findRoot(icfg,"foo", "Exit");
        checkReachabilityForFunction(icfg, functionRoot, functionExit);


        //Z3 solver trial
        //http://www.cs.utah.edu/~vinu/research/formal/tools/notes/z3-notes.html#sec-z3-c-interface-cheat-sheet
//        context c;
//        expr x = c.int_const("x");
//        expr y = c.int_const("y");
//        solver s(c);
//
//        s.add(x >= 1);
//        s.add(y < x + 3);
//        std::cout << s.check() << "\n";
//
//        model m = s.get_model();
//        std::cout << m << "\n";
//
//        // traversing the model
//        for (unsigned i = 0; i < m.size(); i++) {
//            func_decl v = m[i];
//            assert(v.arity() == 0);
//
//            std::cout << v.name() << " = " << m.get_const_interp(v) << "\n";
//        }
//
//        // we can evaluate expressions in the model.
//        std::cout << "x + y + 1 = " << m.eval(x + y + 1) << "\n";
//
//            // STEP-3 checks if the result is unsat.
//            switch (s.check()) {
//                case unsat:   std::cout << "de-Morgan is valid\n"; break;
//                case sat:     std::cout << "de-Morgan is not valid\n"; break;
//                case unknown: std::cout << "unknown\n"; break;
//            }



		LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(".svf.bc");
        MyFile.close();


    return 0;
}


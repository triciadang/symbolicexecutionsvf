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

string EQUALS_STRING = "==================================================================================";


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


bool is_positive_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


bool is_number(const std::string& s)
{
    if (s.find('-') == 0) {
        return is_positive_number(s.substr(1, s.size() - 1));
    } else {
        return is_positive_number(s);
    }
}


void replace_element_at_idx(std::list<expr>& expr_list, int idx, expr& new_expr) {
    auto expr_list_it = expr_list.begin();
    std::advance(expr_list_it, idx);
    expr_list.erase(expr_list_it);
    if (expr_list.size() == idx) {
        expr_list.push_back(new_expr);
    } else {
        expr_list_it = expr_list.begin();
        std::advance(expr_list_it, idx);
        expr_list.insert(expr_list_it, new_expr);
    }
}


expr get_element_at_idx(std::list<expr>& expr_list, unsigned long idx) {
    auto expr_it = expr_list.begin();
    std::advance(expr_it, idx);
    return *expr_it;
}


expr get_expr_for_var(std::map<string, unsigned long>& expr_str_to_idx_map, std::list<expr>& expr_list, string var) {
    unsigned long idx = expr_str_to_idx_map[var];
    return get_element_at_idx(expr_list, idx);
}


/**
 * Accepts a reference to a list of ICFGNode objects, extracts constraints from each node, run an SMT solver on it, and
 * returns whether or not is the path feasible.
 *
 * @param nodeList  Reference to a list of ICFGNode objects
 * @return          Boolean value denoting the feasibility of the path
 */
bool checkPathFeasibility(std::list<const ICFGNode*>& nodeList, std::list<bool>& icmpBranchList) {
    // TODO: Extract constraints from nodes
    // TODO: Feed it to SMT solver
    // TODO: Update output to return based on output of SMT solver
    context c;
    std::list<expr> expr_list;
    std::list<expr> constraints;
    std::map<string, unsigned long> expr_str_to_idx_map;

    for (const ICFGNode* vNode : nodeList) {
        SVF::ICFGNode::PAGEdgeList pagEdges = vNode->getPAGEdges();
        string s1 = vNode->toString();

        //for each PAGEdge in ICFGNode.getPAGEdges()
        for (const PAGEdge* pagEdge : pagEdges) {
            const Value *valuea = pagEdge->getValue();
            const Instruction *valueb = pagEdge->getInst();

//            cout << pagEdge->toString() << endl;

            string opcodeName = valueb->getOpcodeName();
            string beforeEqualOperand = valuea->getName().begin();
            string loadOperand = valueb->operand_values().begin()->getName().data();
            loadOperand = "%" + loadOperand;

//            cout << opcodeName << ": " << beforeEqualOperand << "  " << loadOperand << endl;

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

            // add things

            if (opcodeName.compare("add") == 0) {

                if (expr_str_to_idx_map.find(beforeEqualOperand) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[beforeEqualOperand] = expr_list.size();
                    expr_list.push_back(c.int_const(beforeEqualOperand.c_str()));
                }

                int i32Pos = s1.find("i32 ");
                int commaPos = s1.find(",");
                string firstVar = s1.substr(i32Pos + 4, commaPos - i32Pos - 4);

                s1.erase(0,commaPos+2);
                commaPos = s1.find(",");
                string secondVar = s1.substr(0, commaPos);

                if (!is_number(firstVar)) {
                    if (!is_number(secondVar)) {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = first_expr + second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr added_expr = first_expr + stoi(secondVar);
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    }
                } else {
                    if (!is_number(secondVar)) {
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = stoi(firstVar) + second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        constraints.push_back(get_expr_for_var(expr_str_to_idx_map, expr_list, beforeEqualOperand) == (stoi(firstVar) + stoi(secondVar)));
                    }
                }
            }

            // subtract things

            if (opcodeName.compare("sub") == 0) {

                if (expr_str_to_idx_map.find(beforeEqualOperand) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[beforeEqualOperand] = expr_list.size();
                    expr_list.push_back(c.int_const(beforeEqualOperand.c_str()));
                }

                int i32Pos = s1.find("i32 ");
                int commaPos = s1.find(",");
                string firstVar = s1.substr(i32Pos + 4, commaPos - i32Pos - 4);

                s1.erase(0,commaPos+2);
                commaPos = s1.find(",");
                string secondVar = s1.substr(0, commaPos);

                if (!is_number(firstVar)) {
                    if (!is_number(secondVar)) {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = first_expr - second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr added_expr = first_expr - stoi(secondVar);
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    }
                } else {
                    if (!is_number(secondVar)) {
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = stoi(firstVar) - second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        constraints.push_back(get_expr_for_var(expr_str_to_idx_map, expr_list, beforeEqualOperand) == (stoi(firstVar) - stoi(secondVar)));
                    }
                }
            }

            // multiply things

            if (opcodeName.compare("mul") == 0) {

                if (expr_str_to_idx_map.find(beforeEqualOperand) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[beforeEqualOperand] = expr_list.size();
                    expr_list.push_back(c.int_const(beforeEqualOperand.c_str()));
                }

                int i32Pos = s1.find("i32 ");
                int commaPos = s1.find(",");
                string firstVar = s1.substr(i32Pos + 4, commaPos - i32Pos - 4);

                s1.erase(0,commaPos+2);
                commaPos = s1.find(",");
                string secondVar = s1.substr(0, commaPos);

                if (!is_number(firstVar)) {
                    if (!is_number(secondVar)) {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = first_expr * second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr added_expr = first_expr * stoi(secondVar);
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    }
                } else {
                    if (!is_number(secondVar)) {
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = stoi(firstVar) * second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        constraints.push_back(get_expr_for_var(expr_str_to_idx_map, expr_list, beforeEqualOperand) == (stoi(firstVar) * stoi(secondVar)));
                    }
                }
            }

            // add things

            if (opcodeName.compare("sdiv") == 0) {

                if (expr_str_to_idx_map.find(beforeEqualOperand) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[beforeEqualOperand] = expr_list.size();
                    expr_list.push_back(c.int_const(beforeEqualOperand.c_str()));
                }

                int i32Pos = s1.find("i32 ");
                int commaPos = s1.find(",");
                string firstVar = s1.substr(i32Pos + 4, commaPos - i32Pos - 4);

                s1.erase(0,commaPos+2);
                commaPos = s1.find(",");
                string secondVar = s1.substr(0, commaPos);

                if (!is_number(firstVar)) {
                    if (!is_number(secondVar)) {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = first_expr / second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        expr first_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, firstVar);
                        expr added_expr = first_expr / stoi(secondVar);
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    }
                } else {
                    if (!is_number(secondVar)) {
                        expr second_expr = get_expr_for_var(expr_str_to_idx_map, expr_list, secondVar);
                        expr added_expr = stoi(firstVar) / second_expr;
                        replace_element_at_idx(expr_list, expr_str_to_idx_map[beforeEqualOperand], added_expr);
                    } else {
                        constraints.push_back(get_expr_for_var(expr_str_to_idx_map, expr_list, beforeEqualOperand) == (stoi(firstVar) / stoi(secondVar)));
                    }
                }
            }
        }

        //icmp things
        int posOfOpcode = s1.find("icmp");
        //see if icmp is in
        if (posOfOpcode != string::npos) {
//                cout << "Ohh well!" << "\n";
            bool currBranchResult = icmpBranchList.front();
            icmpBranchList.pop_front();
//                cout << "Whoo!" << currBranchResult << "\n";

//            cout << "lol" << s1 << "\n";
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
            else {
                cout << "Invalid compare operator.";
                compareSign = "";
            }

            if (!currBranchResult) {
                expr originalExpr = constraints.back();
                constraints.pop_back();
                constraints.push_back(!originalExpr);
            }

            MyFile << firstOperand << " " << compareSign << " " << secondOperand << endl;
        }


        // store things
        posOfOpcode = s1.find("store");

        //see if store is in node
        if (posOfOpcode != string::npos) {

            int posOfI32 = s1.find("i32 ");
            int posDelimiter = s1.find(',');

            string secondOperand = s1.substr(posOfI32 + 4,posDelimiter-posOfI32-4); //value that needs to be stored
            s1.erase(0,posDelimiter+2); //erase node string up to space

            posDelimiter = s1.find(',');
            int posSpace = s1.find(' ');

            string firstOperand = s1.substr(posSpace+1,posDelimiter-posSpace-1); //place to store it to

            if (expr_str_to_idx_map.find(firstOperand) == expr_str_to_idx_map.end()) {
                expr_str_to_idx_map[firstOperand] = expr_list.size();
                expr_list.push_back(c.int_const(firstOperand.c_str()));
            }

            int firstOperandIdx = expr_str_to_idx_map[firstOperand];
            auto first_expr_it = expr_list.begin();
            std::advance(first_expr_it, firstOperandIdx);
            expr first_expr = *first_expr_it;

            expr second_expr = c.int_const("whoo");
            if (!is_number(secondOperand)) {
                if (expr_str_to_idx_map.find(secondOperand) == expr_str_to_idx_map.end()) {
                    expr_str_to_idx_map[secondOperand] = expr_list.size();
                    expr_list.push_back(c.int_const(secondOperand.c_str()));
                }
                int secondOperandIdx = expr_str_to_idx_map[secondOperand];
                auto second_expr_it = expr_list.begin();
                std::advance(second_expr_it, secondOperandIdx);
                second_expr = *second_expr_it;
            }

            if (!is_number(secondOperand)) {
                replace_element_at_idx(expr_list, firstOperandIdx, second_expr);
//                expr_list.erase(first_expr_it);
//                if (firstOperandIdx == expr_list.size()) {
//                    expr_list.push_back(second_expr);
//                } else {
//                    first_expr_it = expr_list.begin();
//                    std::advance(first_expr_it, firstOperandIdx);
//                    expr_list.insert(first_expr_it, second_expr);
//                }
            } else {
                constraints.push_back(first_expr == stoi(secondOperand));
            }

            //secondOperand = firstOperand
//            cout << "Whoo!" << endl;
//            cout << firstOperand << "=" << secondOperand << endl;
        }
    }
//    for (const auto & it : expr_str_to_idx_map) {
//        cout << it.first << " " << it.second << "\n";
//    }
//    for (const auto & curr_expr : expr_list) {
//        cout << curr_expr << "\n";
//    }

    // Printing results
    cout << "Current Path (in Node ID's): ";
    for (const ICFGNode* currNode : nodeList) {
        if (currNode != nodeList.back()) {
            cout << currNode->getId() << " -> ";
        } else {
            cout << currNode->getId() << endl;
        }
    }

    // Adding constraints
    solver s(c);
    cout << "Constraints:" << endl;
    for (const auto & curr_expr : constraints) {
        s.add(curr_expr);
        cout << curr_expr << endl;
    }

    cout << "Reachable: ";

    switch(s.check()) {
        case z3::sat: {
            cout << "Yes" << endl;

            cout << "Satisfiability Values (Variable Name = Value):" << endl;
            model m = s.get_model();
            for (int i = 0; i < m.size(); i++) {
                func_decl v = m[i];
                string var_name = v.name().str();
                cout << var_name.substr(1, var_name.size() - 1) << " = " << m.get_const_interp(v) << endl;
            }

            cout << EQUALS_STRING << endl;
            return true;
        }
        case z3::unsat: {
            cout << "No" << endl;
            cout << EQUALS_STRING << endl;
            return false;
        }
        case z3::unknown: {
            cout << "Unsure" << endl;
            cout << EQUALS_STRING << endl;
            return false;
        }
        default: {
            cout << "Unsure" << endl;
            cout << EQUALS_STRING << endl;
            return false;
        }
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
    std::list<bool> emptyBoolList;
    std::list<std::list<const ICFGNode*>> pathList;  // FILOList, keeps track of how we got to the current node
    std::list<std::list<bool>> branchList;  // FILOList, keeps track of branching off
    worklist.push_back(iNode);
    pathList.push_back(emptyList);
    branchList.push_back(emptyBoolList);

    bool upcomingSplit = false;

    while (!worklist.empty()) {
        const ICFGNode* vNode = worklist.back();
        worklist.pop_back();
        std::list<const ICFGNode*> currPathList = pathList.back();
        pathList.pop_back();
//        if ((int) vNode->getId() != functionRoot) {
//            cout << vNode->toString() << "==================\n";
//        }

        int cmpTracker = -1;  // -1 -> dw, 0 -> false, 1 -> true

        std::list<bool> currBranchTracker;
        if (upcomingSplit) {
            cmpTracker = 1;
            currBranchTracker = branchList.back();
            branchList.pop_back();
        }

        for (auto it = vNode->OutEdgeBegin(); it != vNode->OutEdgeEnd(); ++it) {
            if (cmpTracker >= 0) {
                std::list<bool> newBranchList = std::list<bool>(currBranchTracker);
                if (cmpTracker == 0) {
                    newBranchList.push_back(false);
                } else {
                    newBranchList.push_back(true);
                }
                branchList.push_back(newBranchList);
                cmpTracker -= 1;
            }
            ICFGEdge* edge = *it;
            ICFGNode* succNode = edge->getDstNode();
            if (succNode->getId() == targetNode) {
//                cout << currPathList.size() << "\n";  // Path leading to the target node
                std::list<bool> currBranchList = branchList.back();
                branchList.pop_back();

                if (checkPathFeasibility(currPathList, currBranchList)) {
//                    return true;
                }
                continue;
            }
            worklist.push_back(succNode);
            std::list<const ICFGNode*> newPathList = std::list<const ICFGNode*>(currPathList);
            newPathList.push_back(succNode);
            pathList.push_back(newPathList);
//            cout << pathList.size() << "\n";
        }

        if (vNode->toString().find("icmp") != string::npos) {
            upcomingSplit = true;
        } else {
            upcomingSplit = false;
        }
    }
    return false;
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


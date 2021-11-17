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
int findRoot(ICFG* icfg, string function_name);

ofstream MyFile;
/*!
 * An example to query alias results of two LLVM values
 */
//AliasResult aliasQuery(PointerAnalysis* pta, Value* v1, Value* v2){
//	return pta->alias(v1,v2);
//}

/*!
 * An example to print points-to set of an LLVM value
 */
//std::string printPts(PointerAnalysis* pta, Value* val){
//
//    std::string str;
//    raw_string_ostream rawstr(str);
//
//    NodeID pNodeId = pta->getPAG()->getValueNode(val);
//    const NodeBS& pts = pta->getPts(pNodeId);
//    for (NodeBS::iterator ii = pts.begin(), ie = pts.end();
//            ii != ie; ii++) {
//        rawstr << " " << *ii << " ";
//        PAGNode* targetObj = pta->getPAG()->getPAGNode(*ii);
//        if(targetObj->hasValue()){
//            rawstr << "(" <<*targetObj->getValue() << ")\t ";
//        }
//    }
//
//    return rawstr.str();
//
//}

int findRoot(ICFG* icfg, string function_name){
    for (int i = 0; i < icfg->getTotalNodeNum(); i++){
        ICFGNode* iNode = icfg->getICFGNode(i);
        string nodeString =  iNode->toString();
        string s1 = "fun: " + function_name;
        string s2 = "Entry";
        if (nodeString.find(s1)!= string::npos && nodeString.find(s2) != string::npos){
            return i;
        }
    }
    return -1;
}

int findRootOfFunction(ICFG* icfg, string fun_name){
    auto it = icfg->begin();
    while(it != icfg->end()){
        if(it->first != 0){
            if(CallBlockNode::classof(icfg->getICFGNode(it->first))==1){
                const Instruction *val = reinterpret_cast<CallBlockNode*>(icfg->getICFGNode(it->first))->getCallSite();

                const CallInst *CI = dyn_cast<SVF::CallInst>(val);

                std::string str;
                llvm::raw_string_ostream rawstr(str);

                Function *F= CI->getCalledFunction();
                string here =  F->getName().begin();
                if (here == fun_name){
                    cout << it->first;
                    return it->first;
                }
            }
        }
        ++it;
    }
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
            for (auto pagIterator = pagEdges.begin(); pagIterator != pagEdges.end(); ++pagIterator) {
                //auto pagIterator = pagEdges.begin();
                //std::advance(pagIterator, 0);
                const PAGEdge *pagEdge = *pagIterator;
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
        int functionRoot = findRoot(icfg,"foo");
		icfg->dump("icfg");
        traverseOnICFG(icfg, functionRoot);


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


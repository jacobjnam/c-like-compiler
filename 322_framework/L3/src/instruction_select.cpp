#include <string> 
#include <iostream>
#include <algorithm>

#include "L3.h"
#include "code_generator.h"
#include "tiling.h"
#include "instruction_select.h"
#include "liveness.h"
using namespace std; 

extern bool is_debug;

namespace L3 {
  /* Context */
  bool Context::isEmpty() { 
    return instructions.size() == 0; 
  }

  vector<Context *> identify_context(Function *f) {
    vector<Context *> contexts;
    Context *C = new Context();
    C->start = 0;
    int ii = 0;
    while (ii < f->instructions.size()) {
      auto temp_inst = f->instructions[ii];
      Instruction_label *l = dynamic_cast<Instruction_label *>(temp_inst);
      Instruction_call *c = dynamic_cast<Instruction_call *>(temp_inst);
      Instruction_br *b = dynamic_cast<Instruction_br *>(temp_inst);
      Instruction_ret *r = dynamic_cast<Instruction_ret *>(temp_inst);
      
      if (l == nullptr && c == nullptr) {
        C->instructions.push_back(temp_inst);
      }
      
      if (l || c || b || r) {
        if (!C->isEmpty()) {
          if (l || c) {
            C->end = ii;
          } else {
            C->end = ii + 1;
          }
          contexts.push_back(C);
        }
        C = new Context();
        C->start = ii + 1;
      }
      ii++;
    }
    return contexts;
  }

  //   for (int idx = 0; idx < f->instructions.size(); idx++) {
  //     Instruction *i = f->instructions[idx];

  //     Instruction_label *l = dynamic_cast<Instruction_label *>(i);
  //     Instruction_call *c = dynamic_cast<Instruction_call *>(i);
      
  //     if (l == nullptr && c == nullptr) {
  //       C->instructions.push_back(i);
  //     }
      
  //     Instruction_br *b = dynamic_cast<Instruction_br *>(i);
  //     Instruction_ret *r = dynamic_cast<Instruction_ret *>(i);
  //     if (l || c || b || r) {
  //       if (!C->isEmpty()) {
  //         if (l || c) {
  //           C->end = idx;
  //         } else {
  //           C->end = idx + 1;
  //         }
  //         contexts.push_back(C);
  //       }
  //       C = new Context();
  //       C->start = idx + 1;
  //     }
  //   }

  //   return contexts;
  // }

  /* Tree Node */
  constexpr int count = 10;
  Node::Node(Item* item) {
      val = item;
  }
  Node::Node() {
      val = nullptr;
  }
  void Node::printNode(Node* node, int space){
      if (node == nullptr) return ;
      space += count;
      if (node->oprand2 != nullptr) printNode(node->oprand2, space);
      for(int i = count; i < space; i++) cout <<" ";
      if (node->op != nullptr && node->val != nullptr) {
          cout << node->val->toString() << "   " << node->op->toString() << endl;
      }
      else if (node->op != nullptr) {
          cout << node->op->toString() << endl;
      }
      else cout << node->val->toString() << endl;
      if (node->oprand1 != nullptr) printNode(node->oprand1, space) ; 
  }
  Trees_Visitor::Trees_Visitor(Instruction* i) {
      this->instruction = i;
  }
  Instruction* Trees_Visitor::getInstruction() {
      return this->instruction;
  }
  void Trees_Visitor::printTree(Trees_Visitor* tree) {
      if(tree->root == nullptr) return; 
      tree->root->printNode(tree->root, 0); 
  }
  void Trees_Visitor::visit(Instruction_ret_not* i) {
      Node* node = new Node(new Empty());
      node->op = i->op;
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_ret_t* i) {
      Node* node = new Node(new Empty());
      node->op = i->op;
      node->oprand1 = new Node(i->arg); 
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_assignment* i) {
      Node* node = new Node(i->dst);
      node->op = new Operation("<-");
      node->oprand1 = new Node(i->src); 
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_math* i) {
      Node* node = new Node(i->dst); 
      node->oprand1 = new Node(i->oprand1); 
      node->op = dynamic_cast<Operation*>(i->op); 
      node->oprand2 = new Node(i->oprand2);
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_compare* i) {
        
      Node* node = new Node(i->dst); 
      node->oprand1 = new Node(i->oprand1); 
      node->op = dynamic_cast<Operation*>(i->op); 
      node->oprand2 = new Node(i->oprand2);
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_load* i) {
        
      Node* node = new Node(i->dst); 
      node->oprand1 = new Node(i->src); 
      node->op = dynamic_cast<Operation*>(i->op); 
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_store* i) {
      Node* node = new Node(i->dst); 
      node->oprand1 = new Node(i->src); 
      node->op = dynamic_cast<Operation*>(i->op);  
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_br_t* i) {
      Node* node = new Node(new Empty()); 
      node->op = i->op;
      node->oprand1 = new Node(i->condition); 
      node->oprand2 = new Node(i->label); 
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_br_label* i) {
      Node* node = new Node(new Empty()); 
      node->op = i->op;
      node->oprand1 = new Node(i->label); 
      root = node;
      uses = i->uses; 
      define = i->define;
  }
  void Trees_Visitor::visit(Instruction_call_noassign *i) {}
  void Trees_Visitor::visit(Instruction_call_assignment *i) {}
  void Trees_Visitor::visit(Instruction_label *i) {}

  //check if there is no definitions of variables used by T2 between T2 and T1
  bool checkDependency(int a, int b, Trees_Visitor* T2, Variable* v, vector<Trees_Visitor*>& trees){
      //between [T2,T1)
      for(int i = a; i <= b; i++){
          Trees_Visitor* t = trees[i]; 
          //check v not used
          for(Item* item : t->uses){
              Variable* u = dynamic_cast<Variable*>(item); 
              if(u != nullptr && u == v){
                  return false; 
              }
          }
          //check used var in T2 not defined between T2 and T1 
          for(Item* item : T2->uses){
              Variable* v_used = dynamic_cast<Variable*>(item); 
              //if v is not a variable, no need to compare, skip 
              if(v_used != nullptr) {
                  for(Item* d : t->define) {
                      Variable* u = dynamic_cast<Variable*>(d); 
                      if(u != nullptr && u == v_used) return false; 
                  }
              }
          }
      }
      return true; 
  }
  bool checkMemoryInst(int a, int b, vector<Instruction*> insts){
      for(int i = a; i <= b; i++){
          Instruction* inst = insts[i];
          Instruction_load* load = dynamic_cast<Instruction_load*>(inst); 
          if(load != nullptr) return false; 
          Instruction_store* store = dynamic_cast<Instruction_store*>(inst);
          if(store != nullptr) return false; 
      }
      return true;
  }
  bool checkDead(int a, Item* v, AnalysisResult* res, vector<Trees_Visitor*> trees){
      for(int i = a; i < trees.size(); i++){
          Trees_Visitor* t = trees[i]; 
          set<Item*> out = res->outs[t->getInstruction()]; 
          if(out.find(v) != out.end()){
              return false; 
          }
      }
      return true;
  }
  bool checkOnlyUsedByT1(int a, Item*v, vector<Instruction*> insts){
      for(int i = a; i < insts.size(); i++){
          vector<Item*> uses = insts[i]->uses; 
          if(find(uses.begin(), uses.end(), v) != uses.end()) return false; 
      }
      return true;
  }
  vector<Trees_Visitor*> getAllTree(Context* context){
      vector<Trees_Visitor*> trees; 
      for(auto i : context->instructions) {
          Trees_Visitor* t = new Trees_Visitor(i); 
          i->accept(t);
          // cout << "tree: " << endl; 
          // t->printTree(t);
          trees.push_back(t);
      }
      return trees;
  }
  vector<Trees_Visitor*> mergeTrees(Context* context, AnalysisResult* res){
      int64_t size = context->instructions.size(); 
      vector<Trees_Visitor*> trees = getAllTree(context); 
      for(int i = 0; i < size; i++){
          Trees_Visitor* T2 = trees[i];
          Variable* v = dynamic_cast<Variable*>(T2->root->val); 
          //current root is not a variable
          //instruction must be return, br, return t, br t, skip
          if(v != nullptr){
              for(int j = i + 1; j < size; j++){
                  Trees_Visitor* T1 = trees[j]; 
                  // if(is_debug) {
                  //     cout << "T2: " << T2->getInstruction()->toString() << endl;
                  //     cout << "T1: " << T1->getInstruction()->toString() << endl;
                  // }
                  //condition 1: T1 uses T2 root
                  if(T1->root->oprand1 && T1->root->oprand1->val == T2->root->val){
                      //condition A, T2->root->val dead after T1
                      bool dead = checkDead(j+1, T2->root->val, res, trees);
                      bool only = checkOnlyUsedByT1(j+1, T2->root->val, context->instructions);
                      if(dead || only) {
                          //condition B
                          bool notDependent = checkDependency(i+1, j-1, T2, v, trees); 
                          //T1 defines v
                          if(T1->root->val == T2->root->val) notDependent = false;
                          //another oprand in T1 uses v 
                          if(T1->root->oprand2 && T1->root->oprand2->val == T2->root->val) notDependent = false;
                          bool noMemoryInst = checkMemoryInst(i+1, j-1, context->instructions);
                          if(notDependent && noMemoryInst){
                              //merge
                              T1->root->oprand1 = T2->root; 
                              T2->root->isroot = false;
                              if (is_debug) {
                                  cout << "merging T2 into T1" << endl;
                              }
                          }
                      }
                  }
                  //condition 1: t_next uses t_curr root
                  else if(T1->root->oprand2 && T1->root->oprand2->val == T2->root->val) {
                      //condition A, T2->root->val dead after T1
                      bool dead = checkDead(j+1, T2->root->val, res, trees);
                      bool only = checkOnlyUsedByT1(j+1, T2->root->val, context->instructions);
                      if(dead || only) {
                          //condition B
                          bool notDependent = checkDependency(i+1, j-1, T2, v, trees); 
                          //T1 defines v
                          if(T1->root->val == T2->root->val) notDependent = false;
                          //another oprand in T1 uses v 
                          if(T1->root->oprand1->val == T2->root->val) notDependent = false;
                          bool noMemoryInst = checkMemoryInst(i+1, j-1, context->instructions);
                          if(notDependent && noMemoryInst){
                              //merge
                              T1->root->oprand2 = T2->root; 
                              T2->root->isroot = false;
                              if (is_debug) {
                                  cout << "merging T2 into T1" << endl;
                              }
                          }
                      }
                  }
                  else continue;
              }
          }
      }
      vector <Trees_Visitor *> merged_trees;
      for (auto t : trees) {
          // t->printTree(t);
          if (t->root->isroot) {
              merged_trees.push_back(t);
          }
      }
      return merged_trees;
  }

  vector<std::string> translate_context(Context *c, AnalysisResult *res, vector<Tile *> alltiles) {
      vector<Trees_Visitor*> merged; 
      CodeGen codegen;
      merged = mergeTrees(c, res); 
      for (auto t : merged) {
          if(is_debug) t->printTree(t);
          //search for matched tiles
          vector<Tile *> tiled;
          tiling(t->root, tiled, alltiles);
          if(is_debug) cout << "# of matched tiles: " << tiled.size() << endl;
          //assign this set of tiles that can cover the tree to this tree
          reverse(tiled.begin(), tiled.end());
          for (auto tile : tiled) {
              tile->accept(&codegen);
          }
      }

      // for (auto s : codegen.L2_instructions) {
      //     cout << s << endl;
      // }
      return codegen.L2_instructions;
  }

  //vector<std::string> translate_label_call(Instruction *i, string longest, int64_t &counter) {
  vector<std::string> translate_label_call(Instruction *i, string fname, int64_t &counter) {
    Instruction_label *l = dynamic_cast<Instruction_label *>(i);
    vector<std::string> res;
    if (l) {
        res.push_back("\t" + l->toString() + '\n');
        return res;
    }

    Instruction_call *c = dynamic_cast<Instruction_call *>(i);
    if (c) {
      vector<L2::Architecture::RegisterID> arguments = L2::Architecture::get_argument_regs();
      string s;
      //input 
      if(c->callee->toString() == "input"){
        s = "\tcall input 0\n";
        res.push_back(s);
        // return res;
      }
      //print
      else if(c->callee->toString() == "print"){
        res.push_back("\trdi <- " + c->args[0]->toString() + "\n");
        res.push_back("\tcall print 1\n"); 
        // return res;
      }
      else if(c->callee->toString() == "allocate"){
        for(int idx= 0; idx < c->args.size(); idx++){
            s = "\t" + L2::Architecture::fromRegisterToString(arguments[idx]) + " <- " + c->args[idx]->toString() + "\n"; 
            res.push_back(s);
        }
        res.push_back("\tcall allocate 2\n"); 
        // return res;                
      }
      else if(c->callee->toString() == "tensor-error"){
        for(int idx= 0; idx < c->args.size(); idx++){
            s = "\t" + L2::Architecture::fromRegisterToString(arguments[idx]) + " <- " + c->args[idx]->toString() + "\n"; 
            res.push_back(s);
        }
        res.push_back("\tcall tensor-error " + to_string(c->args.size()) + "\n");
        // return res;   
      }
      else {
        // string ret_label = ":" + longest + "_ret_" + c->callee->toString().substr(1) + to_string(counter);
        string ret_label = ":" + c->callee->toString().substr(1) + "_ret_" + fname.substr(1)+ "_" + to_string(counter);
        counter++;
        s = "\tmem rsp -8 <- " + ret_label + "\n";
        res.push_back(s);
        for(int idx= 0; idx < c->args.size(); idx++){
            if (idx >= 6) {
                s = "\tmem rsp -" + to_string((idx - 4) * 8) + " <- " + c->args[idx]->toString() + "\n";
                res.push_back(s);
            } else {
          
                s = "\t" + L2::Architecture::fromRegisterToString(arguments[idx]) + " <- " + c->args[idx]->toString() + "\n"; 
                res.push_back(s);
            }
        }
        res.push_back("\tcall " + c->callee->toString() + " " + to_string(c->args.size()) + "\n");        
        res.push_back("\t" + ret_label + "\n"); 

        // return res;
      }
      Instruction_call_assignment* a = dynamic_cast<Instruction_call_assignment*>(i);
      if(a) {
          res.push_back("\t" + a->dst->toString() + " <- rax\n");
      }
      return res;
    }

    cerr << "not label or call" << endl;
    return {};
  }

  vector<std::string> inst_select(Program p, Function* f){        
    //perform liveness analysis on instructions
    GenKill genkill; 
    for(Instruction* i : f->instructions) {
      i->accept(&genkill);
    }
    AnalysisResult* res = computeLiveness(f);
    vector<Context*> ctx = identify_context(f); 
    vector<Tile*> alltiles = getAllTiles();
    vector<std::string> all_instructions;

    int64_t idx = 0;
    int64_t counter = 0;
    for (Context* c : ctx) {
      while (idx != f->instructions.size() && idx != c->start) {
        //auto insts = translate_label_call(f->instructions[idx], p.ll, counter);
        auto insts = translate_label_call(f->instructions[idx], f->name, counter);
        all_instructions.insert(all_instructions.end(), insts.begin(), insts.end());
        idx++;
      }
      auto context_insts = translate_context(c, res, alltiles);
      all_instructions.insert(all_instructions.end(), context_insts.begin(), context_insts.end());
      idx = c->end;
    }

    while (idx != f->instructions.size()) {
      auto insts = translate_label_call(f->instructions[idx], f->name, counter);
      all_instructions.insert(all_instructions.end(), insts.begin(), insts.end());
      idx++;
    }
    if(is_debug){
      for (auto s : all_instructions) {
        cout << "instruction: " << s << endl;
      }        
    }
    return all_instructions;
  }
}
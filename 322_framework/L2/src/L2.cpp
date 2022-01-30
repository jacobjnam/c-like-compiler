#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <liveness.h>
#include <L2.h>

// included libraries
#include <unordered_set>

using namespace std;

namespace L2 {
  /*
   * Constructor/Member Functions
   */
  
  //Register
  Register::Register(reg regi) {
    this->r = regi;
  }
  reg Register::get(void) {
    return this->r;
  }

  //Memory
  Memory::Memory(reg regi, int64_t os){
    this->r = regi;
    this->offset = os;
  }
  std::pair<L2::reg, int64_t> Memory::get(void){
    std::pair<L2::reg, int64_t> res{this->r, this->offset};
    return res; // res.first
  }

  //Number
  Number::Number(int64_t n){
    this->num = n;
  }
  int64_t Number::get(void){
    return this->num;
  }

  //Label
  Label::Label(std::string ln){
    this->labelName = ln;
  }
  std::string Label::get(void){
    return this->labelName;
  }

  //Variable
  Variable::Variable(std::string vn){
    this->varName = vn;
  }
  std::string Variable::get(void){
    return this->varName;
  }

  //Operation
  Operation::Operation(opCode on){
    this->opName = on;
  }
  opCode Operation::get(void){
    return this->opName;
  }

  //Runtime
  Runtime::Runtime(runtimeCode rt) {
    this->runtime = rt;
  }
  runtimeCode Runtime::get(void) {
    return this->runtime;
  };

  // RETURN INSTRUCTION
  void Instruction_ret::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }

  // ASSIGNMENT INSTRUCTION
  void Instruction_assignment::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_assignment::Instruction_assignment(Item *source, Item *dest) {
    this->src = source;
    this->dst = dest;
  }

  // ARITHMETIC INSTRUCTION
  void Instruction_arithmetic::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_arithmetic::Instruction_arithmetic(Item *source, Item *dest, Item *ope) {
    this->src = source;
    this->dst = dest;
    this->op = ope;
  }

  // CREMENT INSTRUCTION
  void Instruction_crement::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_crement::Instruction_crement(Item *dest, Item *ope) {
    this->dst = dest;
    this->op = ope;
  }

  // SHIFT INSTRUCTION
  void Instruction_shift::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_shift::Instruction_shift(Item *source, Item *dest, Item *ope) {
    this->src = source;
    this->dst = dest;
    this->op = ope;
  }

  // COMPARE INSTRUCTION
  void Instruction_cmp::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_cmp::Instruction_cmp(Item *dest, Item *one, Item *two, Item *ope) {
    this->dst = dest;
    this->arg1 = one;
    this->arg2 = two;
    this->op = ope;
  }

  // CONDITIONAL JUMP INSTRUCTION
  void Instruction_cjump::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_cjump::Instruction_cjump(Item *one, Item *two, Item *target, Item *ope) {
    this->arg1 = one;
    this->arg2 = two;
    this->label = target;
    this->op = ope;
  }

  // LEA INSTRUCTION
  void Instruction_lea::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_lea::Instruction_lea(Item *dest, Item *one, Item *two, Item *multiple) {
    this->dst = dest;
    this->arg1 = one;
    this->arg2 = two;
    this->mult = multiple;
  }

  // CALL INSTRUCTION
  void Instruction_calls::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_calls::Instruction_calls(Item *target, Item *numArgs) {
    this->u = target;
    this->N = numArgs;
  }

  // RUNTIME INSTRUCTION
  void Instruction_runtime::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_runtime::Instruction_runtime(Item *target, Item *numArgs) {
    this->runtime = target;
    this->N = numArgs;
  }

  // LABEL INSTRUCTION
  void Instruction_label::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_label::Instruction_label(Item *target) {
    this->label = target;
  }

  // GOTO INSTRUCTION
  void Instruction_goto::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_goto::Instruction_goto(Item *target) {
    this->label = target;
  }

  // STACK-ARG INSTRUCTION
  void Instruction_stackarg::Accept (Visitor *visitor) {
    visitor->VisitInstruction(this);
  }
  Instruction_stackarg::Instruction_stackarg(Item *dest, Item *offset) {
    this->dst = dest;
    this->M = offset;
  }

  /*
   * defining enum to string
   */
  std::string get_enum_string (int enum_value) {
    return (reg_enum_str[enum_value]);

}
#ifndef EVAL_H
#define EVAL_H 1

#include "Value.h"
#include "Context.h"
#include "Instruction.h"

Value Eval_execInstructions(Instruction *instr, Context *ctx);

#endif
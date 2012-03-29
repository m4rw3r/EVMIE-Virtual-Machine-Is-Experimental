#ifndef EVAL_H
#define EVAL_H 1

#include "Value.h"
#include "Instruction.h"
#include "Frame.h"
#include "CFrame.h"

Value Eval_execFrame(Frame *frame);

#endif
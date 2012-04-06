#ifndef EVAL_H
#define EVAL_H 1

#include "Instruction.h"
#include "Frame.h"
#include "CFrame.h"

CFrame_Register Eval_execFrame(Frame *frame);

#endif

#include <assert.h>
#include <stdio.h>
#include "Value.h"
#include "Context.h"
#include "Instruction.h"
#include "Eval.h"

#define VM_DEBUG_LEVEL 0

#define ArithmeticOp(operation)                                              \
	v1 = Context_getVariable(ctx, instr->data1);                             \
	v2 = Context_getVariable(ctx, instr->data2);                             \
	if(Value_isInt32(v1)) {                                                  \
		if(Value_isInt32(v2)) {                                              \
			last_value = Value_fromInt32(                                    \
				Value_getInt32(v1) operation Value_getInt32(v2));            \
		}                                                                    \
		else if(Value_isDouble(v2)) {                                        \
			last_value = Value_fromDouble(                                   \
				(double) Value_getInt32(v1) operation Value_getDouble(v2));  \
		}                                                                    \
		else {                                                               \
			last_value = Value_null();                                       \
		}                                                                    \
	}                                                                        \
	else if(Value_isDouble(v1)) {                                            \
		if(Value_isInt32(v2)) {                                              \
			last_value = Value_fromDouble(                                   \
				Value_getDouble(v1) operation (double) Value_getInt32(v2));  \
		}                                                                    \
		else if(Value_isDouble(v2)) {                                        \
			last_value = Value_fromDouble(                                   \
				Value_getDouble(v1) operation Value_getDouble(v2));          \
		}                                                                    \
		else {                                                               \
			last_value = Value_null();                                       \
		}                                                                    \
	}                                                                        \
	else {                                                                   \
		last_value = Value_null();                                           \
	}

Value Eval_execInstructions(Instruction *instr, Context *ctx)
{
	register Value last_value = Value_null();
	register Value v1;
	register Value v2;
	
	while(instr)
	{
#if VM_DEBUG_LEVEL > 1
		printf("Running %s, %016llx\n", Instruction_getTypeName(instr), (uint64_t) instr);
#endif
		
		/* TODO: More instructions, and expand existing if needed */
		switch(instr->type)
		{
			case INS(NOP):
				break;
			case INS(ADD):
				ArithmeticOp(+);
				break;
			case INS(SUB):
				ArithmeticOp(-);
				break;
			case INS(MUL):
				ArithmeticOp(*);
				break;
			case INS(DIV):
				ArithmeticOp(/);
				break;
			case INS(MOD):
				v1 = Context_getVariable(ctx, instr->data1);
				v2 = Context_getVariable(ctx, instr->data2);
				if(Value_isInt32(v1)) {
					if(Value_isInt32(v2)) {
						last_value = Value_fromInt32(
							Value_getInt32(v1) % Value_getInt32(v2));
					}
					else {
						last_value = Value_null();
					}
				}
				else {
					last_value = Value_null();
				}
				break;
			case INS(SL):
			case INS(SR):
			case INS(BW_OR):
			case INS(BW_AND):
			case INS(BW_XOR):
			case INS(BW_NOT):
			case INS(IS_EQ):
				v1 = Context_getVariable(ctx, instr->data1);
				v2 = Context_getVariable(ctx, instr->data2);
			
				last_value = Value_fromBool(Value_equals(v1, v2));
				break;
			case INS(IS_LT):
				v1 = Context_getVariable(ctx, instr->data1);
				v2 = Context_getVariable(ctx, instr->data2);
			
				if(Value_isInt32(v1) && Value_isInt32(v2))
				{
					last_value = Value_fromBool(Value_getInt32(v1) < Value_getInt32(v2));
				}
				break;
			case INS(VARADD):
				v1 = Context_getVariable(ctx, instr->data1);
				if(Value_isInt32(v1))
				{
					Context_setVariable(ctx, instr->data1, Value_fromInt32(
						Value_getInt32(v1) + (int64_t) instr->data2));
				}
				else if(Value_isDouble(v1))
				{
					Context_setVariable(ctx, instr->data1, Value_fromDouble(
						Value_getDouble(v1) + (double) instr->data2));
				}
				break;
			case INS(SETLV):
				Context_setVariable(ctx, instr->data1, last_value);
				break;
			case INS(SETTO):
				Context_setVariable(ctx, instr->data1, (Value) {.asBits = instr->data2});
				break;
			case INS(FETCH):
				last_value = Context_getVariable(ctx, instr->data1);
				break;
			case INS(COPY):
				Context_setVariable(ctx, instr->data2, Context_getVariable(ctx, instr->data1));
				break;
			case INS(EQ):
				v1 = Context_getVariable(ctx, instr->data1);
				v2 = Context_getVariable(ctx, instr->data2);
				last_value = Value_fromBool(v1.asBits == v2.asBits);
				break;
			case INS(JMP):
				assert(instr->data1 != 0);
				instr = (Instruction *) instr->data1;
				continue;
			case INS(JMPZ):
				if((Value_isBool(last_value) && Value_getBool(last_value) == 0) ||
					(Value_isInt32(last_value) && Value_getInt32(last_value) == 0) ||
					Value_isNull(last_value))
				{
					assert(instr->data1 != 0);
					
					instr = (Instruction *) instr->data1;
					continue;
				}
				break;
			case INS(JNZ):
				if((Value_isBool(last_value) && Value_getBool(last_value) != 0) ||
					(Value_isInt32(last_value) && Value_getInt32(last_value) != 0))
				{
					assert(instr->data1 != 0);
					
					instr = (Instruction *) instr->data1;
					continue;
				}
				break;
			case INS(PRINT):
				printf("bits:            %016lx\n", last_value.asBits);
				
				if(Value_isDouble(last_value))
				{
					printf("double: %f\n", Value_getDouble(last_value));
				}
				if(Value_isInt32(last_value))
				{
					printf("int32: %d\n", Value_getInt32(last_value));
				}
				if(Value_isBool(last_value))
				{
					printf("bool: %d\n", Value_getBool(last_value));
				}
				if(Value_isNull(last_value))
				{
					printf("null\n");
				}
				if(Value_isPointer(last_value))
				{
					printf("Pointer:         %016llx\n", (uint64_t) Value_getPointer(last_value));
				}
				if(Value_isString(last_value))
				{
					printf("String: %s\n", Value_getString(last_value));
				}
				break;
		}
		
		instr = instr->next;
		
#if VM_DEBUG_LEVEL > 1
		printf("last_value: %016lx\n", last_value.asBits);
#endif
	}
	
	return last_value;
}

#undef ArithmeticOp(operation)

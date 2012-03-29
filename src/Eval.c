
#include <assert.h>
#include <stdio.h>

#include "Eval.h"

#define VM_DEBUG_LEVEL 0

#define ArithmeticOp(operation)                                              \
	if(Value_isInt32(VM_VAR1)) {                                                  \
		if(Value_isInt32(VM_VAR2)) {                                              \
			last_value = Value_fromInt32(                                    \
				Value_getInt32(VM_VAR1) operation Value_getInt32(VM_VAR2));            \
		}                                                                    \
		else if(Value_isDouble(VM_VAR2)) {                                        \
			last_value = Value_fromDouble(                                   \
				(double) Value_getInt32(VM_VAR1) operation Value_getDouble(VM_VAR2));  \
		}                                                                    \
		else {                                                               \
			last_value = Value_null();                                       \
		}                                                                    \
	}                                                                        \
	else if(Value_isDouble(VM_VAR1)) {                                            \
		if(Value_isInt32(VM_VAR2)) {                                              \
			last_value = Value_fromDouble(                                   \
				Value_getDouble(VM_VAR1) operation (double) Value_getInt32(VM_VAR2));  \
		}                                                                    \
		else if(Value_isDouble(VM_VAR2)) {                                        \
			last_value = Value_fromDouble(                                   \
				Value_getDouble(VM_VAR1) operation Value_getDouble(VM_VAR2));          \
		}                                                                    \
		else {                                                               \
			last_value = Value_null();                                       \
		}                                                                    \
	}                                                                        \
	else {                                                                   \
		last_value = Value_null();                                           \
	}

#define VM_VAR(var) values[(var)]
#define VM_VAR1     VM_VAR(instr->data1.asUInt)
#define VM_VAR2     VM_VAR(instr->data2.asUInt)

Value Eval_execFrame(const Frame *frame)
{
	register Value last_value           = Value_null();
	register Value *values              = frame->variables;
	register const Instruction *instr   = frame->instructions;
	register const Instruction *const last_instr = &instr[frame->num_instructions - 1];
	
	if(frame->num_instructions == 0)
	{
		/* TODO: Some other error here? */
		/* TODO: Move error code */
		fprintf(stderr, "Error: Frame does not contain any instructions, or has not been packed!\n");
		
		exit(-1);
	}
	
	while(instr <= last_instr)
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
				if(Value_isInt32(VM_VAR1)) {
					if(Value_isInt32(VM_VAR2)) {
						last_value = Value_fromInt32(
							Value_getInt32(VM_VAR1) % Value_getInt32(VM_VAR2));
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
				last_value = Value_fromBool(Value_equals(VM_VAR1, VM_VAR2));
				break;
			case INS(IS_LT):
				if(Value_isInt32(VM_VAR1) && Value_isInt32(VM_VAR2))
				{
					last_value = Value_fromBool(Value_getInt32(VM_VAR1) < Value_getInt32(VM_VAR2));
				}
				break;
			case INS(VARADD):
				if(Value_isInt32(VM_VAR1))
				{
					Frame_setVariable(frame, instr->data1.asUInt, Value_fromInt32(
						Value_getInt32(VM_VAR1) + instr->data2.asInt));
				}
				else if(Value_isDouble(VM_VAR1))
				{
					Frame_setVariable(frame, instr->data1.asUInt, Value_fromDouble(
						Value_getDouble(VM_VAR1) + (double) instr->data2.asInt));
				}
				break;
			case INS(SETLV):
				Frame_setVariable(frame, instr->data1.asUInt, last_value);
				break;
			case INS(SETTO):
				Frame_setVariable(frame, instr->data1.asUInt, (Value) {.asBits = instr->data2.asUInt});
				break;
			case INS(FETCH):
				last_value = Frame_getVariable(frame, instr->data1.asUInt);
				break;
			case INS(COPY):
				Frame_setVariable(frame, instr->data2.asUInt, Frame_getVariable(frame, instr->data1.asUInt));
				break;
			case INS(EQ):
				last_value = Value_fromBool(VM_VAR1.asBits == VM_VAR2.asBits);
				break;
			case INS(JMP):
				assert(instr->data1.asInt != 0);
				instr = instr + instr->data1.asInt;
				continue;
			case INS(JMPZ):
				if((Value_isBool(last_value) && Value_getBool(last_value) == 0) ||
					(Value_isInt32(last_value) && Value_getInt32(last_value) == 0) ||
					Value_isNull(last_value))
				{
					assert(instr->data1.asInt != 0);
					
					instr = instr + instr->data1.asInt;
					continue;
				}
				break;
			case INS(JNZ):
				if((Value_isBool(last_value) && Value_getBool(last_value) != 0) ||
					(Value_isInt32(last_value) && Value_getInt32(last_value) != 0))
				{
					assert(instr->data1.asInt != 0);
					
					instr = instr + instr->data1.asInt;
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
		
		instr++;
		
#if VM_DEBUG_LEVEL > 1
		printf("last_value: %016lx\n", last_value.asBits);
#endif
	}
	
	return last_value;
}

#undef ArithmeticOp(operation)

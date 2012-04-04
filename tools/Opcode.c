
#include "Opcode.h"

Opcode_ParamType Opcode_str2paramType(const char *str)
{
	if(strcmp(str, "variable") == 0) {
		return PARAM_VARIABLE;	
	} else if(strcmp(str, "cframe") == 0) {
		return PARAM_CFRAMEIDX;
	} else if(strcmp(str, "int") == 0) {
		return PARAM_INTEGER;
	} else if(strcmp(str, "uint") == 0) {
		return PARAM_UINT;
	} else if(strcmp(str, "value") == 0) {
		return PARAM_VALUE;
	} else {
		return PARAM_UNKNOWN;
	}
}

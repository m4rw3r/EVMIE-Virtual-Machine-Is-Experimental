
#include "Opcode.h"

Opcode_ParamType Opcode_str2paramType(const char *str)
{
	if(strcmp(str, "variable") == 0) {
		return PARAM_VARIABLE;	
	} else if(strcmp(str, "undef") == 0) {
		return PARAM_UNDEF;
	} else if(strcmp(str, "cframe") == 0) {
		return PARAM_CFRAMEID;
	} else if(strcmp(str, "int") == 0) {
		return PARAM_INTEGER;
	} else if(strcmp(str, "uint") == 0) {
		return PARAM_UINT;
	} else if(strcmp(str, "const") == 0) {
		return PARAM_CONST;
	} else {
		return PARAM_UNKNOWN;
	}
}

const char *Opcode_paramType2str(Opcode_ParamType type)
{
	switch(type & ~PARAM_CONST) {
		case PARAM_VARIABLE:
		return "variable";
		case PARAM_UNDEF:
		return "undef";
		case PARAM_INTEGER:
		case PARAM_VARINT:
		return "int";
		case PARAM_UINT:
		case PARAM_VARUINT:
		return "uint";
		case PARAM_CFRAMEID:
		case PARAM_VARCFRAME:
		return "cframe";
		default:
		return "null";
	}
}

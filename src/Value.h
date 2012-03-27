#ifndef VALUE_H
#define VALUE_H 1

#include "config.h"

/* uint64_t and uintptr_t */
#include <inttypes.h>
/* memcpy */
#include <string.h>
/* malloc */
#include <stdlib.h>
#include <assert.h>

#ifndef DOUBLE_IS_IEEE_754
#  error "double data type does not seem to conform to the IEEE 754 standard"
#endif

#define DOUBLE_MATCH_UINTPTR_T 1

/* First attempt to use uintptr_t datatype to store the raw bits */
#if SIZEOF_DOUBLE != SIZEOF_UINTPTR_T
#  warning "Size mismatch between double and uintptr_t"
#  undef DOUBLE_MATCH_UINTPTR_T
#endif
/* The extra defined() check is to avoid additional warning */
#if ALIGNOF_DOUBLE != ALIGNOF_UINTPTR_T && defined(DOUBLE_MATCH_UINTPTR_T)
#  warning "Alignment mismatch between double and uintptr_t"
#  undef DOUBLE_MATCH_UINTPTR_T
#endif

#ifdef DOUBLE_MATCH_UINTPTR_T
#  define VALUE_PTR_T uintptr_t
#else
#  warning "Falling back on uint64_t"
#  if SIZEOF_DOUBLE != SIZEOF_UINTPTR_T
#    error "Size mismatch between double and uint64_t"
#  endif
#  if ALIGNOF_DOUBLE != ALIGNOF_UINTPTR_T
#    error "Alignment mismatch between double and uint64_t"
#  endif
#  define VALUE_PTR_T uint64_t
#endif

typedef struct Value_CString
{
	char *str;
	int len;
} Value_CString;

typedef union Value
{
	double asDouble;
	VALUE_PTR_T asBits;
} Value;

/*                      0x   c   8   4   0 */
#define Value_PtrMask   0x0000FFFFFFFFFFFF
#define Value_Int32Tag  0x0001000100000000
#define Value_NullTag   0x0001000200000000
#define Value_BoolTag   0x0001000400000000
#define Value_MinDouble 0x0002000000000000
#define Value_TagMask   (Value_Int32Tag | Value_MinDouble | Value_NullTag | Value_BoolTag)
/* Tags for pointers, stored in bits which *should not* be used (ie. large enough allocated memory) */
#define Value_StrTag    0x0000000000000001
#define Value_ObjTag    0x0000000000000002
#define Value_HashTag   0x0000000000000004

static inline Value Value_fromPointer(const void *const pointer)
{
	/* ensure that the pointer really is only 48 bit */
	/* TODO: Also make sure that the lowest bits are not set */
	assert((((VALUE_PTR_T)pointer) & ~Value_PtrMask) == 0);
	
	return (Value) {.asBits = (VALUE_PTR_T) pointer};
}

static inline Value Value_fromInt32(const int32_t number)
{
	return (Value) {.asBits = (VALUE_PTR_T)(((uint32_t) number) | Value_Int32Tag)};
}

static inline Value Value_fromDouble(const double number)
{
	int32_t asInt32 = (int32_t)number;
	
	// if the double can be losslessly stored as an int32 do so
	// (int32 doesn't have -0, so check for that too)
	if (number == asInt32 && ! ((number == 0 && *(VALUE_PTR_T *)(&number) != 0))) {
		return Value_fromInt32(asInt32);
	}
	
	return (Value){.asBits = ((Value) {.asDouble = number}.asBits) + Value_MinDouble};
}

static inline Value Value_fromString(const char *str, const int len)
{
	Value_CString *cstr = malloc(sizeof(Value_CString));
	
	cstr->str = malloc(sizeof(char) * (len + 1));
	memcpy(cstr->str, str, len + 1);
	
	cstr->len = len;
	
	return (Value) {.asBits = ((VALUE_PTR_T) cstr) | Value_StrTag};
}

static inline Value Value_fromBool(const int val)
{
	return (Value) {.asBits = ((VALUE_PTR_T) (val & 1)) | Value_BoolTag};
}

static inline Value Value_null()
{
	return (Value) {.asBits = (VALUE_PTR_T) Value_NullTag};
}

#define Value_isDouble(value)  ((value).asBits > Value_MinDouble)
#define Value_isInt32(value)   (((value).asBits & Value_Int32Tag) == Value_Int32Tag)
#define Value_isBool(value)    (((value).asBits & Value_BoolTag) == Value_BoolTag)
#define Value_isNull(value)    ((value).asBits == Value_NullTag)
#define Value_isString(value)  (((value).asBits & (Value_StrTag | ~Value_PtrMask)) == Value_StrTag)
#define Value_isPointer(value) (!((value).asBits & Value_TagMask))
#define Value_equals(value1, value2) ((value1).asBits == (value2).asBits)

static inline double Value_getDouble(const Value value)
{
	assert(Value_isDouble(value));
	
	return (Value){.asBits = (value.asBits - Value_MinDouble)}.asDouble;
}

static inline int32_t Value_getInt32(const Value value)
{
	assert(Value_isInt32(value));
	
	return (int32_t) (value).asBits;
}

static inline int Value_getBool(const Value value)
{
	assert(Value_isBool(value));
	
	return (int) (value).asBits & ~Value_BoolTag;
}

static inline void *Value_getPointer(const Value value)
{
	assert(Value_isPointer(value));
	
	return (void *) (value).asBits;
}

static inline char *Value_getString(const Value value)
{
	assert(Value_isString(value));
	
	return ((Value_CString *)(value.asBits & ~Value_StrTag))->str;
}

static inline int Value_getStringLen(const Value value)
{
	assert(Value_isString(value));
	
	return ((Value_CString *)(value.asBits & ~Value_StrTag))->len;
}

#endif
#ifndef STATICASSERT_H
#define STATICASSERT_H

#define StaticAssert_CONCAT_(a, b) a##b
#define StaticAssert_CONCAT(a, b) StaticAssert_CONCAT_(a, b)

#define StaticAssert(e, msg) \
	enum { StaticAssert_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif
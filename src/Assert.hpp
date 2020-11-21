#pragma once
#include <cassert>

// Simple assert wrapper.
// If you wish disable asserts, or do something else redefine this macro.
#ifdef DEBUG
#define GG_ASSERT(x) \
	{ assert(x); }
#else
#define GG_ASSERT(x) {}
#endif

#define GG_UNUSED(x) (void)x

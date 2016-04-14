#define HAVE_ROUND
#ifdef _DEBUG

	#undef _DEBUG
	#include <python.h>
	#define _DEBUG 1

#else

	#include <python.h>

#endif

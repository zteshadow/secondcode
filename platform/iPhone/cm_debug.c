
#include "cm_debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>


void cm_assert(S32 flag, const S8 *file, U32 line)
{
    if (!flag)
    {        
        fprintf(stderr, "* Assertion: %s@(%u)\n", file, line);
        assert(0);
    }
}

void cm_trace(const S8 *fmt, ...)
{   
	char buf[200];

	va_list   ap;  
	va_start(ap, fmt);

	vsprintf(buf, fmt, ap);         /*DO NOT MORE THAN 200*/ 	
	fprintf(stderr, "%s", buf);
	
	va_end(ap);  
}


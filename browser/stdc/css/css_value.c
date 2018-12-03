/* this file is generated automatically */

#include "../inc/config.h"
#include "../tools/strList.h"
#include "css_value.h"

static NStrList* l_cssValueNames = N_NULL;

void css_initVals(void)
{
    const char* vals = \
		",bold,both,bottom,center,dashed" \
		",dotted,fixed,hidden,inline-block,left" \
		",middle,no-repeat,none,repeat-x,repeat-y" \
		",right,solid,top,underline" \
		;

    l_cssValueNames = strList_create(vals);
}

void css_delVals(void)
{
    strList_delete(&l_cssValueNames);
}

const char** css_getValueNames()
{
    return (const char**)l_cssValueNames->lst;
}

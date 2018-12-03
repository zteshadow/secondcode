/* this file is generated automatically */

#include "../inc/config.h"
#include "../tools/strList.h"
#include "char_ref.h"

static NStrList* l_charref = N_NULL;
static NStrList* l_entity = N_NULL;

void xml_initEntityTable(void)
{
    const char* refs = \
		",amp,gt,lt,nbsp" \
     ;
    const char* ents = \
		",&,>,<, " \
     ;
        
    l_charref = strList_create(refs);
    l_entity = strList_create(ents);
}

void xml_delEntityTable(void)
{
    strList_delete(&l_charref);
    strList_delete(&l_entity);
}

const char** xml_getCharrefNames(void)
{
    return (const char**)l_charref->lst;
}

const char** xml_getEntityNames(void)
{
    return (const char**)l_entity->lst;
}

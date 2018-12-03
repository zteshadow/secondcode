/* this file is generated automatically */

#include "../inc/config.h"
#include "../tools/strList.h"
#include "xml_tags.h"

static NStrList* l_xmlTagNames = N_NULL;

void xml_initTags(void)
{
    const char* tags = \
		",a,anchor,b,base,big,body,br,button,card,div" \
		",font,form,frame,frameset,go,h1,h2,h3,h4,h5" \
		",h6,head,hr,html,img,input,label,li,link,meta" \
		",object,ol,onevent,option,p,param,postfield,script,select,small" \
		",span,strong,style,table,tc_attachment,td,textarea,timer,title,tr" \
		",ul,wml" \
		",text";

    l_xmlTagNames = strList_create(tags);
}

void xml_delTags(void)
{
    strList_delete(&l_xmlTagNames);
}

const char** xml_getTagNames(void)
{
    return (const char**)l_xmlTagNames->lst;
}

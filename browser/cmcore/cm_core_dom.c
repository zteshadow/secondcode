
#include "cm_core_dom.h"
#include "cm_core_parser.h"

#include "cm_lib.h"
#include "cm_string.h"

#include "cm_debug.h"


typedef struct _property
{
    S8 *name;
    S8 *value;
    struct _property *next;
    
} NodeProperty;

typedef enum
{
    ElementNode,
    TextNode,
    NodeTypeMax
    
} NodeType;

typedef struct _node
{
    NodeType type;
    S8 *value;      //html, head, etc
    
    struct _node *first_child;
    struct _node *next_sibling;
    struct _node *parent;

    NodeProperty *property;

} NodeStruct;

typedef struct
{
    NodeStruct *root;
    NodeStruct *current;

} DOMStruct;


static void cm_parser_output(CMOutputType cmd, S8 *data, void *usr);
static const S8 *cm_parser_get_cmd_name(CMOutputType cmd);

static DOMStruct *cm_parser_dom_init(void);

static NodeStruct *cm_parser_create_node(NodeType type, const S8 *tag_name);
static void cm_parser_append_node(NodeStruct *parent, NodeStruct *child);

static NodeProperty *cm_parser_create_property(const S8 *p_name);
static void cm_parser_set_p_value(NodeProperty *property, const S8 *value);

static void cm_parser_open_tag(DOMStruct *docoment, S8 *tag);
static void cm_parser_close_tag(DOMStruct *docoment, S8 *tag);
static void cm_parser_get_p_name(DOMStruct *docoment, S8 *p_name);
static void cm_parser_get_p_value(DOMStruct *docoment, S8 *p_value);
static void cm_parser_get_text(DOMStruct *docoment, S8 *text);


static void cm_parser_output(CMOutputType cmd, S8 *data, void *usr)
{
    CM_TRACE_0("%s\t\t\t%s\n", cm_parser_get_cmd_name(cmd), data);

    if (usr != 0)
    {
        DOMStruct *docoment = (DOMStruct *)usr;

        switch (cmd)
        {
        case CM_GET_OPEN_TAG:
            cm_parser_open_tag(docoment, data);
        break;

        case CM_GET_P_NAME:
            cm_parser_get_p_name(docoment, data);
        break;

        case CM_GET_P_VALUE:
            cm_parser_get_p_value(docoment, data);
        break;

        case CM_GET_CLOSE_TAG:
            cm_parser_close_tag(docoment, data);
        break;

        case CM_GET_TEXT:
            cm_parser_get_text(docoment, data);
        break;

        case CM_PARSE_DONE:
        {
            S32 tmp;
            tmp = 0;
        }
        break;
        
        default:
            CM_ASSERT(0);
        break;
        }
    }
}

static const S8 *cm_parser_get_cmd_name(CMOutputType cmd)
{
    const S8 *value;
    
    switch(cmd)
    {
    case CM_GET_TEXT:
        value = "Text:\t";
    break;

    case CM_GET_OPEN_TAG:
        value = "Open tag:\t";
    break;

    case CM_GET_CLOSE_TAG:
        value = "Close tag:\t";
    break;
    
    case CM_GET_P_NAME:
        value = "Property name:\t";
    break;
    
    case CM_GET_P_VALUE:
        value = "Property value:\t";
    break;

    default:
        value = 0;
    break;
    }

    return value;
}

static DOMStruct *cm_parser_dom_init(void)
{
    DOMStruct *p = (DOMStruct *)CM_MALLOC(sizeof(DOMStruct));

    if (p)
    {
        p->root = cm_parser_create_node(ElementNode, "document");
        p->current = p->root;
    }

    return p;
}

static NodeStruct *cm_parser_create_node(NodeType type, const S8 *tag_name)
{
    NodeStruct *p = (NodeStruct *)CM_MALLOC(sizeof(NodeStruct));

    if (p)
    {
        cm_memset(p, 0, sizeof(NodeStruct));
        p->type = type;
        p->value = cm_strdup(tag_name);
    }

    return p;
}

static void cm_parser_append_node(NodeStruct *parent, NodeStruct *child)
{
    NodeStruct *p = parent->first_child;

    if (p != 0)
    {
        while (p->next_sibling != 0)
        {
            p = p->next_sibling;
        }

        p->next_sibling = child;
    }
    else
    {
        parent->first_child = child;
    }
    
    child->parent = parent;
}

static NodeProperty *cm_parser_create_property(const S8 *p_name)
{
    NodeProperty *p = (NodeProperty *)CM_MALLOC(sizeof(NodeProperty));

    if (p != 0)
    {
        p->name = cm_strdup(p_name);
        p->value = 0;
        p->next = 0;
    }

    return p;
}

static void cm_parser_set_p_value(NodeProperty *property, const S8 *value)
{
    CM_ASSERT(property->value == 0);

    property->value = cm_strdup(value);
}

static void cm_parser_open_tag(DOMStruct *docoment, S8 *tag)
{
    NodeStruct *node = cm_parser_create_node(ElementNode, tag);

    if (node != 0)
    {
        cm_parser_append_node(docoment->current, node);
        docoment->current = node;
    }
}

static void cm_parser_close_tag(DOMStruct *docoment, S8 *tag)
{
    CM_ASSERT(cm_strcmp(docoment->current->value, tag) == 0);

    docoment->current = docoment->current->parent;
}

static void cm_parser_get_p_name(DOMStruct *docoment, S8 *p_name)
{
    NodeProperty *p = cm_parser_create_property(p_name);

    if (p != 0)
    {
        p->next = docoment->current->property;
        docoment->current->property = p;
    }
}

static void cm_parser_get_p_value(DOMStruct *docoment, S8 *p_value)
{
    cm_parser_set_p_value(docoment->current->property, p_value);
}

static void cm_parser_get_text(DOMStruct *docoment, S8 *text)
{
    NodeStruct *node = cm_parser_create_node(TextNode, text);

    if (node != 0)
    {
        cm_parser_append_node(docoment->current, node);
    }
}


S32 cm_core_dom_create(S8 *data)
{
    DOMStruct *docoment = cm_parser_dom_init();

    if (docoment != 0)
    {
        cm_data_parser(data, cm_parser_output, (void *)docoment);
    }

    return 0;
}


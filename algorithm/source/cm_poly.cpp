/*
 * poly.cpp
 *
 *  Created on: 2010-12-1
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_poly.h"
#include "cm_list.h"
#include "cm_debug.h"
#include "cm_lib.h"
//#include "cm_string.h"


#define IS_POLY_SIGN(a) ((a) == '+' || (a) == '-')
#define GET_POLY_SIGN(a) (((a) == '-') ? -1 : 1)
#define IS_POLY_KEY(a) ((a) == 'X' || (a) == 'x')

#define POLY_ABS(a) ((a) > 0 ? (a) : (-a))


typedef struct _CM_POLY_NODE
{
    S32 coeff;
    S32 expo;

}CM_POLY_NODE;

typedef struct
{
    void *list;

} CM_POLY_STRUCT;

//coeff * x (expo)
typedef enum
{
    CM_PARSE_COEFF,
    CM_PARSE_EXPO

} CM_PARSE_STATE;


static void *cm_poly_make_node(S32 coeff, S32 expo);
static CM_POLY_STRUCT *cm_poly_create(void);


static void *cm_poly_make_node(S32 coeff, S32 expo)
{
    CM_POLY_NODE *node = (CM_POLY_NODE *)CM_MALLOC(sizeof(CM_POLY_NODE));
    CM_ASSERT(node != 0);

    if (node)
    {
        node->expo = expo;
        node->coeff = coeff;
    }

    return node;
}

static CM_POLY_STRUCT *cm_poly_create(void)
{
    CM_POLY_STRUCT *poly = (CM_POLY_STRUCT *)CM_MALLOC(sizeof(CM_POLY_STRUCT));
    CM_ASSERT(poly != 0);

    if (poly)
    {
        poly->list = cm_list_create();
        CM_ASSERT(poly->list != 0);
    }

    return poly;
}

void *cm_poly_create_by_string(S8 *p)
{
    CM_ASSERT(p != 0 && p[0] != 0);

    CM_POLY_STRUCT *poly = 0;
    if (p != 0 && p[0] != 0)
    {
        poly = cm_poly_create();

        S32 coeff, expo;
        S8 *p_coeff, *p_expo;

        S8 ch;
        S32 prefix = 1;
        p_coeff = 0;
        CM_PARSE_STATE state = CM_PARSE_COEFF;
        while ((ch = *p) != 0)
        {
            if (CM_PARSE_COEFF == state)
            {
                if (IS_POLY_SIGN(ch))
                {
                    prefix = GET_POLY_SIGN(ch);
                }
                else if (cm_isdigit(ch))
                {
                    if (p_coeff == 0)
                    {
                        p_coeff = p;                //coeff starts here
                    }
                }
                else if (IS_POLY_KEY(ch))
                {
                    if (p_coeff == 0)
                    {
                        coeff = 1;
                    }
                    else
                    {
                        *p = 0;                     //for convenient
                        coeff = cm_atoi((const S8 *)p_coeff);
                    }
                    
                    coeff = prefix * coeff;

                    p_expo = 0;
                    state = CM_PARSE_EXPO;
                }
                else
                {
                    CM_TRACE_1("invalid char: 0x%x @CM_PARSE_COEFF", ch);
                    CM_ASSERT(0);
                }
            }
            else if (CM_PARSE_EXPO == state)
            {
                if (IS_POLY_SIGN(ch))
                {
                    prefix = GET_POLY_SIGN(ch);   //for next coeff

                    if (p_expo == 0)
                    {
                        expo = 1;
                    }
                    else
                    {
                        *p = 0;                     //for convenient
                        expo = cm_atoi((const S8 *)p_expo);
                    }

                    void *node = cm_poly_make_node(coeff, expo);
                    cm_list_append_item(poly->list, node);

                    p_coeff = 0;
                    state = CM_PARSE_COEFF;                    
                }            
                else if (cm_isdigit(ch))
                {
                    if (p_expo == 0)
                    {
                        p_expo = p;                 //expo starts here
                    }
                }
                else
                {
                    CM_TRACE_1("invalid char: 0x%x @CM_PARSE_EXPO", ch);
                    CM_ASSERT(0);                
                }
            }

            p++;
        }

        if (CM_PARSE_COEFF == state)
        {
            CM_ASSERT(p_coeff != 0);
            if (p_coeff != 0)
            {
                coeff = prefix * cm_atoi((const S8 *)p_coeff);
                void *node = cm_poly_make_node(coeff, 0);
                cm_list_append_item(poly->list, node);
            }
        }
        else if (CM_PARSE_EXPO == state)
        {
            if (p_expo == 0)
            {
                expo = 1;
            }
            else
            {
                expo = cm_atoi((const S8 *)p_expo);
            }

            void *node = cm_poly_make_node(coeff, expo);
            cm_list_append_item(poly->list, node);
        
        }
        else
        {
            CM_ASSERT(0);
        }
    }
    
    return poly;
}

void *cm_poly_add(void *poly1, void *poly2)
{
    CM_ASSERT(poly1 != 0 && poly2 != 0);

    CM_POLY_STRUCT *p1 = (CM_POLY_STRUCT *)poly1;
    CM_POLY_STRUCT *p2 = (CM_POLY_STRUCT *)poly2;

    cm_list_start_get_item(p1->list);
    cm_list_start_get_item(p2->list);

    CM_POLY_NODE *n1 = (CM_POLY_NODE *)cm_list_get_next_item(p1->list);
    CM_POLY_NODE *n2 = (CM_POLY_NODE *)cm_list_get_next_item(p2->list);

    void *tmp;
    CM_POLY_STRUCT *h = cm_poly_create();
    if (h)
    {
        while (n1 && n2)
        {
            if (n1->expo < n2->expo)
            {
                tmp = cm_poly_make_node(n1->coeff, n1->expo);
                n1 = (CM_POLY_NODE *)cm_list_get_next_item(p1->list);
            }
            else if (n1->expo > n2->expo)
            {
                tmp = cm_poly_make_node(n2->coeff, n2->expo);
                n2 = (CM_POLY_NODE *)cm_list_get_next_item(p2->list);
            }
            else
            {
                tmp = cm_poly_make_node(n1->coeff + n2->coeff, n1->expo);
                n1 = (CM_POLY_NODE *)cm_list_get_next_item(p1->list);
                n2 = (CM_POLY_NODE *)cm_list_get_next_item(p2->list);
            }

            cm_list_append_item(h->list, tmp);
        }

        while (n1)
        {
            tmp = cm_poly_make_node(n1->coeff, n1->expo);
            cm_list_append_item(h->list, tmp);
            n1 = (CM_POLY_NODE *)cm_list_get_next_item(p1->list);
        }

        while (n2)
        {
            tmp = cm_poly_make_node(n2->coeff, n2->expo);
            cm_list_append_item(h->list, tmp);
            (CM_POLY_NODE *)cm_list_get_next_item(p2->list);
        }
    }

    return h;
}

void *cm_poly_multi(void *p1, void *p2)
{
    return 0;
}

void cm_poly_print(void *poly)
{
    CM_ASSERT(poly != 0);

    CM_POLY_STRUCT *p = (CM_POLY_STRUCT *)poly;
    if (p)
    {
        cm_list_start_get_item(p->list);

        CM_TRACE_0("\n");
        CM_POLY_NODE *node = (CM_POLY_NODE *)cm_list_get_next_item(p->list);

        //print coeff without sign '+'
        if (POLY_ABS(node->coeff) != 1)
        {
            CM_TRACE_0("%d", POLY_ABS(node->coeff));
        }
        else
        {
            if (node->coeff == -1)
            {
                CM_TRACE_0("-");
            }
            if (node->expo == 0)
            {
                CM_TRACE_0("%d", POLY_ABS(node->coeff));
            }
        }

        //print expo
        if (node->expo != 0)
        {
            CM_TRACE_0("x");
            if (node->expo != 1)
            {
                CM_TRACE_0("%d", node->expo);
            }
        }

        node = (CM_POLY_NODE *)cm_list_get_next_item(p->list);
        while (node)
        {
            //print sign
            if (node->coeff > 0)
            {
                CM_TRACE_0("+");
            }
            else
            {
                CM_TRACE_0("-");
            }

            //print coeff
            if (POLY_ABS(node->coeff) != 1)
            {
                CM_TRACE_0("%d", POLY_ABS(node->coeff));
            }
            else
            {
                if (node->expo == 0)
                {
                    CM_TRACE_0("%d", POLY_ABS(node->coeff));
                }
            }

            //print expo
            if (node->expo != 0)
            {
                CM_TRACE_0("x");
                if (node->expo != 1)
                {
                    CM_TRACE_0("%d", node->expo);
                }
            }

            node = (CM_POLY_NODE *)cm_list_get_next_item(p->list);
        }
        
        //CM_TRACE_0("%dx%d", node->coeff, node->expo);
    }
}

void cm_poly_destory(void *poly)
{
    CM_ASSERT(poly != 0);

    CM_POLY_STRUCT *h = (CM_POLY_STRUCT *)poly;
    if (h)
    {
        if (h->list)
        {
            cm_list_destory(h->list);
        }

        CM_FREE(h);
    }
}


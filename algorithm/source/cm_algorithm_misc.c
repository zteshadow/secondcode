/*
 * cm_algorithm_misc.cpp
 *
 *  Created on: 12 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_algorithm_misc.h"
#include "cm_stack.h"
#include "cm_io.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"


#define CM_IS_LEFT_SYMBOL(c)    ((c) == '[' || (c) == '(' || (c) == '{')
#define CM_IS_RIGHT_SYMBOL(c)   ((c) == ']' || (c) == ')' || (c) == '}')
#define CM_IS_LINE_SYMBOL(c)    ((c) == '\r' || (c) == '\n')
#define CM_IS_STR_SYMBOL(c)     ((c) == '\'' || (c) == '\"')
#define CM_IS_SAME_LINE(pre, c) ((pre) == '\r' && (c) == '\n')

//#define CM_IS_OPERATOR(c)       ((c) == '+' || (c) == '*')
#define CM_IS_OPERATOR(c)       ((c) == '+' || (c) == '*' || (c) == '(' || (c) == ')')
#define CM_IS_R_OPERATOR(c)     ((c) == ')')
#define CM_IS_L_OPERATOR(c)     ((c) == '(')
#define CM_IS_PLUS_OPERATOR(c)  ((c) == '+')


static S32 cm_match_pair(S8 l, S8 r);
static S32 cm_has_higher_level(S8 l, S8 r);


static S32 cm_match_pair(S8 l, S8 r)
{
    if ((l == '(' && r == ')')
     || (l == '[' && r == ']')
     || (l == '{' && r == '}'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static S32 cm_has_higher_level(S8 l, S8 r)
{
    S32 value = 0;

    if (CM_IS_L_OPERATOR(l))
    {
        value = 1;
    }
    else
    {
        if ((l == '*') && (r == '+'))
        {
            value = 1;
        }
    }
    
    return value;
}

S32 cm_algorithm_check_symbol_pair(const S8 *fname)
{
    S32 value = 1, error = 0;
    CM_HANDLE fp = cm_fopen(fname, "rb");

    if (fp)
    {
        void *stack = cm_stack_create((void *)100);
        if (stack)
        {
            S8 ch, pre;
            S32 pre_is_line = 0, line = 1;
            S32 in_str = 0;

            while (cm_fread(&ch, 1, 1, fp) != 0)
            {
                if (CM_IS_LEFT_SYMBOL(ch))
                {
                    if (!in_str)
                    {
                        cm_stack_push(stack, (void *)ch);
                    }
                    pre_is_line = 0;
                }
                else if (CM_IS_RIGHT_SYMBOL(ch))
                {
                    if (!in_str)
                    {
                        S8 r = (S8)cm_stack_pop(stack);
                        if (!cm_match_pair(r, ch))
                        {
                            error = 1;
                            CM_TRACE_0("error at line: %d\n", line);
                        }
                    }
                    
                    pre_is_line = 0;
                }
                else if (CM_IS_LINE_SYMBOL(ch))
                {
                    if (!CM_IS_SAME_LINE(pre, ch))
                    {
                        pre_is_line = 1;
                        line++;
                    }
                }
                else if (CM_IS_STR_SYMBOL(ch))
                {
                    in_str = !in_str;                    
                }
                else
                {
                    pre_is_line = 0;
                }

                pre = ch;
            }

            if (!cm_stack_is_empty(stack))
            {
                error = 1;
                CM_TRACE_0("error at file\n");
            }

            cm_stack_destory(stack);
        }

        cm_fclose(fp);
    }

    if (!error)
    {
        CM_TRACE_0("symbol in pair is good\n");
    }
    
    return value;
}

S32 cm_algorithm_gcd(S32 a, S32 b)
{
    while (b)
    {
        S32 c = a % b;
        a = b;
        b = c;
    }

    return a;
}

S32 cm_algorithm_postfix_value(const S8 *data)
{
    S8 ch;
    S32 value, len;
	void *stack;
    const S8 *p = data;
    
    CM_ASSERT(data != 0);

    len = cm_strlen(data);
    stack = cm_stack_create((void *)len);
    if (stack)
    {
        while ((ch = *p++) != 0)
        {
            if (cm_isdigit(ch))
            {
                cm_stack_push(stack, (void *)(ch - '0'));
            }
            else if (CM_IS_OPERATOR(ch))
            {
                S32 v1 = (S32)cm_stack_pop(stack);
                S32 v2 = (S32)cm_stack_pop(stack);

                if(CM_IS_PLUS_OPERATOR(ch))
                {
                    v1 = v1 + v2;
                }
                else
                {
                    v1 = v1 * v2;
                }
                cm_stack_push(stack, (void *)v1);
            }
        }

        value = (S32)cm_stack_top(stack);
        cm_stack_destory(stack);
    }

    return value;
}

S32 cm_algorithm_infix_to_postfix(const S8 *infix, S8 *postfix)
{
    S8 ch;
    S32 len = 0;
	void *stack;
    const S8 *p = infix;
    
    CM_ASSERT(infix != 0 && postfix != 0);

    stack = cm_stack_create((void *)cm_strlen(infix));
    if (stack)
    {
        while ((ch = *p++) != 0)
        {
            if (cm_isdigit(ch))
            {
                postfix[len++] = ch;
            }
            else if (CM_IS_OPERATOR(ch))
            {
                S8 top;

                if (CM_IS_R_OPERATOR(ch))
                {
                    //')' match '('
                    while (!cm_stack_is_empty(stack))
                    {
                        top = (S8)cm_stack_pop(stack);
                        if (cm_match_pair(top, ch))
                        {
                            break;
                        }
                        else
                        {
                            postfix[len++] = top;
                        }
                    }
                }
                else
                {
                    while (!cm_stack_is_empty(stack))
                    {
                        top = (S8)cm_stack_top(stack);
                        if (cm_has_higher_level(ch, top) || CM_IS_L_OPERATOR(top))
                        {
                            break;
                        }
                        else
                        {
                            if (!CM_IS_L_OPERATOR(top)) //left operator never show
                            {
                                postfix[len++] = top;
                            }
                            cm_stack_pop(stack);
                        }
                    }
                    cm_stack_push(stack, (void *)ch);
                }
            }
        }

        while (!cm_stack_is_empty(stack))
        {
            postfix[len++] = (S8)cm_stack_pop(stack);
        }
        
        cm_stack_destory(stack);
    }

    return len;
}


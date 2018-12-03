
#include "cm_tree_utility.h"

#include "cm_io.h"
#include "cm_string.h"

#include "cm_debug.h"


#define CM_HORIZONTAL_PAD   (45)
#define CM_NODE_RADIUS      (30)


static S32 cm_general_compare(CMTreeNode d1, CMTreeNode d2);
static void cm_general_print(CMTreeNode d, S8 *text);
static CM_POINT cm_binary_tree_mid_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        const CM_POINT* right,
                                        S32 pad);
static CM_POINT cm_binary_tree_right_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        S32 pad);
static CM_POINT cm_binary_tree_draw_node(CMTree node, CMTreeFunction *func,
                                         const CM_POINT* p, void *hdc);
static CM_POINT cm_binary_tree_print_ext(CMTree tree, CMTreeFunction *func,
                                         S32 x, S32 y, S32 left, S32 *r,
                                         void *hdc);


static S32 cm_general_compare(CMTreeNode d1, CMTreeNode d2)
{
    if ((S32)d1 > (S32)d2)
    {
        return 1;
    }
    else if ((S32)d1 < (S32)d2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static void cm_general_print(CMTreeNode d, S8 *text)
{
    cm_sprintf(text, "%d", (S32)d);
}

static CM_POINT cm_binary_tree_mid_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        const CM_POINT* right,
                                        S32 pad)
{
    CM_POINT pos;

    if (left->y == 0 && right->y == 0)
    {
        pos.x = x;
        pos.y = y;
    }
    else if (left->y == 0)
    {
        pos.x = right->x - pad;
        pos.y = right->y - pad;
        
    }
    else if (right->y == 0)
    {
        pos.x = left->x + pad;
        pos.y = left->y - pad;
    }
    else
    {
        pos.x = (left->x + right->x) / 2;
        pos.y = left->y - pad;
    }

    return pos;
}

static CM_POINT cm_binary_tree_right_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        S32 pad)
{
    CM_POINT pos;

    if (left->y == 0)
    {
        pos.x = x + pad;
        pos.y = y + pad;
    }
    else
    {
        pos.x = left->x + 2 * pad;
        pos.y = left->y;
        
    }

    return pos;
}

static CM_POINT cm_binary_tree_draw_node(CMTree node, CMTreeFunction *func,
                                         const CM_POINT* p, void *hdc)
{
    CM_POINT pos = {0};

    if (node)
    {
        S32 offset;
        S8 text[11];
        CM_RECT rect;
        CM_POINT text_pos;

        offset = CM_NODE_RADIUS / 2;
        rect.x = p->x - offset > 0 ? p->x - offset : 1;
        rect.y = p->y - offset > 0 ? p->y - offset : 1;

        rect.w = CM_NODE_RADIUS;
        rect.h = CM_NODE_RADIUS;

        cm_gdi_draw_circle(hdc, &rect);

        pos.x = rect.x + offset;
        pos.y = rect.y + offset;

        //cm_sprintf(text, "%d", node->value);
        func->print(node, text);
        
        text_pos = pos;
        text_pos.x -= 4;
        text_pos.y -= 8;
        cm_gdi_draw_text(hdc, text, cm_strlen(text), &text_pos);
    }

    return pos;
}

static CM_POINT cm_binary_tree_print_ext(CMTree tree, CMTreeFunction *func,
                                         S32 x, S32 y, S32 left, S32 *r,
                                         void *hdc)
{
    S32 border;
    CM_POINT l_pos = {0}, pos, r_pos = {0};

    border = x;
    *r = x;

    pos.x = x;
    pos.y = y;

    if (tree != 0)
    {
        if (func->getLeft(tree) != 0)       //left
        {
            x = x - CM_HORIZONTAL_PAD > left ? x - CM_HORIZONTAL_PAD : left + CM_HORIZONTAL_PAD;

            l_pos = cm_binary_tree_print_ext(func->getLeft(tree), func, 
                                           x,
                                           y + CM_HORIZONTAL_PAD,
                                           left,
                                           &border,
                                           hdc);
        }

        if (func->getRight(tree) != 0)      //right
        {
            pos = cm_binary_tree_right_node_pos(x, y, &l_pos, CM_HORIZONTAL_PAD);

            r_pos = cm_binary_tree_print_ext(func->getRight(tree), func, pos.x, pos.y, border + CM_HORIZONTAL_PAD, &border, hdc);

            *r = r_pos.x + CM_NODE_RADIUS;
        }

        pos = cm_binary_tree_mid_node_pos(x, y, &l_pos, &r_pos, CM_HORIZONTAL_PAD);        
        
        pos = cm_binary_tree_draw_node(tree, func, &pos, hdc);
		*r = pos.x > border ? pos.x : border;

        if (func->getLeft(tree) != 0)
        {
            cm_gdi_draw_line(hdc, &l_pos, &pos);
        }

        if (func->getRight(tree) != 0)
        {
            cm_gdi_draw_line(hdc, &pos, &r_pos);
        }
    }

    return pos;
}

CM_POINT cm_binary_tree_print(CMTree tree, CMTreeFunction *func,
                          S32 x, S32 y, void *hdc)
{
    S32 r;
    
    CM_ASSERT(func != 0 && func->getLeft != 0 && func->getRight != 0);
    
    if (func->print == 0)
    {
        func->print = cm_general_print;
    }

    if (func->compare == 0)
    {
        func->compare = cm_general_compare;
    }
    
    return cm_binary_tree_print_ext(tree, func, x, y, 0, &r, hdc);
}


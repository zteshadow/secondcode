
#include "cm_binary_tree.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_gdi.h"
#include "cm_io.h"
#include "cm_debug.h"


#define CM_HORIZONTAL_PAD   (45)
#define CM_NODE_RADIUS      (30)
#define CM_USE_RECURSIVE 1
#define CM_MAX(a, b)    ((a) > (b) ? (a) : (b))
#define CM_ABS(a)       ((a) > 0 ? (a) : (-(a)))


struct BTreeNode
{
    S32 value;
    SearchTree left;
    SearchTree right;
    S32 height;

    S32 pos;
};


static CM_POINT cm_binary_tree_mid_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        const CM_POINT* right,
                                        S32 pad);

static CM_POINT cm_binary_tree_right_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        S32 pad);

static CM_POINT cm_binary_tree_draw_node(SearchTree node, const CM_POINT* pos, void *hdc);
static SearchTree cm_binary_tree_create_node(S32 value);
static void cm_binary_tree_layout(SearchTree tree, S32 pos);
static S32 cm_binary_tree_get_width(SearchTree tree);
static S32 cm_binary_tree_print_level(SearchTree t, S32 level);

static S32 cm_binary_tree_get_height(SearchTree t);
static SearchTree cm_single_rotation_with_left(SearchTree t);
static SearchTree cm_single_rotation_with_right(SearchTree t);
static SearchTree cm_double_rotation_with_left(SearchTree t);
static SearchTree cm_double_rotation_with_right(SearchTree t);


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

static CM_POINT cm_binary_tree_draw_node(SearchTree node, 
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

        cm_sprintf(text, "%d", node->value);
        text_pos = pos;
        text_pos.x -= 4;
        text_pos.y -= 8;
        cm_gdi_draw_text(hdc, text, cm_strlen(text), &text_pos);
    }

    return pos;
}

static SearchTree cm_binary_tree_create_node(S32 value)
{
    SearchTree node = (SearchTree)CM_MALLOC(sizeof(*node));

    CM_ASSERT(node != 0);
    
    if (node)
    {
        node->value = value;
        node->left = 0;
        node->right = 0;
        node->height = 0;
    }

    return node;
}

static void cm_binary_tree_layout(SearchTree tree, S32 pos)
{
    if (tree != 0)
    {
        S32 l = 0, r = 0;

        if (tree->left != 0)
        {
            pos = 0; //有左子树，由左子树决定，无左子树，用指定
            l = cm_binary_tree_get_width(tree->left);
            tree->left->pos = (l >> 1);

            cm_binary_tree_layout(tree->left->left, tree->left->pos - 2);
            cm_binary_tree_layout(tree->left->right, tree->left->pos + 2);
        }

        tree->pos = pos + l + 2;
        
        if (tree->right != 0)
        {
            r = cm_binary_tree_get_width(tree->right);
            tree->right->pos = tree->pos + 2 + (r >> 1);

            cm_binary_tree_layout(tree->right->left, tree->right->pos - 2);
            cm_binary_tree_layout(tree->right->right, tree->right->pos + 2);
        }
    }
}

static S32 cm_binary_tree_get_width(SearchTree tree)
{
    S32 width = 0;
    
    if (tree != 0)
    {
        if (tree->left != 0)
        {
            width += 2 + cm_binary_tree_get_width(tree->left);
        }

        if (tree->right != 0)
        {
            width += 2 + cm_binary_tree_get_width(tree->right);
        }
    }

    return width;
}

static S32 cm_binary_tree_print_level(SearchTree t, S32 level)
{
    if (t)
    {
        if (level == 0)
        {
            S32 i;

            for (i = 0; i < t->pos; i++)
            {
                CM_TRACE_5(" ");
            }
            CM_TRACE_5("%d", t->value);
            return 1;
        }
        else
        {
            S32 v1 = cm_binary_tree_print_level(t->left, level - 1);
            S32 v2 = cm_binary_tree_print_level(t->right, level - 1);

            return ((v1 == 0) && (v2 == 0)) ? 0 : 1;
        }
    }
    else
    {
        return 0;
    }
}

static S32 cm_binary_tree_get_height(SearchTree t)
{
    if (t == 0)
    {
        return -1;
    }
    else
    {
        return t->height;
    }
}

static SearchTree cm_single_rotation_with_left(SearchTree t)
{
    S32 l, r;
    
    SearchTree value = t->left;

    t->left = value->right;
    value->right = t;

    l = cm_binary_tree_get_height(t->left);
    r = cm_binary_tree_get_height(t->right);
    t->height = CM_MAX(l, r) + 1;

    l = cm_binary_tree_get_height(value->left);
    value->height = CM_MAX(l, t->height) + 1;

    return value;
}

static SearchTree cm_single_rotation_with_right(SearchTree t)
{
    S32 l, r;
    SearchTree value = t->right;

    t->right = value->left;
    value->left = t;

    l = cm_binary_tree_get_height(t->left);
    r = cm_binary_tree_get_height(t->right);
    t->height = CM_MAX(l, r) + 1;

    r = cm_binary_tree_get_height(value->right);
    value->height = CM_MAX(t->height, r) + 1;

    return value;
}

static SearchTree cm_double_rotation_with_left(SearchTree t)
{
    t->left = cm_single_rotation_with_right(t->left);
    return cm_single_rotation_with_left(t);
}

static SearchTree cm_double_rotation_with_right(SearchTree t)
{
    t->right = cm_single_rotation_with_left(t->right);
    return cm_single_rotation_with_right(t);
}

void cm_binary_tree_free(SearchTree tree)
{
    //left, right, mid(postfix)
    if (tree != 0)
    {
        cm_binary_tree_free(tree->left);
        cm_binary_tree_free(tree->right);

        CM_FREE(tree);
    }
}

SearchTree cm_binary_tree_insert(SearchTree t, S32 value)
{
    if (t == 0)                             //null
    {
        SearchTree node = cm_binary_tree_create_node(value);
        return node;
    }
    else
    {
        if (t->value != value)
        {
            S32 l_height, r_height;

            if (value < t->value)
            {
                t->left = cm_binary_tree_insert(t->left, value);

                l_height = cm_binary_tree_get_height(t->left);
                r_height = cm_binary_tree_get_height(t->right);

                if ((l_height - r_height) == 2)
                {
                    if (value < t->left->value)
                    {
                        t = cm_single_rotation_with_left(t);
                    }
                    else
                    {
                        t = cm_double_rotation_with_left(t);
                    }
                }
            }
            else
            {
                t->right = cm_binary_tree_insert(t->right, value);

                l_height = cm_binary_tree_get_height(t->left);
                r_height = cm_binary_tree_get_height(t->right);

                if ((r_height - l_height) == 2)
                {
                    if (value > t->right->value)
                    {
                        t = cm_single_rotation_with_right(t);
                    }
                    else
                    {
                        t = cm_double_rotation_with_right(t);
                    }
                }                
            }

            l_height = cm_binary_tree_get_height(t->left);
            r_height = cm_binary_tree_get_height(t->right);
            t->height = CM_MAX(l_height, r_height) + 1;
        }
    }

    return t;    
}

SearchTree cm_binary_tree_delete(SearchTree tree, S32 value)
{
    CM_ASSERT(tree != 0);
    if (tree != 0)
    {
        if (value < tree->value)
        {
            tree->left = cm_binary_tree_delete(tree->left, value);

            if (cm_binary_tree_get_height(tree->right)
              - cm_binary_tree_get_height(tree->left) == 2)
            {
                if (cm_binary_tree_get_height(tree->right->right)
                  > cm_binary_tree_get_height(tree->right->left))
                {
                    tree = cm_single_rotation_with_right(tree);
                }
                else
                {
                    tree = cm_double_rotation_with_right(tree);
                }
            }
        }
        else if (value > tree->value)
        {
            tree->right = cm_binary_tree_delete(tree->right, value);

            if (cm_binary_tree_get_height(tree->left)
              - cm_binary_tree_get_height(tree->right) == 2)
            {
                if (cm_binary_tree_get_height(tree->left->left)
                  > cm_binary_tree_get_height(tree->left->right))
                {
                    tree = cm_single_rotation_with_left(tree);
                }
                else
                {
                    tree = cm_double_rotation_with_left(tree);
                }
            }            
        }
        else
        {
            Position p;

            if (tree->left != 0 && tree->right != 0)
            {
                p = cm_binary_tree_find_min(tree->right);
                tree->value = p->value;
                cm_binary_tree_delete(tree->right, p->value);
            }
            else if (tree->left == 0)
            {
                p = tree;
                tree = tree->right;
                CM_FREE(p);
            }
            else
            {
                p = tree;
                tree = tree->left;
                CM_FREE(p);
            }
        }

        if (tree != 0)
        {
            S32 l_height, r_height;

            l_height = cm_binary_tree_get_height(tree->left);
            r_height = cm_binary_tree_get_height(tree->right);
            tree->height = CM_MAX(l_height, r_height) + 1;
        }
    }
    
    return tree;
}

S32 cm_binary_tree_retrieve_data(Position node)
{
    CM_ASSERT(node != 0);

    return node->value;
}

Position cm_binary_tree_find(SearchTree tree, S32 value)
{
    Position pos = 0;

    CM_ASSERT(tree != 0);
    if (tree != 0)
    {
        if (value < tree->value)
        {
            pos = cm_binary_tree_find(tree->left, value);
        }
        else if (value > tree->value)
        {
            pos = cm_binary_tree_find(tree->right, value);
        }
        else
        {
            pos = tree;
        }
    }
    
    return pos;
}

Position cm_binary_tree_find_min(SearchTree tree)
{
    Position min = 0;

    CM_ASSERT(tree != 0);

#if CM_USE_RECURSIVE
    if (tree != 0)
    {
        if (tree->left == 0)
        {
            min = tree;
        }
        else
        {
            min = cm_binary_tree_find_min(tree->left);
        }
    }
#else
    if (tree != 0)
    {
        while (tree->left != 0)
        {
            tree = tree->left;
        }

        min = tree;
    }
#endif

    return min;
}

Position cm_binary_tree_find_max(SearchTree tree)
{
    Position max = 0;

    CM_ASSERT(tree != 0);

#if CM_USE_RECURSIVE
    if (tree != 0)
    {
        if (tree->right == 0)
        {
            max = tree;
        }
        else
        {
            max = cm_binary_tree_find_max(tree->right);
        }
    }
#else
    if (tree != 0)
    {
        while (tree->right != 0)
        {
            tree = tree->right;
        }

        max = tree;
    }
#endif

    return max;
}

S32 cm_binary_tree_max_depth(SearchTree tree)
{
    if (tree == 0 || ((tree->left == 0) && (tree->right == 0)))
    {
        return 0;
    }
    else
    {
        S32 left = cm_binary_tree_max_depth(tree->left);
        S32 right = cm_binary_tree_max_depth(tree->right);
        return 1 + CM_MAX(left, right);
    }
}

void cm_binary_tree_print(SearchTree tree)
{
    if (tree != 0)
    {
        S32 i = 0;
        cm_binary_tree_layout(tree, 0);

        while (1)
        {
            if (cm_binary_tree_print_level(tree, i) == 0)
            {
                break;
            }

            CM_TRACE_5("\n");
            i++;
        }
    }
}

CM_POINT cm_binary_tree_print_ext(SearchTree tree, S32 x, S32 y, 
                                  S32 left, S32 *r,
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
        if (tree->left != 0)
        {
            x = x - CM_HORIZONTAL_PAD > left ? x - CM_HORIZONTAL_PAD : left + CM_HORIZONTAL_PAD;

            l_pos = cm_binary_tree_print_ext(tree->left, 
                                           x,
                                           y + CM_HORIZONTAL_PAD,
                                           left,
                                           &border,
                                           hdc);
        }

        if (tree->right != 0)
        {
            pos = cm_binary_tree_right_node_pos(x, y, &l_pos, CM_HORIZONTAL_PAD);

            r_pos = cm_binary_tree_print_ext(tree->right, pos.x, pos.y, border + CM_HORIZONTAL_PAD, &border, hdc);

            *r = r_pos.x + CM_NODE_RADIUS;
        }

        pos = cm_binary_tree_mid_node_pos(x, y, &l_pos, &r_pos, CM_HORIZONTAL_PAD);        
        
        pos = cm_binary_tree_draw_node(tree, &pos, hdc);
		*r = pos.x > border ? pos.x : border;

        if (tree->left != 0)
        {
            cm_gdi_draw_line(hdc, &l_pos, &pos);
        }

        if (tree->right != 0)
        {
            cm_gdi_draw_line(hdc, &pos, &r_pos);
        }
    }

    return pos;
}


/*
 * cm_huffman.c
 *
 *  Created on: 10 May 2012
 *      Author: samuel.song.bc@gmail.com
 */


#include "cm_huffman.h"

#include "cm_tree_utility.h"
#include "cm_priority_queue.h"

#include "cm_io.h"
#include "cm_lib.h"
#include "cm_debug.h"


typedef struct
{
    S8 symbol;
    S32 count;

} CMSymbolInfo;

typedef struct CMHuffmanNode_
{
    S8 symbol;
    S32 count;

    struct CMHuffmanNode_ *left;
    struct CMHuffmanNode_ *right;

}CMHuffmanNode;


static S32 cm_huffman_compare(CMTreeNode d1, CMTreeNode d2);
static void cm_huffman_print(CMTreeNode d, S8 *buffer);
static CMTreeNode cm_huffman_get_left(CMTree tree);
static CMTreeNode cm_huffman_get_right(CMTree tree);

static CMHuffmanNode *cm_create_huffman_node(S8 symbol, S32 count);
static CMHuffmanNode *cm_merge_huffman_node(CMHuffmanNode *n1, CMHuffmanNode *n2);


static S32 cm_huffman_compare(CMPQueueData d1, CMPQueueData d2)
{
    CMHuffmanNode *p1 = (CMHuffmanNode *)d1;
    CMHuffmanNode *p2 = (CMHuffmanNode *)d2;

    if (p1 == 0)
    {
        return -1;
    }
    else if (p2 == 0)
    {
        return 1;
    }
    else
    {
        return p1->count - p2->count;
    }
}

static void cm_huffman_print(CMTreeNode d, S8 *buffer)
{
    CMHuffmanNode *p = (CMHuffmanNode *)d;

    cm_sprintf(buffer, "%c", p->symbol);
}

static CMTreeNode cm_huffman_get_left(CMTree tree)
{
    CMHuffmanNode *p = (CMHuffmanNode *)tree;

    return p->left;
}

static CMTreeNode cm_huffman_get_right(CMTree tree)
{
    CMHuffmanNode *p = (CMHuffmanNode *)tree;

    return p->right;
}


static CMHuffmanNode *cm_create_huffman_node(S8 symbol, S32 count)
{
    CMHuffmanNode *p = (CMHuffmanNode *)CM_MALLOC(sizeof(CMHuffmanNode));

    if (p != 0)
    {
        p->symbol = symbol;
        p->count = count;
        p->left = p->right = 0;
    }
    
    return p;
}

static CMHuffmanNode *cm_merge_huffman_node(CMHuffmanNode *n1, CMHuffmanNode *n2)
{
    CMHuffmanNode *p = (CMHuffmanNode *)CM_MALLOC(sizeof(CMHuffmanNode));

    if (p != 0)
    {
        p->symbol = '0';
        p->count = n1->count + n2->count;
        p->left = n1;
        p->right = n2;
    }
    
    return p;
}


void cm_huffman_encode_test(void *hdc)
{
    S32 i, cnt;
    CMPQueue queue;
    CMTreeFunction callback;
    
    CMHuffmanNode *t1 = 0, *t2 = 0;
    
    CMSymbolInfo data[] =
    {
        {'a', 10},
        {'e', 15},
        {'i', 12},
        {'s', 3},
        {'t', 4},
        {'1', 13},  //for space
        {'2', 1}    //for new line
    };

    cnt = sizeof(data) / sizeof(CMSymbolInfo);
    
    queue = cm_p_queue_create(cnt, cm_huffman_compare);
    
    for (i = 0; i < cnt; i++)
    {
        CMHuffmanNode *node = cm_create_huffman_node(data[i].symbol, data[i].count);
        cm_p_queue_enqueue(queue, (CMPQueueData)node);
    }

    if (!cm_p_queue_is_empty(queue))
    {
        while (1)
        {
            t1 = (CMHuffmanNode *)cm_p_queue_delete_min(queue);

            if (cm_p_queue_is_empty(queue))
            {
                break;  //t1 is the finally tree
            }
            else
            {
                CMHuffmanNode* tmp;

                t2 = (CMHuffmanNode *)cm_p_queue_delete_min(queue);
                tmp = cm_merge_huffman_node(t2, t1);
                cm_p_queue_enqueue(queue, (CMPQueueData)tmp);
            }
        }

        callback.compare = cm_huffman_compare;
        callback.print = cm_huffman_print;
        callback.getLeft = cm_huffman_get_left;
        callback.getRight = cm_huffman_get_right;
        
        cm_binary_tree_print(t1, &callback, 20, 20, hdc);
    }
    
    cm_p_queue_destory(queue);
}


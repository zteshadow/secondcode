
#include "cm_graph.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_list.h"
#include "cm_queue.h"

#include "cm_debug.h"


#define CM_MAX_WEIGHT   (10000)
#define CM_NOT_VERTEX   (0)


typedef struct
{
    S32 vertext;
    S32 weight;
    
} CM_GRAPH_EDGE;

typedef struct
{
    void *adjacency;    //list

    S32 known;          //1 for known, 0 for unknown
    S32 weight;         //init to be -1
    S32 path;
    
} CM_GRAPH_ITEM;

typedef struct CM_GRAPH_STRUCT_EXT
{
    S32 cnt;
    CM_GRAPH_ITEM *vertex;

} CM_GRAPH_STRUCT;


static void *cm_graph_edge_create(S32 v, S32 w);
static S32 cm_graph_edge_get_vertex(void *edge);
static S32 cm_graph_edge_get_weight(void *edge);

static S32 *cm_graph_degree_init(CM_GRAPH g);
static S32 cm_graph_find_shortest_unknown(CM_GRAPH g);


static void *cm_graph_edge_create(S32 v, S32 w)
{
    CM_GRAPH_EDGE *p = (CM_GRAPH_EDGE *)CM_MALLOC(sizeof(CM_GRAPH_EDGE));

    if (p != 0)
    {
        p->vertext = v;
        p->weight = w;
    }

    return p;
}

static S32 *cm_graph_degree_init(CM_GRAPH g)
{
    S32 i, cnt = g->cnt;
    
    S32 *degree = (S32 *)CM_MALLOC(sizeof(S32) * cnt);
    if (degree != 0)
    {
        cm_memset(degree, 0, sizeof(S32) * cnt);

        for (i = 0; i < cnt; i++)
        {
            void *v;
            CM_GRAPH_ITEM *p = g->vertex + i;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                degree[cm_graph_edge_get_vertex(v) - 1] ++;
            }
        }
    }

    return degree;
}

static S32 cm_graph_find_shortest_unknown(CM_GRAPH g)
{
    S32 i, cnt, shortest = CM_MAX_WEIGHT;

    CM_ASSERT(g != 0);

    cnt = g->cnt;
    if (g->vertex != 0)
    {
        for (i = 0; i < cnt; i++)
        {
            CM_GRAPH_ITEM *p = g->vertex + i;

            if (p->weight < CM_MAX_WEIGHT && p->known == 0)
            {
                break;
            }
        }
    }

    return (i == cnt) ? 0 : (i + 1);
}

static S32 cm_graph_edge_get_vertex(void *edge)
{
    CM_GRAPH_EDGE *p = (CM_GRAPH_EDGE *)edge;
    return p->vertext;
}

static S32 cm_graph_edge_get_weight(void *edge)
{
    CM_GRAPH_EDGE *p = (CM_GRAPH_EDGE *)edge;
    return p->weight;
}

CM_GRAPH cm_graph_init(S32 cnt)
{
    CM_GRAPH g;

    CM_ASSERT(cnt > 0);
    
    g = (CM_GRAPH)CM_MALLOC(sizeof(CM_GRAPH_STRUCT));

    if (g != 0)
    {
        g->vertex = (CM_GRAPH_ITEM *)CM_MALLOC(sizeof(CM_GRAPH_ITEM) * cnt);
        if (g->vertex != 0)
        {
            S32 i;

            for (i = 0; i < cnt; i++)
            {
                CM_GRAPH_ITEM *p = g->vertex + i;
                p->known = 0;
                p->weight = CM_MAX_WEIGHT;
                p->path = CM_NOT_VERTEX;
                p->adjacency = cm_list_create();
            }

            g->cnt = cnt;
        }
        else
        {
            CM_FREE(g);
            g = 0;
        }
    }
    
    return g;
}

void cm_graph_deinit(CM_GRAPH g)
{
    S32 i, cnt;

    CM_ASSERT(g != 0);

    cnt = g->cnt;
    if (g->vertex != 0)
    {
        for (i = 0; i < cnt; i++)
        {
            void *v;
            CM_GRAPH_ITEM *p = g->vertex + i;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                CM_FREE(v);
            }

            cm_list_destory(p->adjacency);
        }

        CM_FREE(g->vertex);
    }

    CM_FREE(g);
}

void cm_graph_connect(CM_GRAPH g, S32 i, S32 j)
{
    CM_ASSERT(g != 0);
    CM_ASSERT(i > 0 && i <= g->cnt && j > 0 && j <= g->cnt);

    if (g != 0 && i > 0 && i <= g->cnt && j > 0 && j <= g->cnt)
    {
        void *v;
        
        CM_GRAPH_ITEM *p = g->vertex + i - 1;
        
        v = cm_graph_edge_create(j, 0);
        
        cm_list_append_item(p->adjacency, (void *)v);
    }
}

void cm_graph_connect_weight(CM_GRAPH g, S32 i, S32 j, S32 w)
{
    CM_ASSERT(g != 0);
    CM_ASSERT(i > 0 && i <= g->cnt && j > 0 && j <= g->cnt);

    if (g != 0 && i > 0 && i <= g->cnt && j > 0 && j <= g->cnt)
    {
        void *v;
        
        CM_GRAPH_ITEM *p = g->vertex + i - 1;
        
        v = cm_graph_edge_create(j, w);
        
        cm_list_append_item(p->adjacency, (void *)v);
    }
}

void cm_graph_sort(CM_GRAPH g)
{
    S32 i;
    CM_ASSERT(g != 0);

    if (g != 0)
    {
        S32 total, *degree;
        void *queue;
        void *list = cm_list_create();  //for result

        total = g->cnt;
        queue = cm_queue_create((void *)total);
        degree = cm_graph_degree_init(g);

        for (i = 0; i < total; i++)
        {
            if (degree[i] == 0)
            {
                cm_queue_enqueue(queue, (void *)(i + 1));
            }
        }

        while (!cm_queue_is_empty(queue))
        {
            CM_GRAPH_ITEM *p;
            S32 v = (S32)cm_queue_dequeue(queue);

            cm_list_append_item(list, (void *)v);

            p = g->vertex + v - 1;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                if (--degree[cm_graph_edge_get_vertex(v) - 1] == 0)
                {
                    cm_queue_enqueue(queue, cm_graph_edge_get_vertex(v));
                }
            }
        }
        
        CM_FREE(degree);
        cm_queue_destory(queue);
        cm_list_print(list);
        cm_list_destory(list);
    }
}

S32 cm_graph_is_no_cycle(CM_GRAPH g)
{
    S32 i, total = 0;

    CM_ASSERT(g != 0);

    if (g != 0)
    {
        S32 total, *degree;
        void *queue;

        total = g->cnt;
        queue = cm_queue_create((void *)total);
        degree = cm_graph_degree_init(g);

        for (i = 0; i < total; i++)
        {
            if (degree[i] == 0)
            {
                cm_queue_enqueue(queue, (void *)(i + 1));
            }
        }

        total = 0;
        while (!cm_queue_is_empty(queue))                               //V
        {
            CM_GRAPH_ITEM *p;
            S32 v = (S32)cm_queue_dequeue(queue);

            total++;
            p = g->vertex + v - 1;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)      //E
            {
                if (--degree[cm_graph_edge_get_vertex(v) - 1] == 0)
                {
                    cm_queue_enqueue(queue, cm_graph_edge_get_vertex(v));
                }
            }
        }

        cm_queue_destory(queue);
        CM_FREE(degree);

        return (total == g->cnt) ? 1 : 0;
    }

    return 0;
}

//Unweighted Path Length
void cm_graph_find_UWPL(CM_GRAPH g, S32 index)
{
    S32 w, i;
    
    CM_ASSERT(g != 0 && index > 0 && index <= g->cnt);

    if (g != 0 && index > 0 && index <= g->cnt)
    {
        CM_GRAPH_ITEM *p = g->vertex + index - 1;

        p->weight = 0;
        p->path = CM_NOT_VERTEX;

        for (w = 0; w < g->cnt; w++)
        {
            for (i = 0; i < g->cnt; i++)
            {
                CM_GRAPH_ITEM *p = g->vertex + i;

                if (!p->known && p->weight == w)
                {
                    void *v;
                    CM_GRAPH_ITEM *tmp;
                    
                    cm_list_start_get_item(p->adjacency);
                    while ((v = cm_list_get_next_item(p->adjacency)) != 0)
                    {
                        tmp = g->vertex + cm_graph_edge_get_vertex(v) - 1;
                        if (tmp->weight == CM_MAX_WEIGHT)
                        {
                            tmp->weight = w + 1;
                            tmp->path= i + 1;
                        }
                    }

                    p->known = 1;
                }
            }
        }
    }
}

void cm_graph_find_UWPL_quick(CM_GRAPH g, S32 index)
{
    S32 w, i;
    
    CM_ASSERT(g != 0 && index > 0 && index <= g->cnt);

    if (g != 0 && index > 0 && index <= g->cnt)
    {
        CM_GRAPH_ITEM *p;
        void *queue = cm_queue_create((void *)g->cnt);
        CM_ASSERT(queue != 0);

        p = g->vertex + index - 1;

        p->weight = 0;
        p->path = CM_NOT_VERTEX;
        cm_queue_enqueue(queue, (void *)index);

        while (!cm_queue_is_empty(queue))
        {
            void *v;
            CM_GRAPH_ITEM *tmp;
            S32 vertex = (S32)cm_queue_dequeue(queue);

            p = g->vertex + vertex - 1;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                tmp = g->vertex + cm_graph_edge_get_vertex(v) - 1;
                if (tmp->weight == CM_MAX_WEIGHT)
                {
                    tmp->weight = p->weight + 1;
                    tmp->path= vertex;
                    cm_queue_enqueue(queue, cm_graph_edge_get_vertex(v));
                }
            }

            p->known = 1;
        }

        cm_queue_destory(queue);
    }
}

void cm_graph_find_WPL_Dijkstra(CM_GRAPH g, S32 index)
{
    S32 w, i;

    CM_ASSERT(g != 0 && index > 0 && index <= g->cnt);

    if (g != 0 && index > 0 && index <= g->cnt)
    {
        CM_GRAPH_ITEM *item = (g->vertex + index - 1);
        item->weight = 0;
        item->path = CM_NOT_VERTEX;

        while (1)                                                       //v
        {
            S32 i = cm_graph_find_shortest_unknown(g);                  //v

            if (i != CM_NOT_VERTEX)
            {
                S32 w;
                void *v;
                CM_GRAPH_ITEM *p, *tmp;

                p = g->vertex + i - 1;
                p->known = 1;

                cm_list_start_get_item(p->adjacency);
                while ((v = cm_list_get_next_item(p->adjacency)) != 0)  //E
                {
                    w = cm_graph_edge_get_weight(v);
                    tmp = g->vertex + cm_graph_edge_get_vertex(v) - 1;

                    if (tmp->weight > p->weight + w)
                    {
                        tmp->weight = p->weight + w;
                        tmp->path = i;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
}

void cm_graph_find_WPL_Dijkstra_no_cycle(CM_GRAPH g, S32 index)
{
    S32 i;

    CM_ASSERT(g != 0 && index > 0 && index <= g->cnt);

    if (g != 0 && index > 0 && index <= g->cnt)
    {
        S32 total, *degree;
        void *queue;
        CM_GRAPH_ITEM *p = (g->vertex + index - 1);

        p->weight = 0;
        p->path = CM_NOT_VERTEX;

        total = g->cnt;
        queue = cm_queue_create((void *)total);
        degree = cm_graph_degree_init(g);

        for (i = 0; i < total; i++)
        {
            if (degree[i] == 0)
            {
                cm_queue_enqueue(queue, (void *)(i + 1));
            }
        }

        while (!cm_queue_is_empty(queue))
        {
            void *v;
            S32 w, vertex = (S32)cm_queue_dequeue(queue);
            CM_GRAPH_ITEM *tmp;

            p = g->vertex + vertex - 1;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                w = cm_graph_edge_get_weight(v);
                tmp = g->vertex + cm_graph_edge_get_vertex(v) - 1;

                if (tmp->weight > p->weight + w)
                {
                    tmp->weight = p->weight + w;
                    tmp->path = i;
                }

                if (--degree[cm_graph_edge_get_vertex(v) - 1] == 0)
                {
                    cm_queue_enqueue(queue, cm_graph_edge_get_vertex(v));
                }
            }
        }
        
        CM_FREE(degree);
    }
}

void cm_graph_find_WPL_negative(CM_GRAPH g, S32 index)
{
    S32 w, i;
    
    CM_ASSERT(g != 0 && index > 0 && index <= g->cnt);

    if (g != 0 && index > 0 && index <= g->cnt)
    {
        CM_GRAPH_ITEM *p;
        void *queue = cm_queue_create((void *)g->cnt);
        CM_ASSERT(queue != 0);

        p = g->vertex + index - 1;

        p->weight = 0;
        p->path = CM_NOT_VERTEX;
        cm_queue_enqueue(queue, (void *)index);

        while (!cm_queue_is_empty(queue))
        {
            void *v;
            CM_GRAPH_ITEM *tmp;
            S32 vertex = (S32)cm_queue_dequeue(queue);

            p = g->vertex + vertex - 1;

            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                w = cm_graph_edge_get_weight(v);
                i = cm_graph_edge_get_vertex(v);
                tmp = g->vertex + i - 1;

                if (tmp->weight > p->weight + w)
                {
                    tmp->weight = p->weight + w;
                    tmp->path = vertex;

                    if (!cm_queue_item_exist(queue, (void *)i))
                    {
                        cm_queue_enqueue(queue, (void *)i);
                    }
                }
            }
        }        

        cm_queue_destory(queue);
    }
}

void cm_graph_print(CM_GRAPH g)
{
    S32 i, cnt;

    CM_ASSERT(g != 0);
    CM_ASSERT(g->cnt > 0 && g->vertex != 0);

    if (g != 0 && g->cnt > 0 && g->vertex != 0)
    {
        cnt = g->cnt;
        
        for (i = 0; i < cnt; i++)
        {
            void *v;
            CM_GRAPH_ITEM *p = g->vertex + i;

            CM_TRACE_0("vertex[%d](w: %d, path: %d):", i+1, p->weight, p->path);
            //cm_list_print(p->adjacency);
            cm_list_start_get_item(p->adjacency);
            while ((v = cm_list_get_next_item(p->adjacency)) != 0)
            {
                CM_TRACE_0("%d ", cm_graph_edge_get_vertex(v));
            }

            CM_TRACE_0("\n");
        }
    }
}



#include "cm_mem_check.h"
#include "cm_debug.h"
#include "cm_lib.h"
#include "cm_string.h"


// �ڴ�Խ����
#define FLAG_SIZE	        16		// �߽緶Χ��ǰ׺����׺��
#define FLAG		        '#'		// ����ַ�


typedef struct 
{
	void* p;
	U32 size;
	const S8* file;
	U32 line;

}CM_MEM_ITEM;


static CM_MEM_ITEM *cm_find_free_slot(void);
static CM_MEM_ITEM *baidu_find_pointer(void *p);
static S32 baidu_mem_border_smashed(void* p, const S8* file, U32 line);
static void baidu_dump_memory(void);


static CM_MEM_ITEM *s_mem_list;
static U32 s_mem_list_size;

static U32 s_malloc_cnt = 0;		// �������
static U32 s_free_cnt = 0;		    // �ͷŴ���

static U32 s_max_mem = 0;			// �ڴ�ʹ�õķ�ֵ
static U32 s_current_mem = 0;


static CM_MEM_ITEM *cm_find_free_slot(void)
{
	U32 i;
    CM_MEM_ITEM *list = s_mem_list, *item = 0;

    //find empty channel
	for (i = 0; i < s_mem_list_size; i++)
	{
		if (list[i].p == 0)
		{
		    item = list + i;
		    break;
        }
	}

    CM_ASSERT(item != 0);
	return item;
}

static CM_MEM_ITEM *baidu_find_pointer(void *p)
{
	U32 i;
    CM_MEM_ITEM *list = s_mem_list, *item = 0;

	for (i = 0; i < s_mem_list_size; i++)
	{
		if (list[i].p == p)
		{
		    item = list + i;
		    break;
        }
	}

	CM_ASSERT(item != 0);
	return item;
}

static S32 baidu_mem_border_smashed(void* p, const S8* file, U32 line)
{
	S32 i;
	for (i = 0; i < FLAG_SIZE; i++)
	{
		if (((S8*)p)[i] != FLAG)
		{
			cm_trace("ERROR: memory smashed: 0x%x line:%d file:%s\n ",p, line, file);
			CM_ASSERT(0);

			return 1;
		}
	}

	return 0;
}

static void baidu_dump_memory(void)
{
    //return;
    U32 i;

	cm_trace("memory leaks: %d\n", s_malloc_cnt - s_free_cnt);
	cm_trace("---------------------------------------------------------------------\n");
	
	for (i = 0; i < s_mem_list_size; i++)
	{
	    CM_MEM_ITEM *list = s_mem_list;
		if (list[i].p != 0)
		{
			cm_trace("\tmemory address[%d]:0x%x size:%d line:%d file:%s",i,
			                                list[i].p,
			                                list[i].size - (2 * FLAG_SIZE),
			                                list[i].line,
			                                list[i].file);
		}
	}
	cm_trace("---------------------------------------------------------------------\n");
}

void cm_mem_check_init(U32 max)
{
    s_mem_list_size = max;

    s_mem_list = (CM_MEM_ITEM *)cm_malloc(sizeof(CM_MEM_ITEM) * max);
    CM_ASSERT(s_mem_list != 0);

    if (s_mem_list)
    {
        cm_memset(s_mem_list, 0, (sizeof(CM_MEM_ITEM) * max));
    }
}

U32 cm_mem_check_size(U32 size)
{
	return 2 * FLAG_SIZE + size;        //���ͷ��β
}

//��ʱ��size�Ǽӹ�pad֮���size
void cm_mem_check_append(CM_POINTER *p, U32 size, const S8 *fname, U32 line)
{
    CM_MEM_ITEM *list = 0;
    void *ptr = *p;

	CM_ASSERT(p != 0 && *p != 0 && size > 0);

    list = cm_find_free_slot();
    CM_ASSERT(list != 0);

    if (list)
    {
		cm_memset(ptr, FLAG, FLAG_SIZE);
		cm_memset(((S8*)ptr + (size - FLAG_SIZE)), FLAG, FLAG_SIZE);

        list->p = ptr;
		list->size = size;
		list->file = fname;
		list->line = line;

		s_malloc_cnt++;
		s_current_mem += (size -  (2 * FLAG_SIZE));

		if(s_current_mem > s_max_mem)
		{
			s_max_mem = s_current_mem;

        #if 0 //for memory dump
			if (s_max_mem == 69552)
			{
			    baidu_dump_memory();
			}
        #endif
		}

        //����ƶ�ָ�룬��ʹ����ʹ��
		*p = (S8*)ptr + FLAG_SIZE;
    }
}

//file and line are not used here
void cm_mem_check_delete(CM_POINTER *ptr)
{
	CM_MEM_ITEM *list = 0;

    CM_ASSERT(ptr != 0 && *ptr != 0);    

    //��ǰ�ƶ�ָ�룬�õ�ԭʼ������ڴ��ַ
	*ptr = (S8*)*ptr - FLAG_SIZE;

    list = baidu_find_pointer(*ptr);
	if(baidu_mem_border_smashed(*ptr, list->file, list->line))  //�ϱ߽���
	{
	    CM_ASSERT(0);
		return;
	}

    // �±߽���
    if(baidu_mem_border_smashed((S8*)*ptr + (list->size - FLAG_SIZE), list->file, list->line))
    {
        CM_ASSERT(0);
    	return;
    }

    s_current_mem -= list->size;
    cm_memset(list, 0, sizeof(CM_MEM_ITEM));

    s_free_cnt++;
}

void cm_mem_check(void)
{
	cm_trace("memory usage:\n");
	cm_trace("\tallocated memory:%d\n", s_malloc_cnt);
	cm_trace("\tfree memory:%d\n", s_free_cnt);
	cm_trace("\tmax memory used:%d\n", s_max_mem);

	if (s_malloc_cnt == s_free_cnt)
	{
		cm_trace("memory is OK!\n");
		//return;
	}
	else
	{
	    baidu_dump_memory();
		CM_ASSERT(0);
	}
}

void cm_mem_check_deinit(void)
{
    s_mem_list_size = 0;
    CM_ASSERT(s_mem_list != 0);

    if (s_mem_list)
    {
        cm_free(s_mem_list);
    }
}


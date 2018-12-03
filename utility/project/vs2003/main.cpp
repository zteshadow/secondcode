
#include <stdio.h>
#include <string.h>
#include "cm_lib.h"
#include "cm_base64.h"


extern void cm_base64_map_create(char *data);

static void cm_base64_test(char *data, int size);


static void cm_base64_test(char *data, int size)
{
    static s_index = 0;

    printf("\ntest(%d):\n", s_index++);
    char *encode = cm_base64_encode(data, size);
    printf("encode: %s\n", encode);
    S32 len;
    char *decode = cm_base64_decode(encode, strlen(encode), &len);

    printf("decode: ");
    for (int i = 0; i < len; i++)
    {
        printf("0x%x ", decode[i]);
    }
    printf("\n");
    
    CM_FREE(encode);
    CM_FREE(decode);
}

int main(int argc, char **argv)
{
	int wait;

#if 0
	char map['z' + 1];

	cm_base64_map_create(map);

	printf("-------------------------------------------\n");
	for (int i = 0; i < 'z' + 1; i++)
	{
		printf("%2d, ", map[i]);
		if ((i + 1) %16 == 0)
		{
			printf("\n");
		}

	}
	printf("-------------------------------------------\n");
#endif

    char data1[] = {0x02};
    char data2[] = {0x02, 0x03};
    char data3[] = {0x02, 0x03, 0x04};
    char data4[] = {0x02, 0x03, 0x04, '5'};
    char data5[] = "womenshigongchanzhuyijiebanren";

    cm_base64_test(data1, sizeof(data1));
    cm_base64_test(data2, sizeof(data2));
    cm_base64_test(data3, sizeof(data3));
    cm_base64_test(data4, sizeof(data4));
    cm_base64_test(data5, sizeof(data5));

	printf("hello, world\n");
	scanf("%d", &wait);

	return 0;
}


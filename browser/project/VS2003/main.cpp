
#include <stdio.h>
#include "page.h"


int main(int argc, char **argv)
{
	int wait;
	printf("hello, world\n");

	NBK_Page* p = page_create(0);

	scanf("%d", &wait);
	return 0;
}
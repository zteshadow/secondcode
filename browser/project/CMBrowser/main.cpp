

#include <stdio.h>

#include "cm_core_dom.h"
#include "cm_utility.h"


int main(int argc, char **argv)
{
	int wait;
	//printf("hello world!\n");
	
	S8 *data = cm_read_file_string("example.htm");
	if (data != 0)
	{
		cm_core_dom_create(data);
	}
	
	scanf("%d", &wait);
	return 0;
}


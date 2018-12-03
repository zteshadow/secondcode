
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "..\..\server\sce\sce_win32_socket.h"
#include "..\..\server\access_server\accept_handler.h"
#include "..\..\server\sce\sce_select_reactor.h"


#define MAX_TEST_CONNECTION (102400)
#define TEST_BLOCK_LEN      (10)


static const S8 *key = 
     "POST / HTTP/1.1\r\nHost: www.imichat.com\r\nContent-Length: 17\r\n\r\nuser=mtk&pass=123";
    

void authen_write_one(sce_win32_socket& sock)
{
    S32 l = sock.write(key, strlen(key));
    printf("write ok\n");
}

void authen_write_n(sce_win32_socket& sock)
{
    S32 current = 0, max = strlen(key), len = 0, block = TEST_BLOCK_LEN;
    
    while (len < max)
    {
        if (max - len < TEST_BLOCK_LEN)
        {
            block = max - len;
        }
        
        S32 l = sock.write(key + current, block);
        printf("block: %d\n", block);
        
        len += l;
        current += l;
    }
    
    printf("write ok\n");
}

void authen_write_more_one(sce_win32_socket& sock)
{
    char data[2048];
    strcpy(data, key);
    S32 l = sock.write(data, 2048);
    printf("write ok\n");
}

void authen_write_more_n(sce_win32_socket& sock)
{
    S32 current = 0, max = strlen(key), len = 0;
    
    while (len < max)
    {        
        S32 l = sock.write(key + current, TEST_BLOCK_LEN);
        printf("block: %d\n", TEST_BLOCK_LEN);
        
        len += l;
        current += l;
    }
    
    printf("write ok\n");
}

S32 authen_test(void)
{
    sce_win32_socket sock;
    sock.create(SOCK_STREAM);

	printf("connecting...\n");
    sock.connect("127.0.0.1", 80);
    printf("connected ok\n");

    //authen_write_one(sock);
    authen_write_n(sock);
    //authen_write_more_one(sock);
    //authen_write_more_n(sock);
 
    char buffer[1024] = {0};

    S32 len, cur = 0;
    while ((len = sock.read(buffer + cur, 1024)) > 0)
    {
        cur += len;
        printf("read data: %d\n", len);
    }
	S32 error;
	if (len < 0)	//error
	{
		error = WSAGetLastError();		
		printf("error code: %d\n", error);
	
	}
    printf("%s\n", buffer);

    printf("connection closed by server\n");
	return 0;
}

S32 main(S32 argc, char **argv)
{
    sce_win32_socket::startup();

	for (S32 i = 0; i < MAX_TEST_CONNECTION; i++)
	{
        if (authen_test() < 0)
		{
			break;
		}
	}

    sce_win32_socket::cleanup();    

	S32 len;
	printf("\ntest done\npress 'q' key to quit\n");
	scanf("%d", &len);
}


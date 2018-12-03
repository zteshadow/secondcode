// algorithm_graphic.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "algorithm_graphic.h"

#include "cm_binary_tree.h"
#include "cm_priority_queue.h"
#include "cm_leftist_heap.h"
#include "cm_huffman.h"

#include "cm_lib.h"
#include "cm_io.h"

extern "C"
{
void cm_mem_check_init(U32 max);
void cm_mem_check(void);
void cm_mem_check_deinit(void);    
}

typedef struct
{
    int data;
    int useless;

} CMTestNode;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ALGORITHM_GRAPHIC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ALGORITHM_GRAPHIC);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ALGORITHM_GRAPHIC);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_ALGORITHM_GRAPHIC;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable
   cm_mem_check_init(10000);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

    
   return TRUE;
}

static void *cm_custom_makenode(S32 i)
{
    CMTestNode *p = (CMTestNode *)CM_MALLOC(sizeof(CMTestNode));

    if (p != 0)
    {
        p->data = i;
        p->useless = 100;
    }

    return (void *)p;
}

S32 cm_custom_compare(CMPQueueData d1, CMPQueueData d2)
{
    CMTestNode *p1 = (CMTestNode *)d1;
    CMTestNode *p2 = (CMTestNode *)d2;

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
        return p1->data - p2->data;
    }
}

void cm_custom_print(CMPQueueData d, S8 *buffer)
{
    CMTestNode *p = (CMTestNode *)d;

    cm_sprintf(buffer, "%d", p->data);
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			cm_mem_check();
			cm_mem_check_deinit();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		{
#if 0
			SearchTree t = 0;
			int border;

#if 0
			for (int i = 7; i > 0; i--)
			//for (int i = 1; i < 8; i++)
			{
				t = cm_binary_tree_insert(t, i);
			}
#else

			t = cm_binary_tree_insert(t, 5);
			t = cm_binary_tree_insert(t, 4);
			t = cm_binary_tree_insert(t, 3);
			t = cm_binary_tree_insert(t, 1);
			t = cm_binary_tree_insert(t, 2);
#endif
			cm_binary_tree_print_ext(t, 20, 20, 0, &border, hdc);
			t= cm_binary_tree_find(t, 2);
			cm_binary_tree_print_ext(t, 250, 20, 150, &border, hdc);
#endif
        //priority queue
#if 0
        {
        #if 1
            CMPQueue q = cm_p_queue_create(100, cm_custom_compare);
            if (q != 0)
            {
                int i, border;

				for (i = 15; i >0; i--)
				{
					cm_p_queue_enqueue(q, cm_custom_makenode(i));
				}
                //cm_p_queue_enqueue(q, 2);
                //cm_p_queue_enqueue(q, 3);

                //cm_p_queue_delete_min(q);

                cm_p_queue_print(q, 20, 20, hdc);
            }
        #else
            S32 border, data[] = {2, 3, 1, 5, 4};
            CMPQueue q = cm_p_queue_build(data, sizeof(data)/sizeof(S32));
            if (q)
            {
                cm_p_queue_print(q, 20, 20, 0,&border, hdc);
                //cm_p_queue_destory(q);
            }
        #endif
        }
#endif

#if 0
    //leftist heap
    {
        S32 border;
        CMLHeap h = 0;

        h = cm_l_heap_insert(h, 2);
        h = cm_l_heap_insert(h, 3);
        //h = cm_l_heap_insert(h, 4);

#if 0
		CMLHeap h2 = 0, p;
		h2 = cm_l_heap_insert(h2, 1);
		h2 = cm_l_heap_insert(h2, 5);
		h2 = cm_l_heap_insert(h2, 6);
#endif
		//p = cm_l_heap_merge(h, h2);
        
		h = cm_l_heap_delete_min(h, 0);
		//h = cm_l_heap_delete_min(h, 0);

        cm_l_heap_print(h, 20, 20, 0,&border, hdc);
    }
#endif

#if 1
    {
        cm_huffman_encode_test(hdc);
    }
#endif
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

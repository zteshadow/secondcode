
#ifndef _NBK_WEB_VIEW_H
#define _NBK_WEB_VIEW_H


#include "cm_gdi.h"
#include "bd_list.h"
#include "nbk_core.h"
#include "nbk_kernel.h"


struct _nbk_res_conn;
struct _nbk_connection;

typedef enum _nbk_wv_mode
{
	WV_MODE_UNKNOWN		=	-1,
	WV_MODE_DIRECT,
	WV_MODE_UCK,
	WV_MODE_FF_FULL,
	WV_MODE_FF_NARROW,
}nbk_wv_mode;


typedef void (*on_nbk_event)(void* user, S32 id, void *info);

typedef struct _nbk_web_view
{
	nbk_kernel_ptr kernel;

    S32 x, y;
	S32 width;
	S32 height;

	//void* iFocusNode;
	S32 iEventId;//reserved
	NNode *node_focus;
	NNode *node_input;//for input
	//nbk_wv_mode mode;

	NRect view_rect;

	//bd_graphics_ptr g;
	void *graphic;

	NSettings settings;
	NBK_History *history;

	//res connection list of this view
	bd_list *conn_list;

	on_nbk_event call_back;
	void *user;

}nbk_web_view, *nbk_web_view_ptr;


#ifdef __cplusplus
extern "C"
{
#endif


nbk_web_view_ptr nbk_web_view_create(const void *graphic, const CM_RECT *frame);
void nbk_web_view_destroy(nbk_web_view_ptr thiz);

nbk_kernel_ptr nbk_web_view_get_kernel(nbk_web_view_ptr thiz);

S32 nbk_web_view_width(nbk_web_view_ptr thiz);
S32 nbk_web_view_height(nbk_web_view_ptr thiz);

void nbk_web_view_load_url(nbk_web_view_ptr thiz, const S8 *url);
void nbk_web_view_set_mode(nbk_web_view_ptr thiz, nbk_wv_mode mode);

void nbk_web_view_load_file(nbk_web_view_ptr thiz, const S8 *path);
void nbk_web_view_load_data(nbk_web_view_ptr thiz, const S8 *path, S8 *data, S32 len);

void nbk_web_view_on_mouse_down(nbk_web_view_ptr thiz, S32 x, S32 y);
void nbk_web_view_on_mouse_up(nbk_web_view_ptr thiz, S32 x, S32 y);

void nbk_web_view_set_call_back(nbk_web_view_ptr thiz, on_nbk_event cb, void *user);

void nbk_web_view_check_focus(nbk_web_view_ptr thiz);

bd_bool nbk_web_view_forward(nbk_web_view_ptr thiz);
bd_bool nbk_web_view_backward(nbk_web_view_ptr thiz);
bd_bool nbk_web_view_refresh(nbk_web_view_ptr thiz);
bd_bool nbk_web_view_stop(nbk_web_view_ptr thiz);

void nbk_web_view_page_up(nbk_web_view_ptr thiz, S32 d);
void nbk_web_view_page_down(nbk_web_view_ptr thiz, S32 height);
void nbk_web_view_move_to_prev_focus(nbk_web_view_ptr thiz);
void nbk_web_view_move_to_next_focus(nbk_web_view_ptr thiz);
void nbk_web_view_click_focus(nbk_web_view_ptr thiz);

U32 nbk_web_view_move_offset(nbk_web_view_ptr thiz, S32 offset);
U32 nbk_web_view_set_cur_pos(nbk_web_view_ptr thiz, S32  cur_pos);

void nbk_web_view_update_area(nbk_web_view_ptr thiz, CM_RECT *update_disp_rect, CM_BOOL update);


//offset < 0: move to right
//offset > 0: move to left
U32 nbk_web_view_horizontal_move(nbk_web_view_ptr thiz, S32 x, S32 width, S32 offset);

void nbk_web_view_set_view_rect(nbk_web_view_ptr thiz, const CM_RECT *rect);
void nbk_web_view_render(nbk_web_view_ptr thiz, void *g);

NPage* nbk_web_view_get_page(nbk_web_view_ptr thiz);
S32 nbk_web_view_get_page_width(nbk_web_view_ptr thiz);
S32 nbk_web_view_get_page_height(nbk_web_view_ptr thiz);

const S8 *nbk_web_view_get_page_url(nbk_web_view_ptr thiz);
const U16 *nbk_web_view_get_page_title(nbk_web_view_ptr thiz);

void nbk_web_view_clear_cookies(nbk_web_view_ptr thiz);

void nbk_web_view_get_baidu_uid(nbk_web_view_ptr thiz, S8 *uid);
void nbk_web_view_get_baidu_ua(nbk_web_view_ptr thiz, S8 *ua);

void nbk_web_view_add_connection(nbk_web_view_ptr thiz, struct _nbk_connection *conn);
void nbk_web_view_remove_connection(nbk_web_view_ptr thiz, struct _nbk_connection *conn);
void nbk_web_view_sever_connection_by_page_id(nbk_web_view_ptr thiz, S32 pid);


#ifdef __cplusplus
}
#endif


#endif //_NBK_WEB_VIEW_H



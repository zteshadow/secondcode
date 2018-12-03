/*
 * upCmd.c
 *
 *  Created on: 2011-3-16
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "upCmd.h"
#include "loader.h"
#include "../tools/str.h"
#include "../tools/dump.h"
#include "../dom/attr.h"
#include "../dom/node.h"
#include "../dom/xml_tags.h"
#include "../dom/xml_atts.h"
#include "../render/renderInput.h"
#include "../render/renderSelect.h"
#include "../render/renderTextarea.h"

#define DEBUG_IA_FORMDATA   0

#define BUF_GROW    2048
#define CRLF        "\r\n"

enum NUpCommandId {
    UPCMD_URL = 1,
    UPCMD_IA_BASIC = 7,
    UPCMD_IA_FORMDATA = 25,
    UPCMD_IA_SESSION = 33,
    UPCMD_IA_FRAME_BEGIN = 10025,
    UPCMD_IA_FRAME_END = 10026,
    UPCMD_IA_SETTINGS = 34,
    UPCMD_IA_PARAMS = 26,
    UPCMD_IA_DIALOG = 36,
    UPCMD_IA_FILE_UPLOAD = 27,
    UPCMD_MULTIPICS = 35,
    UPCMD_FORM_SUBMIT = 4,
    UPCMD_FORM_FIELDS = 30,
    UPCMD_FORM_TEXTAREA = 21,
    UPCMD_FILE_UPLOAD_FIELD = 23,
    UPCMD_FILE_UPLOAD_DATA = 24
};

typedef struct _NHeader {
    uint32  id;
    uint32  len;
} NHeader;

NUpCmd* upcmd_create(NUpCmdSet* settings)
{
    NUpCmd* cmd = (NUpCmd*)NBK_malloc0(sizeof(NUpCmd));

    if (settings) {
        uint32 s = NEFFS_COMPRESS | NEFFS_STREAM;
        
        if (settings->image)
            s |= NEFFS_ALLOW_IMAGE;
        if (settings->incMode)
            s |= NEFFS_INCREASE_MODE;
        
        if (settings->via == NEREV_FF_NARROW)
            s |= NEFFS_FF_NARROW;
        else if (settings->via == NEREV_FF_FULL_XHTML)
            s |= NEFFS_FF_XHTML;
        
        upcmd_iaSetting(cmd, s);
    }
    
    return cmd;
}

void upcmd_delete(NUpCmd** cmd)
{
    NUpCmd* c = *cmd;
    if (c->buf)
        NBK_free(c->buf);
    NBK_free(c);
    *cmd = N_NULL;
}

static void upcmd_check_size(NUpCmd* cmd, int need)
{
    uint8* t;
    int a, m;
    
    a = cmd->total - cmd->cur;
    if (a >= need)
        return;
    
    m = cmd->total + need;
    while (cmd->total < m)
        cmd->total += BUF_GROW;
    
    t = cmd->buf;
    cmd->buf = (uint8*)NBK_malloc(cmd->total);
    
    if (t) {
        for (m=0; m < cmd->cur; m++)
            cmd->buf[m] = t[m];
        NBK_free(t);
    }
}

static void header_swap(NHeader* hdr)
{
    hdr->id = N_SWAP_UINT32(hdr->id);
    hdr->len = N_SWAP_UINT32(hdr->len);
}

void upcmd_url(NUpCmd* cmd, const char* url, const char* base)
{
    int l1, l2;
    uint8* p;
    NHeader hdr;
    
    if (url == N_NULL)
        return;
    
    l1 = NBK_strlen(url);
    l2 = (base) ? NBK_strlen(base) : 0;
    
    upcmd_check_size(cmd, sizeof(NHeader) + l1 + l2 + 4);

    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_URL;
    hdr.len = l1 + l2 + 4;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    if (base) {
        NBK_memcpy(p, (void*)base, l2);
        p += l2;
    }
    NBK_memcpy(p, CRLF, 2);
    p += 2;
    
    NBK_memcpy(p, (void*)url, l1);
    p += l1;
    NBK_memcpy(p, CRLF, 2);
    p += 2;
    
    cmd->cur = p - cmd->buf;
}

void upcmd_iaBasic(NUpCmd* cmd, const char* url, int32 nidx, uint32 type)
{
    int l;
    uint8* p;
    NHeader hdr;
    int size = 0;
    
    if (url == N_NULL)
        return;
    
    l = NBK_strlen(url);
    size = l + 1 + 4 + 4;
    upcmd_check_size(cmd, sizeof(NHeader) + size);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_IA_BASIC;
    hdr.len = size;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    NBK_memcpy(p, (void*)url, l);
    p += l;
    *p++ = 0;
    
    nidx = N_SWAP_UINT32(nidx);
    NBK_memcpy(p, &nidx, 4);
    p += 4;

    type = N_SWAP_UINT32(type);
    NBK_memcpy(p, &type, 4);
    p += 4;
    
    cmd->cur = p - cmd->buf;
}

static int asse_form_data(NSList* lst, uint8** buf)
{
    int total = 0, l;
    NNode* n = sll_first(lst);
    uint8* p = (buf) ? *buf : N_NULL;
    int32 nidx;
    char* value;
    
    while (n) {

        if (n->id == TAGID_INPUT) {
            NRenderInput* r = (NRenderInput*)n->render;
            nidx = attr_getValueInt32(n->atts, ATTID_TC_NIDX);

            if (nidx != -1 && r->type != NEINPUT_FILE) {
#if DEBUG_IA_FORMDATA
                dump_int(lst->test, nidx);
                dump_char(lst->test, "input", -1);
#endif
                total += 5;
                if (p) {
                    nidx = N_SWAP_UINT32(nidx);
                    NBK_memcpy(p, &nidx, 4);
                    p += 4;
                }
                
                if (r->type == NEINPUT_TEXT || r->type == NEINPUT_PASSWORD) {
                    l = renderInput_getEditTextLen(r);
                    total += l;
                    if (p) {
                        value = renderInput_getEditText(r);
                        if (value) {
#if DEBUG_IA_FORMDATA
                            dump_char(lst->test, value, -1);
#endif
                            NBK_memcpy(p, value, l);
                            NBK_free(value);
                            p += l;
                        }
                    }
                }
                else if (r->type == NEINPUT_CHECKBOX || r->type == NEINPUT_RADIO) {
                    total += 1;
                    if (p) {
                        if (r->checked)
                            NBK_memcpy(p, "1", 1);
                        else
                            NBK_memcpy(p, "0", 1);
                        p += 1;
                    }
                }
                else {
                    value = attr_getValueStr(n->atts, ATTID_VALUE);
                    if (value) {
                        l = NBK_strlen(value);
                        total += l;
                        if (p) {
#if DEBUG_IA_FORMDATA
                            dump_char(lst->test, value, -1);
#endif
                            NBK_memcpy(p, value, l);
                            p += l;
                        }
                    }
                }
                
                if (p)
                    *p++ = 0;
#if DEBUG_IA_FORMDATA
                dump_return(lst->test);
#endif
            }
        }
        else if (n->id == TAGID_SELECT) {
            nidx = attr_getValueInt32(n->atts, ATTID_TC_NIDX);
#if DEBUG_IA_FORMDATA
            dump_int(lst->test, nidx);
            dump_char(lst->test, "select", -1);
#endif
            if (nidx != -1) {
                NNode* m;
                total += 5;
                if (p) {
                    nidx = N_SWAP_UINT32(nidx);
                    NBK_memcpy(p, &nidx, 4);
                    p += 4;
                }
                
                m = n->child;
                while (m) {
                    if (m->id == TAGID_OPTION) {
                        if (attr_getValueBool(m->atts, ATTID_SELECTED_P)) {
                            value = attr_getValueStr(m->atts, ATTID_VALUE);
                            if (value) {
#if DEBUG_IA_FORMDATA
                                dump_char(lst->test, value, -1);
#endif
                                l = NBK_strlen(value);
                                total += l;
                                if (p) {
                                    NBK_memcpy(p, value, l);
                                    p += l;
                                }
                            }
                            break;
                        }
                    }
                    
                    m = m->next;
                }
                
                if (p)
                    *p++ = 0;
            }
#if DEBUG_IA_FORMDATA
            dump_return(lst->test);
#endif
        }
        else if (n->id == TAGID_TEXTAREA) {
            nidx = attr_getValueInt32(n->atts, ATTID_TC_NIDX);
#if DEBUG_IA_FORMDATA
            dump_int(lst->test, nidx);
            dump_char(lst->test, "textarea", -1);
#endif
            if (nidx != -1) {
                NRenderTextarea* r = (NRenderTextarea*)n->render;
                total += 5;
                if (p) {
                    nidx = N_SWAP_UINT32(nidx);
                    NBK_memcpy(p, &nidx, 4);
                    p += 4;
                }
                
                l = renderTextarea_getEditTextLen(r);
                total += l;
                if (p) {
                    value = renderTextarea_getEditText(r);
                    if (value) {
                        int i, j;
#if DEBUG_IA_FORMDATA
                        dump_char(lst->test, value, -1);
#endif
                        for (i=j=0; value[i]; i++, j++) {
                            if (value[i] == 0xa)
                                p[j++] = 0xd;
                            p[j] = value[i];
                        }
                        p += j;
                        NBK_free(value);
                    }
                    *p++ = 0;
                }
            }
#if DEBUG_IA_FORMDATA
            dump_return(lst->test);
#endif
        }
        
        n = sll_next(lst);
    }
    
    if (buf)
        *buf = p;
    return total;
}

void upcmd_iaFormData(NUpCmd* cmd, NSList* lst)
{
    int l;
    uint8* p;
    NHeader hdr;
    
#if DEBUG_IA_FORMDATA
    dump_char(lst->test, "IA form data: predict size", -1);
    dump_return(lst->test);
#endif
    
    l = asse_form_data(lst, N_NULL);
    if (l == 0)
        return;
    
    upcmd_check_size(cmd, sizeof(NHeader) + l);
    
#if DEBUG_IA_FORMDATA
    dump_char(lst->test, "IA form data: write", -1);
    dump_return(lst->test);
#endif
    
    p = cmd->buf + cmd->cur;

    hdr.id = UPCMD_IA_FORMDATA;
    hdr.len = l;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    asse_form_data(lst, &p);

    cmd->cur = p - cmd->buf;
    
#if DEBUG_IA_FORMDATA
    dump_flush(lst->test);
#endif
}

void upcmd_iaSession(NUpCmd* cmd, uint8* data, int32 len)
{
    NHeader hdr;
    uint8* p;
    
    upcmd_check_size(cmd, sizeof(NHeader) + len);
    
    p = cmd->buf + cmd->cur;

    hdr.id = UPCMD_IA_SESSION;
    hdr.len = len;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    NBK_memcpy(p, data, len);
    p += len;
    
    cmd->cur = p - cmd->buf;
}

void upcmd_iaFrameBegin(NUpCmd* cmd, char* name)
{
    NHeader hdr;
    int len = NBK_strlen(name);
    uint8* p;
    
    upcmd_check_size(cmd, sizeof(NHeader) + len + 1);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_IA_FRAME_BEGIN;
    hdr.len = len + 1;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    NBK_memcpy(p, name, len);
    p += len;
    *p++ = 0;
    
    cmd->cur = p - cmd->buf;
}

void upcmd_iaFrameEnd(NUpCmd* cmd)
{
    NHeader hdr;
    uint8* p;
    
    upcmd_check_size(cmd, sizeof(NHeader));
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_IA_FRAME_END;
    hdr.len = 0;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    cmd->cur = p - cmd->buf;
}

void upcmd_iaSetting(NUpCmd* cmd, uint32 settings)
{
    NHeader hdr;
    uint8* p;
    
    upcmd_check_size(cmd, sizeof(NHeader) + 4);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_IA_SETTINGS;
    hdr.len = 4;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    settings = N_SWAP_UINT32(settings);
    NBK_memcpy(p, &settings, 4);
    p += 4;
    
    cmd->cur = p - cmd->buf;
}

static int asse_params(NSList* lst, uint8** buf)
{
    int total = 0, l;
    NNode* n = sll_first(lst);
    uint8* p = (buf) ? *buf : N_NULL;
    int32 id;
    char *idstr, *value;
    
    while (n) {
        if (n->id == TAGID_PARAM) {
            idstr = attr_getValueStr(n->atts, ATTID_ID);
            value = attr_getValueStr(n->atts, ATTID_VALUE);
            id = NBK_atoi(idstr);
            if (id > 0 && value) {
                total += 5;
                if (p) {
                    id = N_SWAP_UINT32(id);
                    NBK_memcpy(p, &id, 4);
                    p += 4;
                }
                
                l = NBK_strlen(value);
                total += l;
                if (p) {
                    NBK_memcpy(p, value, l);
                    p += l;
                    *p++ = 0;
                }
            }
        }  
        n = sll_next(lst);
    }
    
    if (buf)
        *buf = p;
    return total;
}

void upcmd_iaParams(NUpCmd* cmd, NSList* lst)
{
    NHeader hdr;
    uint8* p;
    int l;
    
    l = asse_params(lst, N_NULL);
    if (l == 0)
        return;
    
    upcmd_check_size(cmd, sizeof(NHeader) + l);
    
    p = cmd->buf + cmd->cur;

    hdr.id = UPCMD_IA_PARAMS;
    hdr.len = l;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    asse_params(lst, &p);

    cmd->cur = p - cmd->buf;
}

static int asse_pics_url(NSList* lst, uint8** buf)
{
    int total = 0, l;
    char* url = sll_first(lst);
    uint8* p = (buf) ? *buf : N_NULL;
    
    while (url) {
        l = NBK_strlen(url);
        total += l + 2;
        if (p) {
            NBK_memcpy(p, url, l);
            p += l;
            *p++ = '\r';
            *p++ = '\n';
        }
        url = sll_next(lst);
    }
    
    if (buf)
        *buf = p;
    return total;
}

void upcmd_multiPics(NUpCmd* cmd, NSList* lst)
{
    NHeader hdr;
    uint8* p;
    int l;
    
    l = asse_pics_url(lst, N_NULL);
    if (l == 0)
        return;
    
    upcmd_check_size(cmd, sizeof(NHeader) + l);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_MULTIPICS;
    hdr.len = l;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    asse_pics_url(lst, &p);
    
    cmd->cur = p - cmd->buf;
}

void upcmd_formSubmit(NUpCmd* cmd, char* base, char* action,
    char* charset, char* method, char* enctype)
{
    NHeader hdr;
    uint8* p;
    
    int baseL = (base) ? NBK_strlen(base) : 0;
    int actionL = (action) ? NBK_strlen(action) : 0;
    int charsetL = (charset) ? NBK_strlen(charset) : 0;
    int methodL = (method) ? NBK_strlen(method) : 0;
    int enctypeL = (enctype) ? NBK_strlen(enctype) : 0;
    int total = baseL + 2 + actionL + 2 + charsetL + methodL + enctypeL + 4;
    
    upcmd_check_size(cmd, sizeof(NHeader) + total);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_FORM_SUBMIT;
    hdr.len = total;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    NBK_memcpy(p, base, baseL);
    p += baseL;
    NBK_memcpy(p, CRLF, 2);
    p += 2;
    
    NBK_memcpy(p, action, actionL);
    p += actionL;
    NBK_memcpy(p, CRLF, 2);
    p += 2;
    
    NBK_memcpy(p, charset, charsetL);
    p += charsetL;
    *p++ = ',';
    NBK_memcpy(p, method, methodL);
    p += methodL;
    *p++ = ',';
    NBK_memcpy(p, enctype, enctypeL);
    p += enctypeL;
    NBK_memcpy(p, CRLF, 2);
    p += 2;
    
    cmd->cur = p - cmd->buf;
}

static int asse_form_fields(NSList* lst, uint8** buf)
{
    int total = 0;
    int nameL, valueL;
    char *name, *value;
    uint8* p = (buf) ? *buf : N_NULL;
    NNode* n = sll_first(lst);
    
    while (n) {
        if (n->id == TAGID_INPUT) {
            NRenderInput* r = (NRenderInput*)n->render;
            if (r->type == NEINPUT_TEXT || r->type == NEINPUT_PASSWORD) {
                name = attr_getValueStr(n->atts, ATTID_NAME);
                if (name) {
                    nameL = NBK_strlen(name);
                    valueL = renderInput_getEditTextLen(r);
                    total += nameL + 1 + valueL + 2;
                    
                    if (p) {
                        NBK_memcpy(p, name, nameL);
                        p += nameL;
                        *p++ = '=';
                        
                        if (valueL) {
                            value = renderInput_getEditText(r);
                            NBK_memcpy(p, value, valueL);
                            NBK_free(value);
                            p += valueL;
                        }
                        
                        NBK_memcpy(p, CRLF, 2);
                        p += 2;
                    }
                }
            }
            else if (r->type == NEINPUT_FILE || r->type == NEINPUT_RESET) {
                // ignore this type
            }
            else {
                bd_bool skip = 0;
                
                if ((r->type == NEINPUT_CHECKBOX || r->type == NEINPUT_RADIO) && !r->checked)
                    skip = 1;

                if (!skip) {
                    name = attr_getValueStr(n->atts, ATTID_NAME);
                    value = attr_getValueStr(n->atts, ATTID_VALUE);
                    if (name) {
                        nameL = NBK_strlen(name);
                        valueL = (value) ? NBK_strlen(value) : 0;
                        total += nameL + 1 + valueL + 2;
                        
                        if (p) {
                            NBK_memcpy(p, name, nameL);
                            p += nameL;
                            *p++ = '=';
                            NBK_memcpy(p, value, valueL);
                            p += valueL;
                            NBK_memcpy(p, CRLF, 2);
                            p += 2;
                        }
                    }
                }
            }
        }
        else if (n->id == TAGID_SELECT) {
            NNode* m = n->child;
            name = attr_getValueStr(n->atts, ATTID_NAME);
            nameL = (name) ? NBK_strlen(name) : 0;
            while (name && m) {
                if (m->id == TAGID_OPTION) {
                    if (attr_getValueBool(m->atts, ATTID_SELECTED_P)) {
                        value = attr_getValueStr(m->atts, ATTID_VALUE);
                        valueL = (value) ? NBK_strlen(value) : 0;
                        total += nameL + 1 + valueL + 2;
                        
                        if (p) {
                            NBK_memcpy(p, name, nameL);
                            p += nameL;
                            *p++ = '=';
                            NBK_memcpy(p, value, valueL);
                            p += valueL;
                            NBK_memcpy(p, CRLF, 2);
                            p += 2;
                        }
                        break;
                    }
                }
                m = m->next;
            }
        }
        
        n = sll_next(lst);
    }
    
    if (buf)
        *buf = p;
    return total;
}

void upcmd_formFields(NUpCmd* cmd, NSList* lst)
{
    NHeader hdr;
    uint8* p;
    int total = 0;
    
    total = asse_form_fields(lst, N_NULL);
    
    upcmd_check_size(cmd, sizeof(NHeader) + total);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_FORM_FIELDS;
    hdr.len = total;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    asse_form_fields(lst, &p);
    
    cmd->cur = p - cmd->buf;
}

void upcmd_formTextarea(NUpCmd* cmd, char* name, char* text, int len)
{
    NHeader hdr;
    uint8* p;
    int nameL;

    if (name == N_NULL)
        return;
    
    nameL = NBK_strlen(name);
    
    hdr.id = UPCMD_FORM_TEXTAREA;
    hdr.len = nameL + 2 + len;
    
    upcmd_check_size(cmd, sizeof(NHeader) + hdr.len);
    
    p = cmd->buf + cmd->cur;
    
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    NBK_memcpy(p, name, nameL);
    p += nameL;

    NBK_memcpy(p, CRLF, 2);
    p += 2;
 
    NBK_memcpy(p, text, len);
    p += len;
    
    cmd->cur = p - cmd->buf;
}

int upcmd_fileUploadField(char* name, char* path, uint8** data)
{
    NHeader hdr;
    char* p;
    int size;
    
    // get file name from path
    p = path + NBK_strlen(path) - 1;
    while (p >= path && *p != '\\' && *p != '/')
        p--;
    p++;
    
    hdr.id = UPCMD_FILE_UPLOAD_FIELD;
    hdr.len = NBK_strlen(name) + 2 + NBK_strlen(p) + 2;
    
    size = sizeof(NHeader) + hdr.len;
    
    if (data) {
        uint8* q = (uint8*)NBK_malloc(size);
        int l;
        
        *data = q;
        
        l = sizeof(NHeader);
        header_swap(&hdr);
        NBK_memcpy(q, &hdr, l);
        q += l;
        
        l = NBK_strlen(name);
        NBK_memcpy(q, name, l);
        q += l;
        
        NBK_memcpy(q, CRLF, 2);
        q += 2;
        
        l = NBK_strlen(p);
        NBK_memcpy(q, p, l);
        q += l;
        
        NBK_memcpy(q, CRLF, 2);
    }
    
    return size;
}

int upcmd_fileUploadData(int filesize, uint8** data)
{
    NHeader hdr;
    
    hdr.id = UPCMD_FILE_UPLOAD_DATA;
    hdr.len = filesize;
    
    if (data) {
        uint8* p = (uint8*)NBK_malloc(sizeof(NHeader));
        
        *data = p;
        
        header_swap(&hdr);
        NBK_memcpy(p, &hdr, sizeof(NHeader));
    }
    
    return sizeof(NHeader);
}

int upcmd_iaFileUpload(int nidx, char* path, int filesize, uint8** data)
{
    NHeader hdr;
    int l;
    int size;
    char* fn;
    
    // get file name from path
    fn = path + NBK_strlen(path) - 1;
    while (fn >= path && *fn != '\\' && *fn != '/')
        fn--;
    fn++;

    l = NBK_strlen(fn);
    
    hdr.id = UPCMD_IA_FILE_UPLOAD;
    hdr.len = 4 + l + 1 + filesize;
    
    size = sizeof(NHeader) + hdr.len - filesize;
    
    if (data) {
        uint8* p = (uint8*)NBK_malloc(size);
        *data = p;
        
        header_swap(&hdr);
        NBK_memcpy(p, &hdr, sizeof(NHeader));
        p += sizeof(NHeader);
        
        nidx = N_SWAP_UINT32(nidx);
        NBK_memcpy(p, &nidx, 4);
        p += 4;
        
        NBK_memcpy(p, fn, l);
        p += l;
        *p++ = 0;
    }
    
    return size;
}

void upcmd_iaDialog(NUpCmd* cmd, int type, int choose, char* input)
{
    int l;
    uint8* p;
    NHeader hdr;
    int size = 0;
    int16 i16;
    
    l = (input) ? NBK_strlen(input) : 0;
    size = 2 + 2 + l;
    upcmd_check_size(cmd, sizeof(NHeader) + size);
    
    p = cmd->buf + cmd->cur;
    
    hdr.id = UPCMD_IA_DIALOG;
    hdr.len = size;
    header_swap(&hdr);
    NBK_memcpy(p, &hdr, sizeof(NHeader));
    p += sizeof(NHeader);
    
    i16 = N_SWAP_UINT16((int16)type);
    NBK_memcpy(p, &i16, 2);
    p += 2;
    
    i16 = N_SWAP_UINT16((int16)choose);
    NBK_memcpy(p, &i16, 2);
    p += 2;
    
    if (input) {
        NBK_memcpy(p, input, l);
        p += l;
    }
    
    cmd->cur = p - cmd->buf;
}

#define MULTIPART_BOUNDARY  "Baidu2011NBK"
#define MULTIPART_PREFIX    "--"

char* multipart_contentType(void)
{
    const char* head = "multipart/form-data; boundary=";
    char* type = (char*)NBK_malloc(64);
    NBK_strcpy(type, head);
    NBK_strcpy(type + NBK_strlen(head), MULTIPART_BOUNDARY);
    return type;
}

int multipart_field(const char* name, const char* value, char* body)
{
    const char* head = "Content-Disposition: form-data; name=\"";
    int size, pl, bl, cl, nl, vl;
    
    pl = NBK_strlen(MULTIPART_PREFIX);
    bl = NBK_strlen(MULTIPART_BOUNDARY);
    cl = NBK_strlen(CRLF);
    
    nl = (name) ? NBK_strlen(name) : 0;
    vl = (value) ? NBK_strlen(value) : 0;

    size = 0;
    size += pl + bl + cl;
    size += NBK_strlen(head);
    size += nl + 1;
    size += cl * 2;
    size += vl + cl;
    
    if (body) {
        char* p = body;
        NBK_strcpy(p, MULTIPART_PREFIX);
        p += pl;
        NBK_strcpy(p, MULTIPART_BOUNDARY);
        p += bl;
        NBK_strcpy(p, CRLF);
        p += cl;
        NBK_strcpy(p, head);
        p += NBK_strlen(head);
        if (name) {
            NBK_strcpy(p, name);
            p += nl;
        }
        *p++ = '"';
        NBK_strcpy(p, CRLF);
        p += cl;
        NBK_strcpy(p, CRLF);
        p += cl;
        if (value) {
            NBK_strcpy(p, value);
            p += vl;
        }
        NBK_strcpy(p, CRLF);
    }
    
    return size;
}

int multipart_file(const char* name, const char* value, char* body)
{
    const char* head = "Content-Disposition: form-data; name=\"";
    const char* fname = "; filename=\"";
    const char* ctype = "Content-Type: ";
    const char* ctype_text = "text/plain";
    const char* ctype_image = "image/";
    const char* ctype_image_jpeg = "jpeg";
    const char* ctype_image_gif = "gif";
    const char* ctype_image_png = "png";
    const char* enc_bin = "Content-Transfer-Encoding: binary";
   
    int size, pl, bl, cl, nl, vl;
    bd_bool bin = N_TRUE;
    char* subType = (char*)ctype_image_jpeg;

    pl = NBK_strlen(MULTIPART_PREFIX);
    bl = NBK_strlen(MULTIPART_BOUNDARY);
    cl = NBK_strlen(CRLF);

    nl = (name) ? NBK_strlen(name) : 0;
    vl = (value) ? NBK_strlen(value) : 0;
    
    if (value) {
        if (nbk_strfind_nocase(value, ".txt") != -1)
            bin = N_FALSE;
        else if (nbk_strfind_nocase(value, ".png") != -1)
            subType = (char*)ctype_image_png;
        else if (nbk_strfind_nocase(value, ".gif") != -1)
            subType = (char*)ctype_image_gif;
    }
    
    size = 0;
    size += pl + bl + cl;
    size += NBK_strlen(head) + nl + 1 + NBK_strlen(fname) + vl + 1 + cl;
    if (bin) {
        size += NBK_strlen(ctype) + NBK_strlen(ctype_image) + NBK_strlen(subType) + cl;
        size += NBK_strlen(enc_bin) + cl;
    }
    else {
        size += NBK_strlen(ctype) + NBK_strlen(ctype_text) + cl;
    }
    size += cl;
    
    if (body) {
        char* p = body;
        int len;
        
        len = NBK_sprintf(p, "%s%s%s", MULTIPART_PREFIX, MULTIPART_BOUNDARY, CRLF);
        p += len;
        len = NBK_sprintf(p, "%s%s\"%s%s\"%s", head, name, fname, value, CRLF);
        p += len;
        if (bin)
            NBK_sprintf(p, "%s%s%s%s%s%s%s",
                ctype, ctype_image, subType, CRLF, enc_bin, CRLF, CRLF);
        else
            NBK_sprintf(p, "%s%s%s%s", ctype, ctype_text, CRLF, CRLF);
    }
    
    return size;
}

int multipart_end(char* body)
{
    int size, pl, bl, cl;
    
    pl = NBK_strlen(MULTIPART_PREFIX);
    bl = NBK_strlen(MULTIPART_BOUNDARY);
    cl = NBK_strlen(CRLF);
    
    size = pl + bl + pl + cl;
    
    if (body) {
        char* p = body;
        NBK_sprintf(p, "%s%s%s%s", MULTIPART_PREFIX, MULTIPART_BOUNDARY, MULTIPART_PREFIX, CRLF);
    }
    
    return size;
}

static uint32 get_uint32(const uint8* data)
{
    uint32 v32 = 0;
    v32 = *data;
    v32 += *(data + 1) << 8;
    v32 += *(data + 2) << 16;
    v32 += *(data + 3) << 24;
    return N_SWAP_UINT32(v32);
}

bd_bool upcmd_isIa(NUpCmd* cmd)
{
    uint32 id, len;
    uint8* p = cmd->buf;
    uint8* toofar = p + cmd->cur;
    while (p < toofar) {
        id = get_uint32(p);
        p += 4;
        len = get_uint32(p);
        p += 4;
        if (id == UPCMD_IA_BASIC)
            return N_TRUE;
        p += len;
    }
    return N_FALSE;
}

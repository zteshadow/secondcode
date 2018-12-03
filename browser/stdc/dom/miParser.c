/*
 * sgmlParser.c
 *
 *  Created on: 2011-3-5
 *      Author: wuyulun
 */

#include "miParser.h"

#define USE_CHAR_REF    1

#if USE_CHAR_REF
#include "char_ref.h"
#include "xml_helper.h"
#include "../tools/str.h"
#include "../tools/unicode.h"

#define MI_ATOI(s)          NBK_atoi(s)
#define MI_HTOI(s)          NBK_htoi(s)
#define MI_U16TOU8(hz, cc)  uni_utf16_to_utf8(hz, cc)
#define MI_STRLEN(s)        NBK_strlen(s)
#define MI_STRCMP(s1, s2)   NBK_strcmp(s1, s2)
#define MI_STRNCMP_NOCASE(s1, s2, l)    NBK_strncmp_nocase(s1, s2, l)
#endif

#define MI_BUFSIZE      8000
#define MI_BUF_RESERVED 64
#define MI_MAX_ATTRS    16
#define MI_MAX_REF_LEN  8

#define MI_INVALID_ID   0xffff

#define IS_VISI(u)      (u > 0x20)
#define IS_NUM(c)       (c >= '0' && c <= '9')
#define IS_UPPER(c)     (c >= 'A' && c <= 'Z')
#define IS_LOWER(c)     (c >= 'a' && c <= 'z')
#define IS_LETTER(c)    (IS_UPPER(c) || IS_LOWER(c))
#define IS_UTF8_MB(u)   (u >= 0xc0)
#define IS_UTF8_2B(u)   (u >= 0xc0 && u < 0xe0)
#define IS_UTF8_3B(u)   (u >= 0xe0)
#define IS_TAG(c)       (c == '?' || c == '!' || c == '/' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

//static char* cdata_begin = "<![CDATA[";
//static char* cdata_end = "]]>";

enum MIPR_STATE {
    NO_TAG,
    TAGNAME,
    SEARCH_ATTR,
    ATTRNAME,
    SEARCH_EQUAL,
    SEARCH_VALUE,
    VALUE,
    SEARCH_END,
    TAG_END
};

enum MIPR_COMMENT {
    NO_COMMENT,
    COMMENT_TYPE1, // /* */
    COMMENT_TYPE2, // <!-- -->
    COMMENT_LAST
};

static void mipr_mem_copy(char* dst, const char* src, int length, mi_uint8 reve)
{
    if (dst == src) return;
    
    if (reve) {
        char* crash = MI_NULL;
        *crash = 88;
    }
    else {
        int i=0;
        while (i < length) {
            *dst++ = *src++;
            i++;
        }
    }
}

static void mipr_mem_zero(char* dst, int length)
{
    int i=0;
    while (i < length) {
        *dst++ = 0;
        i++;
    }
}

MiParser* miParser_create(MIPR_MEM* mem)
{
    MiParser* parser = (MiParser*)NBK_malloc(sizeof(MiParser));
    //parser->mem = *mem;
    parser->buf = (char*)NBK_malloc(MI_BUFSIZE);
    parser->attrs = (char**)NBK_malloc(sizeof(char*) * (MI_MAX_ATTRS * 2));
    miParser_reset(parser);
    return parser;
}

void miParser_delete(MiParser** parser)
{
    MiParser* p = *parser;
    NBK_free(p->buf);
    NBK_free(p->attrs);
    NBK_free(p);
    *parser = MI_NULL;
}

void miParser_reset(MiParser* parser)
{
    parser->h = parser->t = parser->buf;
    parser->handle_start_tag = MI_NULL;
    parser->handle_end_tag = MI_NULL;
    parser->handle_text = MI_NULL;
    parser->state = NO_TAG;
    parser->comment = NO_COMMENT;
    parser->text = MI_NULL;
    parser->tag = MI_NULL;
    parser->first = 1;
    parser->de = 0;
    parser->pass = 0;
    parser->abort = 0;
    parser->skipJs = 0;
}

static void mipr_parse_output_text(MiParser* parser, char* curr)
{
    if (parser->text && parser->handle_text)
        parser->handle_text(parser->user, parser->text, curr - parser->text);
    parser->h = curr;
    parser->pass = 0;
    parser->text = MI_NULL;
}

static void mipr_parse(MiParser* parser)
{
    mi_uint8* p = (mi_uint8*)parser->h + parser->pass;
    mi_uint8* tooFar = (mi_uint8*)parser->t;
    mi_uint8* bp = MI_NULL;
    mi_uint8* cref = MI_NULL;
    int move = 1;
    int s = sizeof(char*) * (MI_MAX_ATTRS * 2);
    
    while (p < tooFar && !parser->abort) {
        
        if (parser->skipJs) {
            if (*p == '<' && *(p+1) == '/')
                parser->skipJs = 0;
            else {
                p++;
                continue;
            }
        }
        
        if (parser->state == NO_TAG ) {
            
            if (parser->comment == COMMENT_TYPE1) {
                if (*p == '*') {
                    if (p + 1 == tooFar)
                        break;
                    if (*(p + 1) == '/') {
                        parser->comment = NO_COMMENT;
                        p += 2;
                        parser->pass = 0;
                        parser->h = (char*)p;
                        continue;
                    }
                }
                p++;
                parser->pass++;
                continue;
            }
            else {
                if (*p == '/') {
                    if (p + 1 == tooFar)
                        break;
                    if (*(p + 1) == '*') {
                        mipr_parse_output_text(parser, (char*)p);
                        parser->comment = COMMENT_TYPE1;
                        p += 2;
                        parser->pass += 2;
                        continue;
                    }
                }
            }
            
            if (parser->comment == COMMENT_TYPE2) {
                if (*p == '-') {
                    if (p + 2 >= tooFar)
                        break;
                    if (*(p+1) == '-' && *(p+2) == '>') {
                        parser->comment = NO_COMMENT;
                        p += 3;
                        parser->pass = 0;
                        parser->h = (char*)p;
                        continue;
                    }
                }
                p++;
                parser->pass++;
                continue;
            }
            else {
                if (*p == '<') {
                    if (p + 3 >= tooFar)
                        break;
                    if (*(p+1) == '!' && *(p+2) == '-' && *(p+3) == '-') {
                        mipr_parse_output_text(parser, (char*)p);
                        parser->comment = COMMENT_TYPE2;
                        p += 4;
                        parser->pass += 4;
                        continue;
                    }
                }
            }
            
            if (*p == '}') // end for a style decl
                bp = p;
        }
        
        if (*p == '<' && IS_TAG(*(p+1)) && !parser->de) {
            *p = 0;
            parser->state = TAGNAME;
            mipr_mem_zero((char*)parser->attrs, s);
            parser->aidx = -2;
            parser->close = parser->flat = 0;
            mipr_parse_output_text(parser, (char*)p);
        }
        else if (*p == '/' && !parser->de) {
            if (parser->state != NO_TAG) {
                *p = 0;
                parser->close = 1;
                parser->flat = (parser->tag) ? 1 : 0;
            }
        }
        else if (*p == '>' && !parser->de) {
            if (parser->state != NO_TAG) {
                *p = 0;
                parser->state = NO_TAG;
                if ((parser->close == parser->flat) && parser->handle_start_tag)
                    parser->handle_start_tag(parser->user, parser->tag, (const char**)parser->attrs);
                if (parser->close && parser->handle_end_tag)
                    parser->handle_end_tag(parser->user, parser->tag);
                if (MI_STRCMP(parser->tag, "script") == 0)
                    parser->skipJs = !parser->close;
                parser->text = MI_NULL;
                parser->tag = MI_NULL;
                parser->h = (char*)p + 1;
                parser->pass = -1;
            }
        }
        else if (*p == '=') {
            if (parser->state == SEARCH_EQUAL || parser->state == ATTRNAME) {
                *p = 0;
                parser->state = SEARCH_VALUE;
            }
        }
        else if (IS_VISI(*p)) {
            
            if (IS_UTF8_MB(*p)) {
                if (IS_UTF8_2B(*p))
                    move = 2;
                else if (IS_UTF8_3B(*p))
                    move = 3;
            }
            
            if (parser->state == NO_TAG) {
                // processing charref in content
                if (*p == '&') {
                    cref = p;
                }
                else if (cref && *p == ';') {
                    short cl = p - cref;
                    if (cl < MI_MAX_REF_LEN) {
                        short cnl = -1;
                        mi_uint8* repl = MI_NULL;
                        char hzs[4];
                        *p = 0;
                        
                        if (*(cref + 1) == '#') {
#if USE_CHAR_REF
                            uint16 hz;
                            if (*(cref + 2) == 'x' || *(cref + 2) == 'X')
                                hz = MI_HTOI((char*)cref + 3);
                            else
                                hz = MI_ATOI((char*)cref + 2);
                            if (hz < 0x20) hz = 0x20;
                            cnl = MI_U16TOU8(hz, hzs);
                            repl = (uint8*)hzs;
#else
                            cnl = 0;
#endif
                        }
                        else {
#if USE_CHAR_REF
                            mi_uint16 crid = binary_search_id(xml_getCharrefNames(), CHARREF_TOTAL, (char*)cref + 1);
                            if (crid != MI_INVALID_ID) {
                                cnl = MI_STRLEN(xml_getEntityNames()[crid]);
                                repl = (mi_uint8*)xml_getEntityNames()[crid];
                            }
                            else
#endif
                                cnl = 0;
                        }
                        
                        if (cnl != -1) {
                            int i;
                            char* pdes = (char*)cref + cnl;
                            char* psrc = (char*)p + 1;
                            i = (psrc < pdes) ? 1 : 0;
                            mipr_mem_copy(pdes, psrc, (char*)tooFar - psrc, (mi_uint8)i);
                            if (repl && repl[0] == 0x20 && repl[1] == 0)
                                *cref++ = 0x1; // replace &nbsp; to 0x1
                            else {
                            for (i=0; repl && repl[i]; i++)
                                *cref++ = repl[i];
                            }
                            i = p - cref + 1; // shift
                            p -= i;
                            parser->t -= i;
                            tooFar = (mi_uint8*)parser->t;
                            parser->pass -= i;
                            if (bp)
                                bp -= i;
                        }
                    }
                    cref = MI_NULL;
                }
            }
            else {
                switch (parser->state) {
                case TAGNAME:
                    if (parser->tag == MI_NULL) {
                        parser->tag = (char*)p;
                    }
                    if (IS_UPPER(*p))
                        *p += 32; // convert tag to lower
                    break;
                    
                case SEARCH_ATTR:
                case SEARCH_EQUAL:
                    if (parser->aidx < 0 || parser->attrs[parser->aidx])
                        parser->aidx += 2;
                    if (parser->aidx / 2 < MI_MAX_ATTRS - 1) {
                        if (parser->de == *p) {
                            // no attribute name
                            parser->de = 0;
                        }
                        else if (*p == '"' || *p == '\'') {
                            parser->de = *p;
                        }
                        else {
                            parser->attrs[parser->aidx] = (char*)p;
                            parser->state = ATTRNAME;
                            if (IS_UPPER(*p))
                                *p += 32; // convert attribute to lower
                        }
                    }
                    break;
                    
                case ATTRNAME:
                    if (*p == parser->de) {
                        *p = 0;
                        parser->state = SEARCH_EQUAL;
                        parser->de = 0;
                    }
                    else if (IS_UPPER(*p))
                        *p += 32;
                    break;
                    
                case SEARCH_VALUE:
                    if (parser->aidx / 2 < MI_MAX_ATTRS - 1) {
                        if (*p == '"' || *p == '\'') {
                            if (*p == parser->de) {
                                *p = 0;
                                parser->state = SEARCH_ATTR;
                                parser->de = 0;
                            }
                            else
                                parser->de = *p;
                        }
                        else {
                            parser->attrs[parser->aidx + 1] = (char*)p;
                            parser->state = VALUE;
                        }
                    }
                    break;
                    
                case VALUE:
                    if (*p == parser->de) {
                        *p = 0;
                        parser->state = SEARCH_ATTR;
                        p++;
                        if (p < tooFar && *p == parser->de) // fix case: name="value""
                            *p = ' ';
                        p--;
                        parser->de = 0;
                    }
                    break;
                }
            }
        }
        else {
            switch (parser->state) {
            case TAGNAME:
                *p = 0;
                parser->state = SEARCH_ATTR;
                break;
                
            case ATTRNAME:
                if (parser->de == 0) {
                    *p = 0;
                    parser->state = SEARCH_EQUAL;
                }
                break;
                
            case VALUE:
                if (parser->de == 0) {
                    *p = 0;
                    parser->state = SEARCH_ATTR;
                }
                break;
            }
            
            if (parser->state == NO_TAG && (*p == 0xd || *p == 0xa)) {
                mi_uint8 vi = 0;
                mi_uint8* q = (mi_uint8*)parser->text;
                while (q && q < p && !vi) {
                    if (*q != 0xd && *q != 0xa)
                        vi = 1;
                    q++;
                }
                if (vi)
                    mipr_parse_output_text(parser, (char*)p);
            }
        }
        
        if (parser->state == NO_TAG && *p && parser->text == MI_NULL) {
            parser->text = (char*)p;
        }
        
        if (p + move > tooFar) {
            if (parser->h != parser->buf)
                parser->pass -= tooFar - p;
            break;
        }
        else {
        p += move;
        parser->pass += move;
        }
        move = 1;
    }
    
    if (cref && p - cref < MI_MAX_REF_LEN) {
        parser->pass -= p - cref;
    }
    
    if (parser->h == parser->buf) {
        if (parser->state == NO_TAG) {
            mipr_parse_output_text(parser, (bp) ? (char*)bp+1 : (char*)p);
        }
    }
}

static void mipr_shift(MiParser* parser)
{
    int s, m, i;
    
    if (parser->h > parser->buf) {
        s = parser->t - parser->h;
        mipr_mem_copy(parser->buf, parser->h, s, 0);
        m = parser->h - parser->buf;
        parser->h = parser->buf;
        parser->t = parser->h + s;
        
        if (parser->text)
            parser->text -= m;
        if (parser->tag)
            parser->tag -= m;
        for (i=0; parser->attrs[i]; i += 2) {
            parser->attrs[i] -= m;
            if (parser->attrs[i+1])
                parser->attrs[i+1] -= m;
        }
    }
}

void miParser_write(MiParser* parser, const char* data, int length)
{
    char* tooFar = parser->buf + MI_BUFSIZE - MI_BUF_RESERVED;
    char* da = (char*)data;
    int total = length;
    int n;
    
    if (parser->first) {
        mi_uint8* p = (mi_uint8*)data;
        parser->first = 0;
        if (*p == 0xef && *(p+1) == 0xbb && *(p+2) == 0xbf) { 
            da += 3;
            total -= 3;
        }
    }

    while (total && !parser->abort) {
        n = tooFar - parser->t;
        if (n <= 0)
            break;
        n = (total > n) ? n : total;
        mipr_mem_copy(parser->t, da, n, 0);
        parser->t += n;
        mipr_parse(parser);
        if (parser->h == parser->buf)
            break;
        mipr_shift(parser);
        da += n;
        total -= n;
    }
}

void miParser_abort(MiParser* parser)
{
    parser->abort = 1;
}

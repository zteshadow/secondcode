/*
 * cm_parser.c
 *
 *  Created on: 2012-3-16
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_core_parser.h"

#include "cm_debug.h"
//#include "cm_io.h"
#include "cm_lib.h"


typedef enum
{
    DATA_STATE,
    TEXT_STATE,
    TAG_OPEN,
    TAG_NAME,
    TAG_CLOSE,
    P_NAME,             //for property
    P_VALUE_OPEN,
    P_VALUE,
    P_CLOSE,

    STATE_MAX
    
} CMParserState;

typedef enum
{
    TAG_STATE_IDLE,
    TAG_STATE_OPEN,
    TAG_STATE_CLOSE

} TagState;

typedef struct
{
    CMParserState state;
    TagState tag_state;
    S8 *value;
    
} CMParserStruct;


S32 cm_data_parser(S8 *data, PARSER_FUNC cb, void *usr_data)
{
    S8 ch, *p = data;
    CMParserStruct ctrl = {0};

    ctrl.state = DATA_STATE;
    while ((ch = *p) != 0)
    {
        switch(ctrl.state)
        {
        case DATA_STATE:
        {
            if (ch == '<')
            {
                ctrl.state = TAG_OPEN;
                ctrl.tag_state = TAG_STATE_OPEN;
            }
            else
            {
                if (cm_isalpha(ch))
                {
                    ctrl.value = p;
                    ctrl.state = TEXT_STATE;
                }
            }
        }
        break;

        case TEXT_STATE:
        {
            if (ch == '<')
            {
                ctrl.state = TAG_OPEN;
                ctrl.tag_state = TAG_STATE_OPEN;

                *p = '\0';   //use original buffer
                cb(CM_GET_TEXT, ctrl.value, usr_data);
            }
        }
        break;

        case TAG_OPEN:
        {
            if (ch == '/')
            {
                ctrl.state = TAG_CLOSE;
                ctrl.tag_state = TAG_STATE_CLOSE;
            }
            else
            {
                if (cm_isalpha(ch))
                {
                    ctrl.state = TAG_NAME;
                    ctrl.value = p;
                }
            }
        }
        break;

        case TAG_NAME:
        {
            if (ch == '>')
            {
                *p = '\0';

                cb((ctrl.tag_state == TAG_STATE_OPEN ? CM_GET_OPEN_TAG : CM_GET_CLOSE_TAG),
                   ctrl.value, usr_data);
                
                ctrl.state = DATA_STATE;
            }
            else
            {
                if (cm_isblank(ch))
                {
                    *p = '\0';

                    cb((ctrl.tag_state == TAG_STATE_OPEN ? CM_GET_OPEN_TAG : CM_GET_CLOSE_TAG),
                   ctrl.value, usr_data);
                       
                    ctrl.state = P_NAME;
                    ctrl.value = p + 1;
                }
            }
        }
        break;

        case TAG_CLOSE:
        {
            if (cm_isalpha(ch))
            {
                ctrl.state = TAG_NAME;
                ctrl.value = p;
            }
        }
        break;

        case P_NAME:
        {
            if (ch == '=')
            {
                *p = '\0';
                cb(CM_GET_P_NAME, ctrl.value, usr_data);

                ctrl.state = P_VALUE_OPEN;
            }
        }
        break;

        case P_VALUE_OPEN:
        {
            if (ch == '"')
            {
                ctrl.value = p + 1;
                ctrl.state = P_VALUE;
            }
        }
        break;

        case P_VALUE:
        {
            if (ch == '"')
            {
                *p = 0;
                cb(CM_GET_P_VALUE, ctrl.value, usr_data);

                ctrl.state = P_CLOSE;
            }
        }
        break;

        case P_CLOSE:
        {
            if (ch == '>')
            {
                ctrl.state = DATA_STATE;
            }
        }
        break;

        default:
        break;
        }

        p++;
    }

    cb(CM_PARSE_DONE, 0, usr_data);
    
    return -1;
}



#include "nbk_cookie_manager.h"
#include "cm_utility.h"
#include "cm_debug.h"
#include "cm_time.h"
#include "cm_string.h"
#include "cm_io.h"
#include "cm_lib.h"


#define MAX_COOKIE_LINE	5000
#define MAX_NAME		128

#define COOKIE_STR_EQUAL(a, b) (cm_strcmp((a), (b)) == 0)


static void nbk_cookie_free(nbk_cookie *cookie);


static S32 checkday(const S8 *check, CM_SIZE len)
{
	//initial const
	static const S8 nbk_wkday[][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
	static const S8 weekday[][12] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

	S32 i;
	CM_BOOL found = 0;

	for (i = 0; i < 7; i++)
	{
		if ((len > 3 && COOKIE_STR_EQUAL(check, weekday[i])) || (len <=3 && COOKIE_STR_EQUAL(check, nbk_wkday[i])))
		{
			found = 1;
			break;
		}
	}

	return found?i:-1;
}

static S32 checkmonth(const S8 *check)
{
	static const S8 nbk_month[][4]= { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	S32 i;
	CM_BOOL found = 0;

	for (i = 0; i < 12; i++)
	{
		if (COOKIE_STR_EQUAL(check, nbk_month[i]))
		{
			found = 1;
			break;
		}
	}

	return found?i:-1;
}

struct tzinfo
{
	S8 name[5];
	S32 offset; /* +/- in minutes */
};

/* Here's a bunch of frequently used time zone names. These were supported
   by the old getdate parser. */
#define tDAYZONE -60       /* offset for daylight savings time */
static const struct tzinfo tz[]= {
	{"GMT", 0},              /* Greenwich Mean */
	{"UTC", 0},              /* Universal (Coordinated) */
	{"WET", 0},              /* Western European */
	{"BST", 0 tDAYZONE},     /* British Summer */
	{"WAT", 60},             /* West Africa */
	{"AST", 240},            /* Atlantic Standard */
	{"ADT", 240 tDAYZONE},   /* Atlantic Daylight */
	{"EST", 300},            /* Eastern Standard */
	{"EDT", 300 tDAYZONE},   /* Eastern Daylight */
	{"CST", 360},            /* Central Standard */
	{"CDT", 360 tDAYZONE},   /* Central Daylight */
	{"MST", 420},            /* Mountain Standard */
	{"MDT", 420 tDAYZONE},   /* Mountain Daylight */
	{"PST", 480},            /* Pacific Standard */
	{"PDT", 480 tDAYZONE},   /* Pacific Daylight */
	{"YST", 540},            /* Yukon Standard */
	{"YDT", 540 tDAYZONE},   /* Yukon Daylight */
	{"HST", 600},            /* Hawaii Standard */
	{"HDT", 600 tDAYZONE},   /* Hawaii Daylight */
	{"CAT", 600},            /* Central Alaska */
	{"AHST", 600},           /* Alaska-Hawaii Standard */
	{"NT",  660},            /* Nome */
	{"IDLW", 720},           /* International Date Line West */
	{"CET", -60},            /* Central European */
	{"MET", -60},            /* Middle European */
	{"MEWT", -60},           /* Middle European Winter */
	{"MEST", -60 tDAYZONE},  /* Middle European Summer */
	{"CEST", -60 tDAYZONE},  /* Central European Summer */
	{"MESZ", -60 tDAYZONE},  /* Middle European Summer */
	{"FWT", -60},            /* French Winter */
	{"FST", -60 tDAYZONE},   /* French Summer */
	{"EET", -120},           /* Eastern Europe, USSR Zone 1 */
	{"WAST", -420},          /* West Australian Standard */
	{"WADT", -420 tDAYZONE}, /* West Australian Daylight */
	{"CCT", -480},           /* China Coast, USSR Zone 7 */
	{"JST", -540},           /* Japan Standard, USSR Zone 8 */
	{"EAST", -600},          /* Eastern Australian Standard */
	{"EADT", -600 tDAYZONE}, /* Eastern Australian Daylight */
	{"GST", -600},           /* Guam Standard, USSR Zone 9 */
	{"NZT", -720},           /* New Zealand */
	{"NZST", -720},          /* New Zealand Standard */
	{"NZDT", -720 tDAYZONE}, /* New Zealand Daylight */
	{"IDLE", -720},          /* International Date Line East */
	/* Next up: Military timezone names. RFC822 allowed these, but (as noted in
	RFC 1123) had their signs wrong. Here we use the correct signs to match
	actual military usage.
	*/
	{"A",  +1 * 60},         /* Alpha */
	{"B",  +2 * 60},         /* Bravo */
	{"C",  +3 * 60},         /* Charlie */
	{"D",  +4 * 60},         /* Delta */
	{"E",  +5 * 60},         /* Echo */
	{"F",  +6 * 60},         /* Foxtrot */
	{"G",  +7 * 60},         /* Golf */
	{"H",  +8 * 60},         /* Hotel */
	{"I",  +9 * 60},         /* India */
	/* "J", Juliet is not used as a timezone, to indicate the observer's local time */
	{"K", +10 * 60},         /* Kilo */
	{"L", +11 * 60},         /* Lima */
	{"M", +12 * 60},         /* Mike */
	{"N",  -1 * 60},         /* November */
	{"O",  -2 * 60},         /* Oscar */
	{"P",  -3 * 60},         /* Papa */
	{"Q",  -4 * 60},         /* Quebec */
	{"R",  -5 * 60},         /* Romeo */
	{"S",  -6 * 60},         /* Sierra */
	{"T",  -7 * 60},         /* Tango */
	{"U",  -8 * 60},         /* Uniform */
	{"V",  -9 * 60},         /* Victor */
	{"W", -10 * 60},         /* Whiskey */
	{"X", -11 * 60},         /* X-ray */
	{"Y", -12 * 60},         /* Yankee */
	{"Z", 0},                /* Zulu, zero meridian, a.k.a. UTC */
};


static S32 checktz(const S8 *check)
{
	U32 i = 0;
	CM_BOOL found = 0;

	U32 size = sizeof(tz)/sizeof(tz[0]);
	for (i = 0; i < size; i++)
	{
		if (COOKIE_STR_EQUAL(check, tz[i].name))
		{
			found = 1;
			break;
		}
	}

	return found?tz[i].offset*60:-1;

}

static void nbk_date_skip(const S8 **date)
{
	while (**date && !cm_isalnum(**date))
	{
		(*date)++;
    }
}

enum nbk_date_assume
{
	DATE_MDAY,
	DATE_YEAR,
	DATE_TIME
};

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   -1
#define PARSEDATE_LATER  1
#define PARSEDATE_SOONER 2

static S32 bd_nbk_date_read_next_alpha(const S8 *dateptr, S8 *value)
{
	S32 count = 0;
	S32 item = 1;

	if (NULL == dateptr || NULL == value)
		return 0;

	while (*dateptr)
	{
		if (!cm_isalpha(*dateptr))
		{
			value[count] = 0;
			break;
		}
		else
		{
			value[count++] = dateptr[0];
			dateptr++;
		}
	}

	return item;
}

static S32 bd_nbk_date_read_time(const S8 *dateptr, S32 *hour, S32 *min, S32 *sec)
{
	S32 count = 0;
	S32 item = 0;
	S8 buff[8];

	buff[0] = 0;

	if (NULL == dateptr || NULL == hour || NULL == min || NULL == sec)
		return 0;

	*hour = *min = *sec = -1;
	while (*dateptr)
	{
		if (!cm_isdigit(*dateptr))
		{
			S32 n = 0;
			buff[count] = 0;
			count = 0;

			n = cm_atoi(buff);

			if (*hour == -1)
				*hour = n;
			else if (*min == -1)
				*min = n;
			else if (*sec == -1)
				*sec = n;

			item++;

			buff[0] = 0;

			if (*dateptr != ':')
				break;
		}
		else
		{
			buff[count++] = *dateptr;
		}

		dateptr++;
	}

	if (buff[0])
	{
		S32 n = 0;
		buff[count] = 0;

		n = cm_atoi(buff);

		if (*hour == -1)
			*hour = n;
		else if (*min == -1)
			*min = n;
		else if (*sec == -1)
			*sec = n;

		item++;
	}

	return item;
}

static S32 nbk_parsedate(const S8 *date, CM_TIME *output)
{
	CM_TIME t = 0;
	S32 wdaynum = -1;
	S32 monnum = -1;
	S32 mdaynum = -1;
	S32 yearnum = -1;
	S32 hournum = -1;
	S32 minnum = -1;
	S32 secnum = -1;
	S32 tzoff = -1;

	CM_DATE_TIME tm;
	enum nbk_date_assume dignext = DATE_MDAY;
	const S8 *indate = date;
	S32 part = 0; // max 6

	while (*date && part < 6)
	{
		CM_BOOL found = 0;

		nbk_date_skip(&date);

		if (cm_isalpha(*date))
		{
			S8 buff[32] = "";
			CM_SIZE len = 0;
			bd_nbk_date_read_next_alpha(date, buff);
			len = cm_strlen(buff);

			if (wdaynum == -1)
			{
				wdaynum = checkday(buff, len);
				if (wdaynum != -1)
					found = 1;
			}

			if (!found && (monnum == -1))
			{
				monnum = checkmonth(buff);
				if (monnum != -1)
					found = 1;
			}

			if (!found && (tzoff == -1))
			{
				tzoff = checktz(buff);
				if (tzoff != -1)
					found = 1;
			}

			if (!found)
				return PARSEDATE_FAIL;

			date += len;

		}
		else if (cm_isdigit(*date))
		{
			S32 val = 0;
			S8 *end;

			if ((secnum == -1) && (3 == bd_nbk_date_read_time(date, &hournum, &minnum, &secnum)))
				date += 8;
			else
			{
				val = (S32)cm_strtoul(date, &end, 10);

				if ((tzoff == -1) && ((end - date) == 4) && (val <= 1400) &&
					(indate < date) && ((date[-1] == '+' || date[-1] == '-')))
				{
					found = 1;
					tzoff = (val/100 * 60 + val % 100) * 60;
					tzoff = date[-1]=='+'?-tzoff:tzoff;
				}

				if (((end - date) == 8) && (yearnum == -1) && (monnum == -1) && (mdaynum == -1))
				{//YYYYMMDD
					found = 1;
					yearnum = val / 10000;
					monnum = (val % 10000)/100 - 1; //month is 0-11;
					mdaynum = val % 100;
				}

				if (!found && (dignext == DATE_MDAY) && (mdaynum == -1))
				{
					if ((val > 0) && (val < 32))
					{
						mdaynum = val;
						found = 1;
					}

					dignext = DATE_YEAR;
				}

				if (!found && (dignext == DATE_YEAR) && (yearnum == -1))
				{
					yearnum = val;
					found = 1;
					if (yearnum < 1900)
					{
						if (yearnum > 70)
							yearnum += 1900;
						else
							yearnum += 2000;
					}

					if (mdaynum == -1)
						dignext = DATE_MDAY;
				}

				if (!found)
					return PARSEDATE_FAIL;

				date = end;
			}
		}

		part++;
	}

	if (secnum == -1)
		secnum = minnum = hournum = 0;

	if ((-1 == mdaynum) || (-1 == monnum) || (-1 == yearnum))
		return PARSEDATE_FAIL;

	if (yearnum < 1970)
	{
		*output = 0;
		return PARSEDATE_SOONER;
	}

	tm.tm_sec = secnum;
	tm.tm_min = minnum;
	tm.tm_hour = hournum;
	tm.tm_mday = mdaynum;
	tm.tm_mon = monnum;
	tm.tm_year = yearnum - 1900;

	t = cm_mktime(&tm);

	if (-1 != (S32)t)
	{
		SLong delta = (SLong)(tzoff != -1?tzoff:0);

		if ((delta > 0) && (t+delta < t))
			return -1;

		t += delta;
	}

	*output = t;

	return PARSEDATE_OK;
}

static CM_TIME nbk_getdate(const S8 *p, const CM_TIME *now)
{
	CM_TIME parsed = -1;
	S32 rc = nbk_parsedate(p, &parsed);

	(void)now;
	switch(rc) 
	{
		case PARSEDATE_OK:
		case PARSEDATE_LATER:
		case PARSEDATE_SOONER:
			return parsed;
	}
	/* everything else is fail */
	return -1;
}



/////////////////////////////////////////////////////////////////////////////

static CM_BOOL nbk_tail_match(const S8 *little, const S8 *bigone)
{
	S32 littlelen = cm_strlen(little);
	S32 biglen = cm_strlen(bigone);
	
	if (littlelen > biglen)
		return 0;

	return (COOKIE_STR_EQUAL(little, bigone + biglen - littlelen));
}

static S32 nbk_cookie_line_read_next_attr(const S8 *lineptr, S8 *attr)
{
	S32 count = 0;
	S32 item = 1;

	if (NULL == lineptr || NULL == attr)
		return 0;

	while (*lineptr)
	{
		if (';' == lineptr[0])
		{
			attr[count] = 0;
			break;
		}
		else
		{
			attr[count++] = lineptr[0];
			lineptr++;
		}
	}

	return item;
}

static S32 nbk_cookie_line_read_next_key_value(const S8 *lineptr, S8 *key, S8 *value)
{
	S8 *p = NULL;
	S32 count = 0;
	S32 item = 1;
	if (NULL == lineptr || NULL == key || NULL == value)
		return 0;

	p = key;

	while (*lineptr)
	{
		if (';' == *lineptr)
		{
			p[count] = 0;//end string
			break;
		}
		else if ('=' == *lineptr && item == 1)
		{
			p[count] = 0;//end string
			p = value;
			count = 0;
			item++;
		}
		else
			p[count++] = *lineptr;
		lineptr++;
	}


	p[count] = 0;
	return item;
}

static void print_cookie(nbk_cookie *co)
{
	nbk_cookie *_co = co;
	//while (_co)
	{
		CM_TRACE_5("domain: %s", _co->domain);
		CM_TRACE_5("expires: %ld", _co->expires);
		CM_TRACE_5("expirestr: %s", _co->expirestr);
		CM_TRACE_5("httponly: %d", _co->httponly);
		CM_TRACE_5("maxage: %s", _co->maxage);
		CM_TRACE_5("name: %s", _co->name);
		CM_TRACE_5("path: %s", _co->path);
		CM_TRACE_5("secure: %d", _co->secure);
		CM_TRACE_5("tailmatch: %d", _co->tailmatch);
		CM_TRACE_5("value: %d", _co->value);
		CM_TRACE_5("version: %d", _co->version);
		//_co = _co->next;
	}
}

void nbk_cookie_free(nbk_cookie *cookie)
{
	if (cookie == NULL)
		return;

	CM_FREE(cookie->domain);
	CM_FREE(cookie->expirestr);
	CM_FREE(cookie->maxage);
	CM_FREE(cookie->name);
	CM_FREE(cookie->path);
	CM_FREE(cookie->value);
	CM_FREE(cookie->version);

	CM_FREE(cookie);
}

nbk_cookie_info* nbk_cookie_init(const S8 *file, CM_BOOL newsession)
{
	nbk_cookie_info *cookie = NULL;

    CM_ASSERT(file != 0);

    cookie = (nbk_cookie_info *)CM_MALLOC(sizeof(nbk_cookie_info));
    if (cookie)
    {
        CM_HANDLE fp;

        cm_memset(cookie, 0, sizeof(nbk_cookie_info));
        cookie->filename = cm_strdup(file);

        fp = cm_fopen(file, "rb");
        //CM_ASSERT(fp != 0);

        if (fp)
    	{
    		S8 *lineptr;
    		CM_BOOL headerline;
    		S8 *line = (S8 *)CM_MALLOC(sizeof(S8) * MAX_COOKIE_LINE);
    		if (line)
    		{
    			while (cm_fgets(line, MAX_COOKIE_LINE, fp))
    			{
    				CM_TRACE_5("line: %s", line);

    				if (COOKIE_STR_EQUAL("Set-Cookie:", line))
    				{
    					lineptr = &line[11];
    					headerline = 1;
    				}
    				else
    				{
    					lineptr = line;
    					headerline = 0;
    				}

    				while (*lineptr && cm_isblank(*lineptr))
    					lineptr++;

    				nbk_cookie_add(cookie, headerline, lineptr, NULL, NULL);
    			}

    			//print cookis
    			CM_TRACE_5("cookie num: %d", cookie->numcookies);
    		}

    		CM_FREE(line);
    		cm_fclose(fp);
    	}

    	cookie->newsession = newsession;
    	cookie->running = 1;
    }

	return cookie;
}

void nbk_cookie_deinit(nbk_cookie_info *c)
{
	nbk_cookie *co, *next;

	if (c)
	{
		if (c->filename)
			CM_FREE(c->filename);
		co = c->cookies;

		while (co)
		{
			next = co->next;
			nbk_cookie_free(co);
			co = next;
		}

		CM_FREE(c);
	}
}

nbk_cookie* nbk_cookie_add(nbk_cookie_info* list, CM_BOOL httpheader, S8 *lineptr, const S8 *domain, const S8 *path)
{
	nbk_cookie *cookie = NULL, *clist = NULL, *lastc = NULL;

	CM_TIME now;

	CM_BOOL replace_old = 0;
	CM_BOOL badcookie = 0;

	//alloc instance
	cookie = (nbk_cookie *)CM_MALLOC(sizeof(nbk_cookie));
	if (!cookie)
	{
		return NULL;
    }
    else
    {
        cm_memset(cookie, 0, sizeof(nbk_cookie));
    }

	if (httpheader)
	{
		const S8 *ptr;
		const S8 *sep;
		const S8 *semiptr;
		S8 name[MAX_NAME];
		

		S8 *what = (S8 *)CM_MALLOC(sizeof(S8) * MAX_COOKIE_LINE);
		if (!what)
		{
			CM_FREE(cookie);
			return cookie;
		}

		semiptr = cm_strchr(lineptr, ';');

		while (*lineptr && cm_isblank(*lineptr))
			lineptr++;

		ptr = lineptr;

		do
		{
			sep = cm_strchr(ptr, '=');
			if (sep && (!semiptr || (semiptr > sep)))
			{
				name[0] = what[0] = 0;
				if (1 <= nbk_cookie_line_read_next_key_value(ptr, name, what))
				{
					const S8 *whatptr;
					S32 len = cm_strlen(what);

					//string trim
					while (len && cm_isblank(what[len-1]))
					{
						what[len-1] = 0;
						len--;
					}

					whatptr = what;
					while (*whatptr && cm_isblank(*whatptr))
						whatptr++;

					if (COOKIE_STR_EQUAL("path", name))
					{
						cookie->path = cm_strdup(whatptr);
						if (!cookie->path)
						{
							CM_TRACE_5("cookie->path is null");
							badcookie = 1;
							break;
						}
					}
					else if (COOKIE_STR_EQUAL("domain", name))
					{
						const S8 *domptr = whatptr;
						S32 dotcount = 1;

						if ('.' == *whatptr)
							domptr++;//dont count the initial dot

						do
						{
							domptr = cm_strchr(domptr, '.');
							if (domptr)
							{
								domptr++;
								dotcount++;
							}
						}while(domptr);

						if (dotcount < 2)
						{//too few dot, bad domain, skip this cookie
							CM_TRACE_5("to few dot bad domain");
							badcookie = 1;
						}
						else
						{
							if ('.' == whatptr[0])
								whatptr++;

							if (!domain || nbk_tail_match(whatptr, domain))
							{
								const S8 *tailptr = whatptr;
								if (tailptr[0] == '.')
									tailptr++;
								cookie->domain = cm_strdup(tailptr);
								if (!cookie->domain)
								{
									CM_TRACE_5("cookie->domain is null");
									badcookie = 1;
									break;
								}

								cookie->tailmatch = 1;
							}
							else
							{
								CM_TRACE_5("nbk_tail_match(%s, %s) == %d", whatptr, domain, nbk_tail_match(whatptr, domain));
								CM_TRACE_5("domain && !nbk_tail_match(whatptr, domain)");
								badcookie = 1;
							}
						}
					}
					else if (COOKIE_STR_EQUAL("version", name))
					{
						cookie->version = cm_strdup(whatptr);
						if (!cookie->version)
						{
							CM_TRACE_5("cookie->version is null");
							badcookie = 1;
							break;
						}

					}
					else if (COOKIE_STR_EQUAL("max-age", name))
					{
					}
					else if (COOKIE_STR_EQUAL("expires", name))
					{
						cookie->expirestr = cm_strdup(whatptr);
						if (!cookie->expirestr)
						{
							CM_TRACE_5("cookie->expirestr is null");
							badcookie = 1;
							break;
						}

						cookie->expires = nbk_getdate(what, &now);
					}
					else if (!cookie->name)
					{//custom cookie
						cookie->name = cm_strdup(name);
						cookie->value = cm_strdup(whatptr);
						if (!cookie->name || !cookie->value)
						{
							CM_TRACE_5("!cookie->name || !cookie->value");
							badcookie = 1;
							break;
						}
					}
				}
			}
			else
			{
				if (nbk_cookie_line_read_next_attr(ptr, what))
				{
					if (COOKIE_STR_EQUAL("secure", what))
					{
						cookie->secure = 1;
					}
					else if (COOKIE_STR_EQUAL("httponly", what))
					{
						cookie->httponly = 1;
					}
				}
			}


			if (!semiptr || !*semiptr)
			{
				semiptr = NULL;
				continue;
			}

			ptr = semiptr+1;
			while (ptr && *ptr && cm_isblank(*ptr))
				ptr++;
			semiptr = cm_strchr(ptr, ';');//find next

			if (!semiptr && *ptr)
				semiptr = cm_strchr(ptr, '\0');//last kv

		}while (semiptr);

		if (!badcookie && !cookie->domain)
		{
			if (domain)
			{
				cookie->domain = cm_strdup(domain);
				if (!cookie->domain)
				{
					CM_TRACE_5("!cookie->domain 2");
					badcookie = 1;
				}
			}
		}

		if (!badcookie && !cookie->path && path)
		{
			S8 *queryp = cm_strchr(path, '?');
			S8 *endslash;

			if (!queryp)
				endslash = cm_strrchr(path, '/');
			else
				endslash = cm_memrchr(path, '/', (CM_SIZE)(queryp - path));

			if (endslash)
			{
				CM_SIZE pathlen = (CM_SIZE)(endslash - path + 1);
				cookie->path = (S8 *)CM_MALLOC(pathlen + 1);
				if (cookie->path)
				{
					cm_memcpy(cookie->path, path, pathlen);
					cookie->path[pathlen] = 0;
				}
				else
				{
					CM_TRACE_5("!cookie->path 2");
					badcookie = 1;
				}
			}
		}

		CM_FREE(what);

		if (badcookie || !cookie->name)
		{
			CM_TRACE_5("bad cookie or cookie->name[%s] is null", cookie->name);
			nbk_cookie_free(cookie);
			return NULL;
		}

	}
	else
	{//not http header style line
		S8 *ptr;
		S8 *firstptr;
		S8 *tok_buf = NULL;
		S32 fields;

		if (cm_strncmp(lineptr, "#HttpOnly_", 10) == 0)
		{
			lineptr += 10;
			cookie->httponly = 1;
		}

		if (lineptr[0] == '#')
		{
			CM_FREE(cookie);
			return NULL;
		}

		ptr = cm_strchr(lineptr, '\r');
		if (ptr)
			*ptr = '\0';
		ptr = cm_strchr(lineptr, '\n');
		if (ptr)
			ptr = '\0';

		firstptr = cm_strtok_r(lineptr, "\t", &tok_buf);

		if (!firstptr || cm_strchr(firstptr, ':'))
		{
			CM_FREE(cookie);
			return NULL;
		}

		for (ptr = firstptr, fields = 0; ptr && !badcookie; ptr = cm_strtok_r(NULL, "\t", &tok_buf), fields++)
		{
			switch(fields)
			{
			case 0:
				{
					if (ptr[0] == '.')
						ptr++;
					cookie->domain = cm_strdup(ptr);
					if (!cookie->domain)
						badcookie = 1;
				}
				break;
			case 1:
				{
					cookie->tailmatch = (CM_BOOL)COOKIE_STR_EQUAL(ptr, "TRUE");
				}
				break;
			case 2:
				{
					if (cm_strcmp("TRUE", ptr) && cm_strcmp("FALSE", ptr))
					{
						cookie->path = cm_strdup(ptr);
						if (!cookie->path)
							badcookie = 1;
						break;
					}

					cookie->path = cm_strdup("/");
					if (!cookie->path)
						badcookie = 1;
					fields++;
				}
				//break;
			case 3:
				{
					cookie->secure = (CM_BOOL)COOKIE_STR_EQUAL(ptr, "TRUE");
				}
				break;
			case 4:
				{
					cookie->expires = cm_strtoul(ptr, NULL, 10);
				}
				break;
			case 5:
				{
					cookie->name = cm_strdup(ptr);
					if (!cookie->name)
						badcookie = 1;
				}
				break;
			case 6:
				{
					cookie->value = cm_strdup(ptr);
					if (!cookie->value)
						badcookie = 1;
				}
				break;
			}
		}

		if (6 == fields)
		{
			cookie->value = cm_strdup("");
			if (!cookie->value)
				badcookie = 1;
			else
				fields++;
		}

		if (!badcookie && (7 != fields))
			badcookie = 1;

		if (badcookie)
		{
			nbk_cookie_free(cookie);
			return NULL;
		}
	}

	if (!list->running && list->newsession && !cookie->expires)
	{
		nbk_cookie_free(cookie);
		return NULL;
	}

	cookie->livecookie = list->running;


	clist = list->cookies;
	replace_old = 0;

	while (clist)
	{
		if (COOKIE_STR_EQUAL(clist->name, cookie->name))
		{
			if (clist->domain && cookie->domain)
			{
				if (COOKIE_STR_EQUAL(clist->domain, cookie->domain))
					replace_old = 1;
			}
			else if (!clist->domain && !cookie->domain)
			{
				replace_old = 1;
			}

			if (replace_old)
			{
				if (clist->path && cookie->path)
				{
					if (COOKIE_STR_EQUAL(clist->path, cookie->path))
					{
						replace_old = 1;
					}
					else
					{
						replace_old = 0;
					}
				}
				else if (!clist->path && !cookie->path)
				{
					replace_old = 1;
				}
				else
				{
					replace_old = 0;
				}
			}

			if (replace_old && !cookie->livecookie && clist->livecookie)
			{
				nbk_cookie_free(cookie);
				return NULL;
			}

			if (replace_old)
			{
				cookie->next = clist->next;

				CM_FREE(clist->name);
				CM_FREE(clist->value);
				CM_FREE(clist->domain);
				CM_FREE(clist->path);
				CM_FREE(clist->expirestr);
				CM_FREE(clist->version);
				CM_FREE(clist->maxage);

				*clist = *cookie;
				CM_FREE(cookie);
				cookie = clist;

				do
				{
					lastc = clist;
					clist = clist->next;
				}while(clist);
				break;
			}
		}

		lastc = clist;
		clist = clist->next;
	}

	if (!replace_old)
	{//not replace, append to last
		if (lastc)
			lastc->next = cookie;
		else
			list->cookies = cookie;
	}

	list->numcookies++;
	return cookie;
}

nbk_cookie* nbk_cookie_get_list(nbk_cookie_info *c, const S8 *host, const S8 *path, CM_BOOL secure)
{
	nbk_cookie *newco = NULL;
	nbk_cookie *co = NULL;
	nbk_cookie *mainco = NULL;

	CM_TIME now = cm_time(NULL);

	if (!c) return NULL;

	if (c && (co = c->cookies))
	{
		while (co)
		{
			//print_cookie(co);
			if ((co->expires <= 0 || (co->expires > (SLong)now)) && (co->secure?secure : 1))
			{
				if (!co->domain || (co->tailmatch && nbk_tail_match(co->domain, host)) || (!co->tailmatch && COOKIE_STR_EQUAL(host, co->domain)))
				{
					if (!co->path || !cm_strncmp(co->path, path, cm_strlen(co->path)))
					{
						newco = CM_MALLOC(sizeof(*newco));
						if (newco)
						{
							cm_memcpy(newco, co, sizeof(nbk_cookie));
							newco->next = mainco;
							mainco = newco;
						}
						else
						{
							while (mainco)
							{
								co = mainco->next;
								CM_FREE(mainco);
								mainco = co;
							}

							return mainco;
						}
					}
				}

			}
			co = co->next;
		}
	}

	return mainco;
}

void nbk_cookie_free_list(nbk_cookie *co, CM_BOOL cookietoo)
{
	nbk_cookie *next;
	if (NULL == co) return;

	while (co)
	{
		next = co->next;
		if (cookietoo)
			nbk_cookie_free(co);
		else
			CM_FREE(co);

		co = next;
	}
}

#define FORMAT_OFF_T "ld"
static void get_netscape_format(S8 *format, const nbk_cookie *cookie)
{
	cm_sprintf(format, 
				"%s"     /* httponly preamble */
				"%s%s\t" /* domain */
				"%s\t"   /* tailmatch */
				"%s\t"   /* path */
				"%s\t"   /* secure */
				"%" FORMAT_OFF_T "\t"   /* expires */
				"%s\t"   /* name */
				"%s"     /* value */
				"\n",
				cookie->httponly ? "#HttpOnly_" : "",
				/* Make sure all domains are prefixed with a dot if they allow
				   tailmatching. This is Mozilla-style. */
				(cookie->tailmatch && cookie->domain && cookie->domain[0] != '.')? ".":"",
				cookie->domain ? cookie->domain : "unknown",
				cookie->tailmatch ? "TRUE" : "FALSE",
				cookie->path ? cookie->path : "/",
				cookie->secure ? "TRUE" : "FALSE",
				cookie->expires,
				cookie->name,
				cookie->value ? cookie->value : "");
}

CM_BOOL nbk_cookie_output(nbk_cookie_info *list, const S8 *fname)
{
	CM_HANDLE fp;

	if (NULL == list || (0 == list->numcookies))
	{
		return 1;
	}

    fp = cm_fopen(fname, "a+");
    CM_ASSERT(fp != 0);

    if (fp)
	{
        nbk_cookie *co = NULL;
		S8 *format_str = (S8 *)CM_MALLOC(sizeof(S8) * MAX_COOKIE_LINE);

		co = list->cookies;
		while (co)
		{
			get_netscape_format(format_str, co);
			cm_fwrite(format_str, cm_strlen(format_str), 1, fp);

			cm_memset(format_str, 0, MAX_COOKIE_LINE);
			co = co->next;
		}

		CM_FREE(format_str);
		cm_fclose(fp);
		return 1;
	}

	return 0;
}


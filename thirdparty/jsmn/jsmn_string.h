/*
// Source: https://gist.github.com/schneidersoft/ca11078da9e0607501b85a0cdef317bc

Jsmn does not decode json strings, so it's up to you to do that.
Incidentally the json string is always going to be longer,
or the same size as the decoded string because of the way escape sequences work.
This means you can decode the json string in place in the original input buffer.
So. After you have parsed the input string into jsm tokens you can call
  const char *foo = jsmn_string(buffer, token);
on any token. If the return is not NULL, it is a valid utf8 string.
 
You can even call jsmn_string on a token twice without issue.
jsmn_string overwrites the opening " character in the json string to indicate
if it has already been decoded and will always return the same utf8 string or NULL.
The closing " character is possibly overwritten to '\0' terminate the string.
interesting tidbit,
If the json string contains the unicode code point 0 like this: "\u0000" 
the returned utf8 string will have an embedded '\0' (terminating null) character
One solution is to use an overlong utf8 sequence to encode the codepoint
#define JSMN_STRING_EMBEDDED_NULL 1
Or to count codepoint 0 as an invalid codepoint
#define JSMN_STRING_EMBEDDED_NULL 2
*/

#include "jsmn.h"

#ifndef JSMN_STRING_EMBEDDED_NULL
#define JSMN_STRING_EMBEDDED_NULL 0
#endif

static int hex2int(char ch){
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

const char *jsmn_string(char *buf, const jsmntok_t *tk){
	if (tk->type != JSMN_STRING)
		return NULL;

	//assert(tk->start>0);
	//assert(tk->end>0);

	if (buf[tk->start-1] == '\0') //already decoded...
		return &buf[tk->start];

	if (buf[tk->start-1] == '!') //already decoded...
		return NULL;

	//decode the string...
	char *dst = &buf[tk->start];
	const char *src = dst;
	const char *end = &buf[tk->end];
	for (;src!=end;src++) {
		if (*src & 0x80) {	//get utf8
GET_UTF8:;

			if ((*src & 0xE0) == 0xC0) {	//two bytes
				*(dst++) = *src;

				src++;
				if ((*src & 0xC0) != 0x80)
					goto UTF8_ERROR;
				*(dst++) = *src;
			} else if ((*src & 0xF0) == 0xE0) {	//three
				*(dst++) = *src;

				src++;
				if ((*src & 0xC0) != 0x80)
					goto UTF8_ERROR;
				*(dst++) = *src;

				src++;
				if ((*src & 0xC0) != 0x80)
					goto UTF8_ERROR;
				*(dst++) = *src;
			} else if ((*src & 0xF8) == 0xF0) {	//four
				*(dst++) = *src;

				src++;
				if ((*src & 0xC0) != 0x80)
					goto UTF8_ERROR;
				*(dst++) = *src;

				src++;
				if ((*src & 0xC0) != 0x80)
					goto UTF8_ERROR;
				*(dst++) = *src;

				src++;
				if ((*src & 0xC0) != 0x80)
					goto UTF8_ERROR;
				*(dst++) = *src;
			} else {
				goto UTF8_ERROR;
			}
			continue;
		}

		if (*src == '"')	//there should be only one closing " (that we never scan @ tk->end)
				goto UTF8_ERROR;

		if (*src == '\\') {
			if (*(src+1) & 0x80)	//escape of utf8 sequence... (meaningless really)
				goto GET_UTF8;

			if (*(src+1) == 'u') {	//escape of unicode code point (decode code point to utf8)
				//get a unicode code point
				int uni = 0;
				int i;
				for (i = 0; i < 4; i++) {
					src++;
					if (src == end)
						goto UTF8_ERROR;

					int c = hex2int(*src);
					if (c < 0)
						goto UTF8_ERROR;

					uni <<= 4;
					uni |= c;
				}

				//reencode as utf8 bytes
				#if JSMN_STRING_EMBEDDED_NULL == 0
				#elif JSMN_STRING_EMBEDDED_NULL == 1
				if (uni == 0x00) {
				  *dst++=0xC0;
				  *dst++=0x80;
				}
				#elif JSMN_STRING_EMBEDDED_NULL == 2
				if (uni == 0x00)
				  goto UTF8_ERROR;
				#else
					#error JSMN_STRING_EMBEDDED_NULL must be 0,1, or 2
				#endif
				if (uni < 0x80)
					*dst++=uni;
				else if (uni < 0x800) {
					*dst++=0xC0 + uni/64;
					*dst++=0x80 + uni%64;
				} else if (uni < 0xE000) {
					goto UTF8_ERROR;
				} else if (uni < 0x10000) {
					*dst++=0xE0 + uni/4096;
					*dst++=0x80 + uni/64%64;
					*dst++=0x80 + uni%64;
				} else if (uni < 0x110000) {
					*dst++=0xF0 + uni/262144;
					*dst++=0x80 + uni/4096%64;
					*dst++=0x80 + uni/64%64;
					*dst++=0x80 + uni%64;
				} else goto UTF8_ERROR;
				continue;
			}

			switch (*(src+1)) {
			//decode special
			case 'b': *(dst++) = '\b'; break;
			case 'f': *(dst++) = '\f'; break;
			case 'n': *(dst++) = '\n'; break;
			case 'r': *(dst++) = '\r'; break;
			case 't': *(dst++) = '\t'; break;

			//decode next (single byte utf8)
			default: *(dst++) = *(src+1); break;
			}

			src++;
			continue;
		}

		//get ordinary single byte utf8
		*(dst++) = *src;
	}
	*(dst++)='\0';

	buf[tk->start-1] = '\0';
	buf[tk->end] = '\0';
	return &buf[tk->start];

UTF8_ERROR:;
	buf[tk->start-1] = '!';
	return NULL;
}
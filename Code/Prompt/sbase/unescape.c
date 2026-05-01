/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <string.h>

#include "util.h"

#define is_odigit(c) ('0' <= c && c <= '7')

size_t
unescape(char *s)
{
	static const char escapes[256] = {
		['"'] = '"',
		['\''] = '\'',
		['\\'] = '\\',
		['a'] = '\a',
		['b'] = '\b',
		['E'] = 033,
		['e'] = 033,
		['f'] = '\f',
		['n'] = '\n',
		['r'] = '\r',
		['t'] = '\t',
		['v'] = '\v'};
	size_t m, q;
	char *r, *w;

	for (r = w = s; *r;)
	{
		if (*r != '\\')
		{
			*w++ = *r++;
			continue;
		}
		r++;
		if (!*r)
		{
			printf("null escape sequence\n");
		}
		else if (escapes[(unsigned char)*r])
		{
			*w++ = escapes[(unsigned char)*r++];
		}
		else if (is_odigit(*r))
		{
			for (q = 0, m = 3; m && is_odigit(*r); m--, r++)
				q = q * 8 + (size_t)(*r - '0');
			*w++ = (char)MIN(q, 255);
		}
		else if (*r == 'x' && isxdigit(r[1]))
		{
			r++;
			for (q = 0, m = 2; m && isxdigit(*r); m--, r++)
				if (isdigit(*r))
					q = q * 16 + (size_t)(*r - '0');
				else
					q = q * 16 + (size_t)(tolower(*r) - 'a' + 10);
			*w++ = (char)q;
		}
		else
		{
			printf("invalid escape sequence '\\%c'\n", *r);
		}
	}
	*w = '\0';

	return (size_t)(w - s);
}

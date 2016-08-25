/*
    clib.c

    Minimal c-Library for OS/2 drivers.

    Copyright (c) 1995, 1996 Immisch, Becker & Partner, Hamburg & Lars Immisch

    Author: Lars Immisch <lars@ibp.de>
*/

#include "drvlib.h"
#include "clib.h"

#define isalnum(c)  ((((int)(c)>='0')&((int)(c)<='9'))|(((int)(c)>='A')&((int)(c)<='z')))
#define isspace(c)   (((int)(c)==' ')|((int)(c)=='\t'))


int strlen(const char far * s)
{
    int i;

    for (i=0;*s != '\0';i++,s++);

    return i;
}

char far * strcpy(char far* s1, const char far * s2)
{
    char far* s3 = s1;

    while(*s1++ = *s2++);

    return s3;
}

void far * memcpy(void far * d, const void far * s, int c)
{
    void far * r = d;
    int i;

    for (i=0;i<c;i++)   *((char far *)d)++ = *((char far *)s)++;

    return r;
}

void far * memset(void far * d, int v, int c)
{
    void far * r = d;
    int i;

    for (i=0;i<c;i++)   *((char far *)d)++ = v;

    return r;
}

int valueof(char c)
{
    if (c >= 'a') return c - 'a' + 10;
    else if (c >= 'A') return c - 'A' + 10;
    else return c - '0';
}

char symbolof(int v)
{
    if (v > 9) return 'a' + v -10;
    else return '0' + v;
}

char lower(char c)
{
    if (c <= 'Z' && c >= 'A')   return 'a' + c - 'A';
    else return c;
}

int stricmp(char far * a, char far * b)
{
    int index;
    int value;

    for (index = 0; a[index] != '\0' && b[index] != 0;index++)
    {
        if ((value = lower(a[index]) - lower(b[index])) != 0) return value;
    }

    return 0;
}

char far * strchr(const char far* s, int c)
{
    for (;*s != '\0' && *s != c;s++);

    return *s == '\0' ? 0 : (char far *)s;
}

unsigned atoir(const char far* s, int radix)
{
    unsigned base = 1;
    int i;
    unsigned value = 0;


    /* move to the end, without trailing spaces */
    for (i = strlen(s)-1;isspace(s[i]) && i >= 0;i--);

    for(;i >= 0;i--)
    {
        if (isalnum(s[i]))
        {
            if (valueof(s[i]) > radix)  return 0;
            value += valueof(s[i]) * base;
            base *= radix;
        }
        else return 0;
    }

    return value;
}

int getChar(SHANDLE hFile, char far* buffer, unsigned long far* pPosition, unsigned long far* pLength)
{
    ULONG rc;

	if (*pPosition < *pLength)	return buffer[(*pPosition)++];
	
	rc = DosRead(hFile, buffer, BUFFERSIZE, pLength);
	if (rc != 0 || *pLength == 0)	return -1;

	*pPosition = 0;
	return buffer[(*pPosition)++];
}

void skipToNextLine(SHANDLE hFile, char far* buffer, unsigned long far* pos, unsigned long far* length)
{
	int c;
	
	while(TRUE)
	{
		c = getChar(hFile, buffer, pos, length);
		if (c == -1 || c == '\n')	return;
	}
}

char far* nextToken(SHANDLE hFile, char far* buffer, unsigned long far* pPosition, unsigned long far* pLength, char far* token)
{
	ULONG pos = 0;
	BOOL seenBlack = FALSE;
	int c;
	
	while(1)
	{
		switch(c = getChar(hFile, buffer, pPosition, pLength))
		{
		case -1:
			if (seenBlack)	
            {
                token[pos] = '\0';
                return token;
            }
			token[0] = 0;
			return 0;
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '':
			if (seenBlack)
            {
                token[pos] = '\0';
                return token;
            }
			break;
		case '#':
		case ';':
			skipToNextLine(hFile, buffer, pPosition, pLength);
			break;
		default:
			seenBlack = TRUE;
            if (pos >= TOKENSIZE -1) token[pos] = '\0';
			else token[pos++] = c;
			break;
		}
	}
}

int opterror(const char far * e, char c)
{
    asciizout(e);
    charout(c);
    asciizout(" (ignored)\r\n");
    return EOF;
}

int option(const char far * args, const char far * optstr, char far * optarg, int optlen, int far * pindex)
{
    char far * op;
    int index = *pindex;

    /* search for the next '-' or '/' */
    for (;args[index] != '-' && args[index] != '/' && args[index] != '\0';index++);

    if (args[index] == '\0' || args[index+1] == '\0')    return EOF;

    /* search the option */
    op = strchr(optstr, args[++index]);

    if (op == 0)    
    { 
        *pindex = index+1;
        return 0; 
    }

    if (*(op+1) == ':')
    {
        /* get rid of whitespace between option and arg. No whitespace is allowed */
        for (++index;isspace(args[index]);index++);
        if (args[index] == '\0' || args[index] == '-' || args[index] == '/')    return opterror("need argument for option: ", *op);

        /* copy the argument into optarg buffer */
        for(optlen -=1;!isspace(args[index]) && args[index] != '\0';index++, optlen--)  
            if (optlen > 0) *optarg++ = args[index];

        if (optlen < 0)    return opterror("argument too long for option: ", *op);

        *optarg = '\0';
    }

    *pindex = index;
    return *op;
}

void charout(char c)
{
    DosPutMessage(1, 1, &c);
}

void asciizout(const char far * s)
{
    DosPutMessage(1, strlen(s), s);
}

void integerout(unsigned value, int radix)
{
    char buffer[33];
    unsigned base = 1;
    int i = 0;

    while ((value / base) >= radix) base *= radix;

    do
    {
        buffer[i++] = symbolof(value / base);
    
        value %= base;

        if (base != 1) base /= radix;
        else base = 0;
    } while (base > 0);
    
    buffer[i] = '\0';

    asciizout(buffer);
}


/*
    clib.h

    Copyright (c) 1995, 1996 Immisch, Becker & Partner, Hamburg & Lars Immisch

    Author: Lars Immisch <lars@ibp.de>
*/

#ifndef _CLIB_H_
#define _CLIB_H_

#define EOF (-1)
#define BUFFERSIZE 256
#define TOKENSIZE 256

unsigned atoir(const char far* s, int radix);

char far * strchr(const char far* s, int c);

int strlen(const char far * s);

char far * strcpy(char far* s1, const char far * s2);

void far * memcpy(void far * d, const void far * s, int c);

void far * memset(void far * d, int c, int size);

int stricmp(char far * a, char far * b);

int getChar(SHANDLE hFile, char far* buffer, unsigned long far* pPosition, unsigned long far* pLength);

void skipToNextLine(SHANDLE hFile, char far* buffer, unsigned long far* pos, unsigned long far* length);

char far* nextToken(SHANDLE hFile, char far* buffer, unsigned long far* pos, unsigned long far* length, char far* token);

/*  option is sort of a getopt variant, *pindex must be initialized to 0  */
int option(const char far * args, const char far * optstr, char far * optarg, int optlen, int far * pindex);

void charout(char c);

void asciizout(const char far * s);

void integerout(unsigned value, unsigned radix);

#endif

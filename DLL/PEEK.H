/*
	peek.h

	Copyright (c) 1995 Lars Immisch

	created: 18.3.1995
	Author: Lars Immisch <lars@ibp.de>
*/

#ifndef _PEEK_H_
#define _PEEK_H_

#include <os2def.h>

#ifdef __cpluslus
extern "C" {
#endif

UCHAR  APIENTRY     PeekReadByte(USHORT ioAddr);
USHORT APIENTRY     PeekReadWord(USHORT ioAddr);
VOID   APIENTRY     PeekWriteByte(USHORT ioAddr, UCHAR byte);
VOID   APIENTRY     PeekWriteWord(USHORT ioAddr, USHORT word);
ULONG  APIENTRY     PeekMapPhysicalAddress(ULONG physical, PPVOID pVirtual, ULONG size);
ULONG  APIENTRY     PeekUnmapPhysicalAddress(PVOID virtualAddress);
VOID   APIENTRY     PeekReboot();

#ifdef __cpluslus
}
#endif

#endif

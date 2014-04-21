/*
	peek.c
	
	API for Peek! driver
	
	Copyright (c) 1995 Immisch, Becker & Partner & Lars Immisch

	Author: Lars Immisch <lars@ibp.de>
*/

#include <stdio.h>

#define INCL_BASE
#include <os2.h>

#include "..\ioctls.h"
#include "peek.h"

HFILE   hDriver;

UCHAR  APIENTRY     PeekReadByte(USHORT ioAddr)
{
    ULONG rc;
    PEEKPARAMETER peek;
    ULONG size = sizeof(peek);

    peek.inb.ioAddr = ioAddr;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_INBYTE, &peek, sizeof(peek), &size, NULL, 0, NULL);
    if (rc != 0)    return rc & 0xff;

    return peek.inb.inByte;
}

USHORT APIENTRY     PeekReadWord(USHORT ioAddr)
{
    ULONG rc;
    PEEKPARAMETER peek;
    ULONG size = sizeof(peek);

    peek.inw.ioAddr = ioAddr;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_INWORD, &peek, sizeof(peek), &size, NULL, 0, NULL);
    if (rc != 0)    return rc & 0xff;

    return peek.inw.inWord;
}

VOID   APIENTRY     PeekWriteByte(USHORT ioAddr, UCHAR byte)
{
    ULONG rc;
    PEEKPARAMETER peek;
    ULONG size = sizeof(peek);

    peek.outb.outByte = byte;
    peek.outb.ioAddr = ioAddr;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_OUTBYTE, &peek, sizeof(peek), &size, NULL, 0, NULL);
}

VOID   APIENTRY     PeekWriteWord(USHORT ioAddr, USHORT word)
{
    ULONG rc;
    PEEKPARAMETER peek;
    ULONG size = sizeof(peek);

    peek.outw.outWord = word;
    peek.outw.ioAddr = ioAddr;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_OUTWORD, &peek, sizeof(peek), &size, NULL, 0, NULL);
}

ULONG  APIENTRY    PeekMapPhysicalAddress(ULONG physical, PPVOID pVirtual, ULONG mapSize)
{
    ULONG rc;
    PEEKPARAMETER peek;
    ULONG size = sizeof(peek);

    if (!pVirtual) return ERROR_INVALID_PARAMETER;

    peek.map.physical = physical;
    peek.map.size = mapSize;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_MAP, &peek, sizeof(peek), &size, NULL, 0, NULL);

    if (rc == 0)    *pVirtual = (PVOID)peek.map.userVirtual;

    return rc;
}

ULONG  APIENTRY    PeekUnmapPhysicalAddress(PVOID virtualAddress)
{
    ULONG rc;
    PEEKPARAMETER peek;
    ULONG size = sizeof(peek);

    peek.freemap.userVirtual = (ULONG)virtualAddress;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_FREE_MAP, &peek, sizeof(peek), &size, NULL, 0, NULL);

    return rc;
}

VOID   APIENTRY     PeekReboot()
{
    ULONG rc;

    rc = DosDevIOCtl(hDriver, IOCTL_CATEGORY, IOCTL_REBOOT, NULL, 0, NULL, NULL, 0, NULL);
}

unsigned long _System _DLL_InitTerm(unsigned  long handle, unsigned long flag)
{
	ULONG rc ;
    ULONG action;

    switch (flag)
    {
    case 0:
        rc = DosOpen("poke$", &hDriver, &action, 0, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS, OPEN_SHARE_DENYNONE, NULL);

        if (rc != 0) 
        {
            printf("peek! driver not found\n");
            return 0;
        }

        return 1;
    case 1:
        DosClose(hDriver);
        return 1;
    }
}


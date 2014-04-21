/*
	Copyright 1995 Immisch, Becker & Partner, Hamburg

	created: Fri May 19 21:25:19 GMT+0100 1995

	Author: Lars Immisch <limmisch@t42.ppp.de>
*/

#define INCL_BASE
#include <os2.h>

#include "stdio.h"

#include "peek/dll/peek.h"

int main(int argc, char* argv[])
{
    ULONG rc;
    PVOID phys;
    int i;
    char* pPhys;

    printf("in 0x%x: 0x%0x\n", 0x40, PeekReadByte(0x40));

    rc = PeekMapPhysicalAddress(0x000d0000, &phys, 4096);
    if (rc != 0)
    {
        fprintf(stderr, "PeekMapPhysicalAddress failed: %d\n", rc);
        return 2;
    }

    printf("mapped address is: %d\n", phys);
    
    pPhys = (char*)phys;

    for (i = 0; i < 20; i++)
    {
        printf("%c", pPhys[i]);
    }
    printf("\n");

    // PeekUnmapPhysicalAddress(phys);

#if 0
    *pPhys = 'W';
    for (i = 0; i < 20; i++)
    {
        printf("%c", pPhys[i]);
    }
    printf("\n");

    *pPhys = 'S';
    for (i = 0; i < 20; i++)
    {
        printf("%c", pPhys[i]);
    }

    printf("\n");
#endif

//    PeekReboot();
}
	

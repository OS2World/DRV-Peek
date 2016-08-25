/* 
    peek.c strategy routine for peek!

    Copyright (c) 1995, 1996 Immisch, Becker & Partner, Hamburg & Lars Immisch

    Author: Lars Immisch <lars@ibp.de>
*/

#include "drvlib.h"
#include "clib.h"
#include "build.h"

#define MAX_MAPPINGS 64

#define NULL (void far *)0
typedef FARPOINTER PVOID;

#include "ioctls.h"

extern void near strat(); /* name of strat rout.*/
void timer_handler();

DEVICEHDR devhdr = 
{
    (void far *) 0xFFFFFFFF,  /* link              */
    (DAW_CHR | DAW_SHR | DAW_OPN | DAW_LEVEL2),/* attribute  */
    (OFF) strat,              /* &strategy         */
    (OFF) 0,                  /* &IDCroutine       */
    "POKE$   "
};


typedef struct _mappings
{
    PID pid;
    ULONG map;
    USHORT inUse;

} MAPPING, far * PMAPPING;

FPFUNCTION      DevHlp=0;      /* for DevHlp calls  */

MAPPING Mappings[MAX_MAPPINGS];

char   startup[] = "\r\nPeek! The final shortcut. v0.1a [build ";
char   startup2[] = "(c) 1995 IBP & Lars Immisch. First tiger Hacko.\r\n";


/* Helper functions */

void fatal(char * error)
{
    InternalError(error, strlen(error));
}

PID currentPID()
{
    PLINFOSEG localInfo;
    void far *ptr;

    if ((ptr = GetDOSVar(2)) == 0)  fatal("poke$: GetDOSVar failed");

    localInfo = *(PLINFOSEG far *)(ptr);

    return localInfo->pidCurrent;
}

PMAPPING allocateMapping(PID pid)
{
    SHORT index;
 
    for (index = 0; index < MAX_MAPPINGS; index++)
    {
        if (!Mappings[index].inUse) 
        {
            Mappings[index].pid = pid;
            Mappings[index].inUse = TRUE;
            return &Mappings[index];
        }
    }

    return NULL;
}

PMAPPING findMapping(PID pid, ULONG map)
{
    SHORT index;
 
    for (index = 0; index < MAX_MAPPINGS; index++)    
        if (Mappings[index].inUse && Mappings[index].pid == pid && Mappings[index].map == map) return &Mappings[index];

    return NULL;
}

/* entry point to strat routine */

unsigned int strategy_handler(PREQPACKET rp)
{
    ULONG handle;
    PEEKPARAMETER far* p;
    ULONG phys;
    ULONG linear;
    PID pid;
    PMAPPING map;
    SHORT rc = 0;
    SHORT index;
    
    switch(rp->RPcommand)
    {
    case RPINIT:       /* 0x00                  */

        /* init called by kernel in prot mode */

        /* store DevHlp entry point */

        DevHlp = rp->s.Init.DevHlp;
    
        /* output initialization message */

        asciizout(startup);
        integerout(BUILD, 10);
        asciizout("]\r\n");
        asciizout(startup2);

        for(index = 0; index < 64; index++) Mappings[index].inUse = FALSE;

        /* send back our cs and ds values to os/2 */

        rp->s.InitExit.finalCS = Limit(timer_handler);
        rp->s.InitExit.finalDS = Limit(&devhdr);
        
        return RPDONE;

    case RPOPEN:       /* 0x0d                  */

        return RPDONE;

    case RPCLOSE:        /* 0x0e                */
        pid = currentPID();
        for (index = 0; index < MAX_MAPPINGS; index++)
        {
            if (Mappings[index].inUse && Mappings[index].pid == pid)
            {
                VMFree(Mappings[index].map);
                Mappings[index].inUse = FALSE;
            }
        }

        return RPDONE;

    case RPIOCTL:            /* 0x10            */

        /* check if request valid */

        if (rp->s.IOCtl.category != IOCTL_CATEGORY) return RPDONE | ERROR_BAD_COMMAND;
        if (rp->s.IOCtl.function < IOCTL_FIRST || rp->s.IOCtl.function > IOCTL_LAST) return RPDONE | ERROR_BAD_COMMAND;

        /* reboot doesn't take parameters */
        if (rp->s.IOCtl.function != IOCTL_REBOOT)
        {
            /* Lock the parameter buffer and verify the users access */
        
            if ((handle = Lock(FP_SEG(rp->s.IOCtl.parameters), 0, SHORT_TERM)) == 0)
            {
                return RPDONE | RPERR | ERROR_GEN_FAILURE;
            }
            if (VerifyAccess(rp->s.IOCtl.parameters, rp->s.IOCtl.parmlength, 1) != 0)
            {
                Unlock(handle);
                return RPDONE | RPERR | ERROR_GEN_FAILURE;
            }
            p = (PEEKPARAMETER far *)rp->s.IOCtl.parameters;
        }
       
        switch (rp->s.IOCtl.function)
        {
        case IOCTL_INBYTE:
            p->inb.inByte = inportb(p->inb.ioAddr);
            break;
        case IOCTL_INWORD:
            p->inw.inWord = inportw(p->inw.ioAddr);
            break;
        case IOCTL_OUTBYTE:
            outportb(p->outb.ioAddr, p->outb.outByte);
            break;
        case IOCTL_OUTWORD:
            outportw(p->outw.ioAddr, p->outw.outWord);
            break;
        case IOCTL_MAP:
            map = allocateMapping(currentPID());
            if (!map)
            {
                rc = ERROR_OUT_OF_MAPPINGS;
                break;
            }
            phys = p->map.physical;
            linear = VirtToLin(&phys);
            if (linear == 0)
            {
                p->map.userVirtual = 0;
                break;
            }
            rc = VMAlloc(linear, &p->map.userVirtual, p->map.size, VMDHA_PHYS | VMDHA_PROCESS);
            map->map = p->map.userVirtual;
            break;
        case IOCTL_FREE_MAP:
            map = findMapping(currentPID(), p->freemap.userVirtual);
            if (!map)
            {
                rc = ERROR_INVALID_MAPPING;
                break;
            }
            VMFree(map->map);
            map->inUse = FALSE;
            break;
        case IOCTL_REBOOT:
            SendEvent(7, 0);
            break;
        }
        if (rp->s.IOCtl.function != IOCTL_REBOOT) Unlock(handle);

        return RPDONE | rc;

    case RPDEINSTALL:  /* 0x14                  */
        /* don't allow deinstall */

        return RPDONE | RPERR | ERROR_BAD_COMMAND;

        /* all other commands are ignored */

    default:
        return RPDONE;

    }
}


void timer_handler()
{
    return;
}



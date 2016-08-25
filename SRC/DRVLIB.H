//  file drvlib.h
//   This header file contains definitions intended to go along with
//   DRVLIB.LIB, a C-callable subroutine library.
//
//   This file is for OS/2 2.0
//
//   adaptions made for Watcom C made by Lars Immisch <lars@ibp.de>

#ifndef _DRVLIB_H_
#define _DRVLIB_H_

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef short           SHORT;
typedef unsigned short  BOOL;
typedef unsigned long   ULONG;
typedef UCHAR near      *PUCHAR;
typedef UCHAR far       *FPUCHAR;
typedef USHORT near     *PUSHORT;
typedef USHORT far      *FPUSHORT;
typedef ULONG near      *PULONG;
typedef ULONG far       *FPULONG;
typedef char near       *PCHAR;
typedef short near      *PSHORT;
typedef short near      *FPSHORT;
typedef long near       *PLONG; 
typedef void near       *POINTER;
typedef POINTER near    *PPOINTER;
typedef void far        *FARPOINTER;
typedef FARPOINTER near *PFARPOINTER;
typedef FARPOINTER far  *FPFARPOINTER;

typedef USHORT          ERRCODE;    // error code returned                 
typedef ERRCODE far     *PERRCODE;  // pointer to an error code            
typedef UCHAR           FLAG;       // 8-bit flag                          
typedef FLAG far        *PFLAG;     // pointer to 8-bit flag               
typedef USHORT          SELECTOR;   // 16-bit selector                     
typedef SELECTOR near   *PSELECTOR; // pointer to a selector               
typedef SELECTOR far    *FPSELECTOR;// far pointer to selector             
typedef USHORT          SEG;        // 16-bit segment                      
typedef USHORT          OFF;        // 16-bit offset                       
typedef void near       *ENTRY;     // near call assembler entry
typedef ULONG           LOFF;       // 32-bit offset                       
typedef USHORT          PID;        // Process ID                          
typedef USHORT          TID;        // Thread ID                           
typedef ULONG           PHYSADDR;   // 32-bit physical address             
typedef ULONG           LINADDR;    // 32-bit linear address               
typedef LINADDR  far    *PLINADDR;  // pointer to 32 bit linear address    
typedef PLINADDR far    *PPLINADDR; // pointer to linear address pointer   
typedef PHYSADDR far    *PPHYSADDR; // pointer to 32-bit physical address  
typedef char near       *PSTRING;   // pointer to character string         
typedef char far        *FPSTRING;// far pointer to string               
typedef USHORT          SHANDLE;    // short (16-bit) handle               
typedef SHANDLE far     *PSHANDLE;  // pointer to a short handle           
typedef ULONG           LHANDLE;    // long  (32-bit) handle               
typedef LHANDLE far     *PLHANDLE;  // pointer to a long handle            

//  pointers to functions 

typedef int (pascal near          *PFUNCTION) ();
typedef int (pascal near * near  *PPFUNCTION) ();
typedef int (pascal far          *FPFUNCTION) ();
typedef int (pascal far  * near *PFPFUNCTION) ();

// macros 

#define FALSE   0
#define TRUE    1

#define NP near pascal

// far pointer from selector-offset 

#define MK_FP(sel, off)     ( (void far *) MAKEULONG(off, sel) )

// get selector or offset from far pointer 

#define FP_SEG(p)         ( ((USHORT far *) &(p)) [1])
#define FP_OFF(p)         ( ((USHORT far *) &(p)) [0])

// Combine l(ow) & h(igh) to form a 32 bit quantity. 

#define MAKEULONG(l, h)  ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)   ((LONG)MAKEULONG(l, h))
#define MAKEBIGOFFSETOF(p) ((ULONG) (OFFSETOF (p)))

// Combine l(ow) & h(igh) to form a 16 bit quantity. 

#define MAKEUSHORT(l, h) (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)  ((SHORT)MAKEUSHORT(l, h))

// get high and low order parts of a 16 and 32 bit quantity 

#define LOBYTE(w)       LOUCHAR(w)
#define HIBYTE(w)       HIUCHAR(w)
#define LOUCHAR(w)      ((UCHAR)(w))
#define HIUCHAR(w)      (((USHORT)(w) >> 8) & 0xff)
#define LOUSHORT(l)     ((USHORT)(l))
#define HIUSHORT(l)     ((USHORT)(((ULONG)(l) >> 16) & 0xffff))

//  the driver device header 

typedef struct DeviceHdr {
   struct DeviceHdr far *DHnext;    // pointer to next header, or FFFF     
   USHORT DHattribute;              // device attribute word               
   ENTRY  DHstrategy;               // offset of strategy routine          
   ENTRY  DHidc;                    // offset of IDC routine               
   UCHAR  DHname[8];                // dev name (char) or #units (blk)     
   char   reserved[8];
   ULONG  bit_strip;                // bit 0 DevIOCtl2, bit 1 32 bit addr  
   } DEVICEHDR;
typedef DEVICEHDR near *PDEVICEHDR;

//  driver device attributes word 

#define DAW_CHR    0x8000           // 1=char, 0=block                     
#define DAW_IDC    0x4000           // 1=IDC available in this DD          
#define DAW_IBM    0x2000           // 1=non-IBM block format              
#define DAW_SHR    0x1000           // 1=supports shared device access     
#define DAW_OPN    0x0800           // 1=open/close, or removable media    
#define DAW_LEVEL1 0x0080           // level 1                             
#define DAW_LEVEL2 0x0100           // level 2 DosDevIOCtl2                
#define DAW_LEVEL3 0x0180           // level 3 bit strip                   
#define DAW_GIO    0x0040           // 1=generic IOCtl supported           
#define DAW_CLK    0x0008           // 1=CLOCK device                      
#define DAW_NUL    0x0004           // 1=NUL device                        
#define DAW_SCR    0x0002           // 1=STDOUT (screen)                   
#define DAW_KBD    0x0001           // 1=STDIN  (keyboard)                 

// capabilities bit strip 

#define CBS_SHD    0x0001           // 1=shutdown/DevIOCtl2                
#define CBS_HMEM   0x0002           // hign memory map for adapters        
#define CBS_PP     0x0004           // supports parallel ports             

// SaveMessage structure 

typedef struct MessageTable {
   USHORT      id;
   USHORT      fill_in_item;
   FARPOINTER  item1;
   FARPOINTER  item2;
   FARPOINTER  item_last;
   } MESSAGETABLE;

// OS/2 circular character queues 

#define QUEUE_SIZE  512             //  size of queues                     
typedef struct CharQueue {
   USHORT   qsize;                  // number of bytes in queue            
   USHORT   qchrout;                // index of next char to put out       
   USHORT   qcount;                 // number of charactes in queue        
   UCHAR    qbuf[QUEUE_SIZE]; 
   } CHARQUEUE;
typedef CHARQUEUE near *PCHARQUEUE;

// AttachDD inter device driver communication data area 

typedef struct AttachArea {
   OFF realOFF;                     // real-mode offset of idc entry point 
   SEG realCS;                      // real-mode CS of IDC entry point     
   SEG realDS;                      // real-mode DS of IDC DD              
   OFF protOFF;                     // protect-mode offset of entry point  
   SELECTOR protCS;                      // protect-mode CS of entry point      
   SELECTOR protDS;                      // protect-mode DS of other DD         
   } ATTACHAREA;
typedef ATTACHAREA near *PATTACHAREA;

// driver request packet 

typedef struct ReqPacket {
   UCHAR RPlength;                  // request packet length               
   UCHAR   RPunit;                  // unit code for block DD only         
   UCHAR RPcommand;                 // command code                        
   USHORT  RPstatus;                // status word                         
   UCHAR   RPreserved[4];           // reserved bytes                      
   ULONG RPqlink;                   // queue linkage                       
   union {                          // command-specific data               
   UCHAR   avail[19];               
    struct {                        // init                                
      UCHAR      units;             // number of units                     
      FPFUNCTION DevHlp;            // &DevHlp                             
      char far   *args;             // &args                               
      UCHAR      drive;             // drive #                             
      }Init;                        
    struct { 
      UCHAR     units;              // same as input                       
      OFF       finalCS;            // final offset, 1st code segment      
      OFF       finalDS;            // final offset, 1st data segment      
      FARPOINTER BPBarray;          // &BPB                                
      } InitExit;

    struct {                        // read, write, write w/verify         
      UCHAR      media;             // media descriptor                    
      PHYSADDR   buffer;            // transfer address                    
      USHORT     count;             // bytes/sectors                       
      ULONG      startsector;       // starting sector#                    
      USHORT     reserved; 
      } ReadWrite;                  

    struct {                        // cached read, write, write w/verify  
      UCHAR      media;             // media descriptor                    
      PHYSADDR   buffer;            // transfer address                    
      USHORT     count;             // bytes/sectors                       
      ULONG      startsector;       // starting sector#                    
      USHORT     reserved; 
      } CReadWrite;
      
    struct {                        // system shutdown                     
      UCHAR      subcode;           // sub request code                    
      ULONG      reserved;
      } Shutdown;

    struct {                        // open/close                          
      USHORT     sysfilenum;        // system file number                  
      } OpenClose;

    struct {                        // IOCtl                               
      UCHAR      category;          // category code                       
      UCHAR      function;          // function code                       
      FARPOINTER parameters;        // &parameters                         
      FARPOINTER data;              // &data
      SHANDLE    hfile;
      USHORT     parmlength;
      USHORT     datalength;
      } IOCtl;                      

    struct {                        // read, no wait                       
      UCHAR      char_returned;     // char to return                      
      } ReadNoWait;                 

    struct {                        // media check                         
      UCHAR      media;             // media descriptor                    
      UCHAR      return_code;       // see #defines                        
      FARPOINTER prev_volume;       // &previous volume ID                 
      } MediaCheck;                 

    struct {                        // build BPB                           
      UCHAR      media;             // media descriptor                    
      FARPOINTER buffer;            // 1-sector buffer FAT                 
      FARPOINTER BPBarray;          // &BPB array                          
      UCHAR      drive;             // drive #                             
      } BuildBPB;                   
                                                                             
    struct {                        // query partitionalble fixed disks    
      UCHAR      count;             // # disks                             
      ULONG      reserved;
      } Partitionable;              
     
    struct {                        // fixed disk LU map                   
      ULONG      units;             // units supported                     
      ULONG      reserved;
      } GetFixedMap;  

    struct {                        // get driver capabilities             
      UCHAR      reserved[3];
      FARPOINTER capstruct;         // 16:16 pointer to DCS                
      FARPOINTER volcharstruct;     // 16:16 pointer to VCS                
      } GetDriverCaps;
              
   } s;                             // command info                        
} REQPACKET;

typedef REQPACKET far *PREQPACKET;
typedef PREQPACKET far *PPREQPACKET;
typedef PREQPACKET QHEAD;           // Queue Head is &ReqPacket            
typedef QHEAD near *PQHEAD;

// Global Info Seg 

typedef struct _GINFOSEG {      
    ULONG   time;
    ULONG   msecs;
    UCHAR   hour;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    USHORT  timezone;
    USHORT  cusecTimerInterval;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    UCHAR   weekday;
    UCHAR   uchMajorVersion;
    UCHAR   uchMinorVersion;
    UCHAR   chRevisionLetter;
    UCHAR   sgCurrent;
    UCHAR   sgMax;
    UCHAR   cHugeShift;
    UCHAR   fProtectModeOnly;
    USHORT  pidForeground;
    UCHAR   fDynamicSched;
    UCHAR   csecMaxWait;
    USHORT  cmsecMinSlice;
    USHORT  cmsecMaxSlice;
    USHORT  bootdrive;
    UCHAR   amecRAS[32];
    UCHAR   csgWindowableVioMax;
    UCHAR   csgPMMax;
} GINFOSEG;
typedef GINFOSEG far *PGINFOSEG;

// local info seg 

typedef struct _LINFOSEG {      
    PID      pidCurrent;
    PID      pidParent;
    USHORT   prtyCurrent;
    TID      tidCurrent;
    USHORT   sgCurrent;
    UCHAR    rfProcStatus;
    UCHAR    dummy1;
    USHORT   fForeground;
    UCHAR    typeProcess;
    UCHAR    dummy2;
    SELECTOR selEnvironment;
    USHORT   offCmdLine;
    USHORT   cbDataSegment;
    USHORT   cbStack;
    USHORT   cbHeap;
    USHORT   hmod;
    SELECTOR selDS;
} LINFOSEG;

typedef LINFOSEG far *PLINFOSEG;

typedef struct _REGSTACK {          // stack usgae structure					
    USHORT  usStruct;					// set to 14 before using              
	 USHORT  usFlags;						// 0x01 means that the interrupt proc  
	                                 // enables interrupts. All others resvd
	 USHORT  usIRQ;						// IRQ of interrupt handler            
	 USHORT  usStackCLI;             // # of stack bytes with interrupts off
	 USHORT  usStackSTI;             // # of stack bytes with interrupts on 
	 USHORT  usStackEOI;					// number of bytes needed after EOI    
	 USHORT  usNest;						// max number of nested levels         
	 } REGSTACK;

typedef REGSTACK near *PREGSTACK;

// page list struct 

typedef struct _PAGELIST {
    ULONG pl_Physaddr;
	 ULONG pl_cb;
	 } PAGELIST;
typedef PAGELIST far *PPAGELIST;

// RPstatus bit values 

#define RPERR   0x8000              //  error occurred, err in RPstatus    
#define RPDEV   0x4000              //  error code defined by driver       
#define RPBUSY  0x0200              //  device is busy                     
#define RPDONE  0x0100              //  driver done with request packet    

// error codes returned in RPstatus 

#define ERROR_WRITE_PROTECT         0x0000
#define ERROR_BAD_UNIT              0x0001
#define ERROR_NOT_READY             0x0002
#define ERROR_BAD_COMMAND           0x0003
#define ERROR_CRC                   0x0004
#define ERROR_BAD_LENGTH            0x0005    
#define ERROR_SEEK                  0x0006
#define ERROR_NOT_DOS_DISK          0x0007
#define ERROR_SECTOR_NOT_FOUND      0x0008
#define ERROR_OUT_OF_PAPER          0x0009
#define ERROR_WRITE_FAULT           0x000A
#define ERROR_READ_FAULT            0x000B
#define ERROR_GEN_FAILURE           0x000C
#define ERROR_DISK_CHANGE           0x000D
#define ERROR_WRONG_DISK            0x000F
#define ERROR_UNCERTAIN_MEDIA       0x0010
#define ERROR_CHAR_CALL_INTERRUPTED 0x0011
#define ERROR_NO_MONITOR_SUPPORT    0x0012
#define ERROR_INVALID_PARAMETER     0x0013
#define ERROR_DEVICE_IN_USE         0x0014

// driver request codes  B=block, C=character 

#define RPINIT          0x00        //  BC                                 
#define RPMEDIA_CHECK   0x01        //  B                                  
#define RPBUILD_BPB     0x02        //  B                                  
#define RPREAD          0x04        //  BC                                 
#define RPREAD_NO_WAIT  0x05        //   C                                 
#define RPINPUT_STATUS  0x06        //   C                                 
#define RPINPUT_FLUSH   0x07        //   C                                 
#define RPWRITE         0x08        //  BC                                 
#define RPWRITE_VERIFY  0x09        //  BC                                 
#define RPOUTPUT_STATUS 0x0a        //   C                                 
#define RPOUTPUT_FLUSH  0x0b        //   C                                 
#define RPOPEN          0x0d        //  BC                                 
#define RPCLOSE         0x0e        //  BC                                 
#define RPREMOVABLE     0x0f        //  B                                  
#define RPIOCTL         0x10        //  BC                                 
#define RPRESET         0x11        //  B                                  
#define RPGET_DRIVE_MAP 0x12        //  B                                  
#define RPSET_DRIVE_MAP 0x13        //  B                                  
#define RPDEINSTALL     0x14        //   C                                 
#define RPPARTITIONABLE 0x16        //  B                                  
#define RPGET_FIXED_MAP 0x17        //  B                                  
#define RPSHUTDOWN      0x1c        //  BC                                 
#define RPGET_DRIVER_CAPS 0x1d      //  B                                  

// check for monitor call in DosOpen/DosClose 

#define MON_OPEN_STATUS   0x08      // open from DosMonOpen                
#define MON_CLOSE_STATUS  0x08      // close from DosMonClose              

// media descriptor byte 

#define MDB_REMOVABLE     0x04      //  1=removable                        
#define MDB_EIGHT_SECTORS 0x02      //  1=8 sectors per track              
#define MDB_DOUBLE_SIDED  0x01      //  1=double-sided media               

// return codes from MediaCheck 

#define MC_MEDIA_UNCHANGED 0x01
#define MC_MEDIA_CHANGED   0xFF
#define MC_MEDIA_UNSURE    0x00

// event numbers for SendEvent 

#define EVENT_SM_MOUSE   0x00       // session switch via mouse            
#define EVENT_CTRLBRK    0x01       // control break                       
#define EVENT_CTRLC      0x02       // control C                           
#define EVENT_CTRLNUMLK  0x03       // control num lock                    
#define EVENT_CTRLPRTSC  0x04       // control printscreen                 
#define EVENT_SHFTPRTSC  0x05       // shift printscreen                   
#define EVENT_SM_KBD     0x06       // session switch hot key              

// #defines for Block

#define INTERRUPTABLE      0
#define NOT_INTERRUPTABLE  1

#define WAIT_INTERRUPTED           2
#define WAIT_TIMEOUT               3

// #defines for Lock
#define SHORT_TERM  0x00
#define LONG_TERM   0x01
#define HIGH_MEMORY 0x02
#define VERIFY_LOCK 0x04

// defines for VMAlloc
#define VMDHA_16MB      0x0001
#define VMDHA_FIXED     0x0002
#define VMDHA_SWAP      0x0004
#define VMDHA_CONTIG    0x0008
#define VMDHA_PHYS      0x0010
#define VMDHA_PROCESS   0x0020
#define VMDHA_SGSCONT   0x0040

FARPOINTER GetDOSVar (USHORT index);
#pragma aux GetDosVar parm [ax]

USHORT SetIRQ  (PFUNCTION handler, USHORT isShared, USHORT interrupt);

USHORT EOI  (USHORT interrupt);
#pragma aux EOI parm [ax]

USHORT Block (FARPOINTER blockId, ULONG timeout, USHORT interruptable);
#pragma aux Block parm [ax bx] [cx di] [dx]

USHORT Run (FARPOINTER blockId);
#pragma aux Run parm [bx ax]

void Yield();

USHORT VerifyAccess(FPFARPOINTER anAddress, USHORT length, USHORT write);
#pragma aux VerifyAccess parm [di ax] [cx] [dx]

LHANDLE Lock(SELECTOR sel, USHORT wait, USHORT flags);
#pragma aux Lock parm [ax] [bx] [dx]

USHORT Unlock(LHANDLE);
#pragma aux Unlock parm [ax bx]

USHORT AllocGDTSelector(FPSELECTOR selectorArray, USHORT count);
#pragma aux AllocGDTSelector parm [es di] [cx]

USHORT PhysToGDTSelector(FARPOINTER phys, USHORT length, SELECTOR selector);
#pragma aux PhysToGDTSelector parm [ax bx] [cx] [si]

FARPOINTER VirtToPhys(FARPOINTER virt);
#pragma aux VirtToPhys parm [ax bx] 

FARPOINTER PhysToVirt(FARPOINTER phys);
#pragma aux PhysToVirt parm [ax bx] 

FARPOINTER    PhysToUVirt(FARPOINTER phys, USHORT length, USHORT mode);
#pragma aux PhysToUVirt parm [ax bx] [cx] [dx]

FARPOINTER AllocPhys(ULONG size, USHORT above1meg);
#pragma aux AllocPhys parm [ax bx] [dx]

SHORT FreePhys(FARPOINTER phys);
#pragma aux FreePhys parm [ax bx]

ULONG    VirtToLin(FARPOINTER virt);
#pragma aux VirtToLin parm [ax bx]

SHORT SendEvent(USHORT event, USHORT argument);
#pragma aux SendEvent parm [ax] [bx]

ULONG    VMAlloc(ULONG physAddress, ULONG far * result, ULONG length, ULONG flags);
#pragma aux VMAlloc parm [di si] [es dx] [cx bx]

void    VMFree(ULONG physAddress);
#pragma aux VMFree parm [dx ax]


// miscellanous functions

void int3();
#pragma aux int3 = \
        "int     3" 

USHORT inportb(USHORT port);
#pragma aux inportb = \
        "in     al, dx" \
        parm [dx]

USHORT outportb(USHORT port, UCHAR value);
#pragma aux outportb = \
        "out    dx, al" \
        parm [dx] [ax]

USHORT inportw(USHORT port);
#pragma aux inportw = \
        "in     ax, dx" \
        parm [dx]

USHORT outportw(USHORT port, USHORT value);
#pragma aux outportw = \
        "out    dx, ax" \
        parm [dx] [ax]

void sti();
#pragma aux sti = "sti"

void cli();
#pragma aux cli = "cli"

void nop();
#pragma aux nop = "nop"

void clc();
#pragma aux clc = "clc"

OFF Limit(FARPOINTER address);
#pragma aux Limit parm [ax bx]

void    InternalError(char * message, USHORT length);
#pragma aux InternalError parm [si] [di]

// these are the only API's available to the driver at Init time

/* Dosopen/DosQFHandState/DosQueryFileInfo et al file attributes; also        */
/* known as Dos File Mode bits...                                             */
#define FILE_NORMAL                0x0000
#define FILE_READONLY              0x0001
#define FILE_HIDDEN                0x0002
#define FILE_SYSTEM                0x0004
#define FILE_DIRECTORY             0x0010
#define FILE_ARCHIVED              0x0020

/* DosOpen() actions */
#define FILE_EXISTED               0x0001
#define FILE_CREATED               0x0002
#define FILE_TRUNCATED             0x0003

/* DosOpen() open flags */
#define FILE_OPEN                  0x0001
#define FILE_TRUNCATE              0x0002
#define FILE_CREATE                0x0010

/* applies if file already exists */

#define OPEN_ACTION_FAIL_IF_EXISTS     0x0000  /* ---- ---- ---- 0000         */
#define OPEN_ACTION_OPEN_IF_EXISTS     0x0001  /* ---- ---- ---- 0001         */
#define OPEN_ACTION_REPLACE_IF_EXISTS  0x0002  /* ---- ---- ---- 0010         */

/* applies if file does not exist */

#define OPEN_ACTION_FAIL_IF_NEW        0x0000  /* ---- ---- 0000 ----         */
#define OPEN_ACTION_CREATE_IF_NEW      0x0010  /* ---- ---- 0001 ----         */

/* DosOpen/DosSetFHandState flags */

#define OPEN_ACCESS_READONLY           0x0000  /* ---- ---- ---- -000         */
#define OPEN_ACCESS_WRITEONLY          0x0001  /* ---- ---- ---- -001         */
#define OPEN_ACCESS_READWRITE          0x0002  /* ---- ---- ---- -010         */
#define OPEN_SHARE_DENYREADWRITE       0x0010  /* ---- ---- -001 ----         */
#define OPEN_SHARE_DENYWRITE           0x0020  /* ---- ---- -010 ----         */
#define OPEN_SHARE_DENYREAD            0x0030  /* ---- ---- -011 ----         */
#define OPEN_SHARE_DENYNONE            0x0040  /* ---- ---- -100 ----         */
#define OPEN_FLAGS_NOINHERIT           0x0080  /* ---- ---- 1--- ----         */
#define OPEN_FLAGS_NO_LOCALITY         0x0000  /* ---- -000 ---- ----         */
#define OPEN_FLAGS_SEQUENTIAL          0x0100  /* ---- -001 ---- ----         */
#define OPEN_FLAGS_RANDOM              0x0200  /* ---- -010 ---- ----         */
#define OPEN_FLAGS_RANDOMSEQUENTIAL    0x0300  /* ---- -011 ---- ----         */
#define OPEN_FLAGS_NO_CACHE            0x1000  /* ---1 ---- ---- ----         */
#define OPEN_FLAGS_FAIL_ON_ERROR       0x2000  /* --1- ---- ---- ----         */
#define OPEN_FLAGS_WRITE_THROUGH       0x4000  /* -1-- ---- ---- ----         */
#define OPEN_FLAGS_DASD                0x8000  /* 1--- ---- ---- ----         */
#define OPEN_FLAGS_NONSPOOLED      0x00040000

#define APIENTRY far pascal

USHORT APIENTRY DosBeep(USHORT, USHORT);
USHORT APIENTRY DosCaseMap(USHORT, FARPOINTER, FARPOINTER);
USHORT APIENTRY DosChgFilePtr(SHANDLE, long, USHORT, FARPOINTER);
USHORT APIENTRY DosClose(SHANDLE);
USHORT APIENTRY DosDelete(FARPOINTER, ULONG);
USHORT APIENTRY DosDevConfig(FARPOINTER, USHORT, USHORT);
USHORT APIENTRY DosDevIOCtl(FARPOINTER, FARPOINTER, USHORT, USHORT, USHORT);
USHORT APIENTRY DosFindClose(SHANDLE);
USHORT APIENTRY DosFindFirst(FARPOINTER, FARPOINTER, USHORT, FARPOINTER,
        USHORT, FARPOINTER, ULONG);
USHORT APIENTRY DosFindNext(SHANDLE, FARPOINTER, USHORT, FARPOINTER);
USHORT APIENTRY DosGetEnv(FARPOINTER, FARPOINTER);
USHORT APIENTRY DosGetMessage(FARPOINTER, USHORT, FARPOINTER, USHORT,
         USHORT, FARPOINTER, FARPOINTER);
USHORT APIENTRY DosOpen(FARPOINTER, FARPOINTER, FARPOINTER, ULONG,
   USHORT, USHORT, USHORT, ULONG);
USHORT APIENTRY DosPutMessage(SHANDLE, USHORT, FARPOINTER);
USHORT APIENTRY DosQCurDir(USHORT, FARPOINTER, FARPOINTER);
USHORT APIENTRY DosQCurDisk(FARPOINTER, FARPOINTER);
USHORT APIENTRY DosQFileInfo(SHANDLE, USHORT, FARPOINTER, USHORT);
USHORT APIENTRY DosQFileMode(FARPOINTER, FARPOINTER, ULONG);
USHORT APIENTRY DosRead(SHANDLE, FARPOINTER, USHORT, FARPOINTER);
USHORT APIENTRY DosWrite(SHANDLE, FARPOINTER, USHORT, FARPOINTER);


#endif
// end of DRVLIB.H 

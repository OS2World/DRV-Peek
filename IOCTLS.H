/*
    ioctls.h
*/

#ifndef _PEEK_IOCTLS_H_
#define _PEEK_IOCTLS_H_

#define IOCTL_CATEGORY          0x90

#define IOCTL_FIRST             0x01
#define IOCTL_INBYTE            0x02
#define IOCTL_INWORD            0x03
#define IOCTL_OUTBYTE           0x04
#define IOCTL_OUTWORD           0x05
#define IOCTL_MAP               0x06
#define IOCTL_FREE_MAP          0x07
#define IOCTL_REBOOT            0x08
#define IOCTL_LAST              0x08

#define ERROR_INVALID_MAPPING 0x42
#define ERROR_OUT_OF_MAPPINGS 0x43

#ifdef _OS2_DRIVER_
#define _Seg16
#else
#define far
#endif

#pragma pack(1)

typedef union _peekParameter
{
    struct map
    {
        ULONG  physical;
        ULONG  userVirtual;
        ULONG  size;
    } map;
    struct freemap
    {
        ULONG  userVirtual;
    } freemap;
    struct inb
    {
        USHORT ioAddr;
        UCHAR  inByte;
    } inb;
    struct inw
    {
        USHORT ioAddr;
        USHORT inWord;
    } inw;
    struct outb
    {
        USHORT ioAddr;
        UCHAR  outByte;
    } outb;
    struct outw
    {
        USHORT ioAddr;
        USHORT outWord;
    } outw;
} PEEKPARAMETER, *PPEEKPARAMETER;

#pragma pack()

#endif

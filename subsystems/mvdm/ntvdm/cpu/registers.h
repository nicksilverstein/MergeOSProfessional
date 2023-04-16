/*
 * COPYRIGHT:       GPL - See COPYING in the top level directory
 * PROJECT:         MergeOS Virtual DOS Machine
 * FILE:            subsystems/mvdm/ntvdm/cpu/registers.h
 * PURPOSE:         Exported functions for manipulating registers
 * PROGRAMMERS:     Hermes Belusca-Maito (hermes.belusca@sfr.fr)
 */

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

/* INCLUDES *******************************************************************/

#if 0   // Those function prototypes are already included via ddk/vddsvc.h

PVOID  WINAPI getIntelRegistersPointer(VOID);

ULONG  WINAPI getEAX(VOID);
VOID   WINAPI setEAX(ULONG);
USHORT WINAPI getAX(VOID);
VOID   WINAPI setAX(USHORT);
UCHAR  WINAPI getAH(VOID);
VOID   WINAPI setAH(UCHAR);
UCHAR  WINAPI getAL(VOID);
VOID   WINAPI setAL(UCHAR);

ULONG  WINAPI getEBX(VOID);
VOID   WINAPI setEBX(ULONG);
USHORT WINAPI getBX(VOID);
VOID   WINAPI setBX(USHORT);
UCHAR  WINAPI getBH(VOID);
VOID   WINAPI setBH(UCHAR);
UCHAR  WINAPI getBL(VOID);
VOID   WINAPI setBL(UCHAR);

ULONG  WINAPI getECX(VOID);
VOID   WINAPI setECX(ULONG);
USHORT WINAPI getCX(VOID);
VOID   WINAPI setCX(USHORT);
UCHAR  WINAPI getCH(VOID);
VOID   WINAPI setCH(UCHAR);
UCHAR  WINAPI getCL(VOID);
VOID   WINAPI setCL(UCHAR);

ULONG  WINAPI getEDX(VOID);
VOID   WINAPI setEDX(ULONG);
USHORT WINAPI getDX(VOID);
VOID   WINAPI setDX(USHORT);
UCHAR  WINAPI getDH(VOID);
VOID   WINAPI setDH(UCHAR);
UCHAR  WINAPI getDL(VOID);
VOID   WINAPI setDL(UCHAR);



ULONG  WINAPI getESP(VOID);
VOID   WINAPI setESP(ULONG);
USHORT WINAPI getSP(VOID);
VOID   WINAPI setSP(USHORT);

ULONG  WINAPI getEBP(VOID);
VOID   WINAPI setEBP(ULONG);
USHORT WINAPI getBP(VOID);
VOID   WINAPI setBP(USHORT);

ULONG  WINAPI getESI(VOID);
VOID   WINAPI setESI(ULONG);
USHORT WINAPI getSI(VOID);
VOID   WINAPI setSI(USHORT);

ULONG  WINAPI getEDI(VOID);
VOID   WINAPI setEDI(ULONG);
USHORT WINAPI getDI(VOID);
VOID   WINAPI setDI(USHORT);

ULONG  WINAPI getEIP(VOID);
VOID   WINAPI setEIP(ULONG);
USHORT WINAPI getIP(VOID);
VOID   WINAPI setIP(USHORT);

USHORT WINAPI getCS(VOID);
VOID   WINAPI setCS(USHORT);
USHORT WINAPI getSS(VOID);
VOID   WINAPI setSS(USHORT);
USHORT WINAPI getDS(VOID);
VOID   WINAPI setDS(USHORT);
USHORT WINAPI getES(VOID);
VOID   WINAPI setES(USHORT);
USHORT WINAPI getFS(VOID);
VOID   WINAPI setFS(USHORT);
USHORT WINAPI getGS(VOID);
VOID   WINAPI setGS(USHORT);

ULONG  WINAPI getCF(VOID);
VOID   WINAPI setCF(ULONG);
ULONG  WINAPI getPF(VOID);
VOID   WINAPI setPF(ULONG);
ULONG  WINAPI getAF(VOID);
VOID   WINAPI setAF(ULONG);
ULONG  WINAPI getZF(VOID);
VOID   WINAPI setZF(ULONG);
ULONG  WINAPI getSF(VOID);
VOID   WINAPI setSF(ULONG);
ULONG  WINAPI getIF(VOID);
VOID   WINAPI setIF(ULONG);
ULONG  WINAPI getDF(VOID);
VOID   WINAPI setDF(ULONG);
ULONG  WINAPI getOF(VOID);
VOID   WINAPI setOF(ULONG);

ULONG  WINAPI getEFLAGS(VOID);
VOID   WINAPI setEFLAGS(ULONG);

USHORT WINAPI getMSW(VOID);
VOID   WINAPI setMSW(USHORT);

#endif

#endif /* _REGISTERS_H_ */

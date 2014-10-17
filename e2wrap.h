#ifndef _EXTDEFS_H_
#define _EXTDEFS_H_

#include "os2head.h"

/* In file e2wrap.asm */
void memset (void FAR *Buffer, UCHAR Value, USHORT Count);
void memcpy (void FAR *Dest, void FAR *Source, USHORT Size);

void VirtToPhys (void FAR *pDataSeg,ULONG FAR *ppDataSeg);
void far * PhysToVirt(ULONG ppXferBuf, USHORT XferLength);

VOID FAR E2FilterIORBWrapper (PIORB pIORB);
PIORB FAR FilterNotifyWrapper (PIORB pIORB);
PIORB FAR PartNotifyWrapper (PIORB pIORB);

int FAR ProcRun (PIORB pIORB);
int ProcBlock(PIORB pIORB);

PDevClassTableStruc E2GetADDTable (void);
USHORT RegisterADD (PADDEntryPoint pAddEP, PSZ DriverName);
void E2Print (PSZ Text);


#endif

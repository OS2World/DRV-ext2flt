/************************************************************************/
/*  Linux partition filter (C) Deon van der Westhuysen.                 */
/*                                                                      */
/*  Dedicated to Jesus Christ, my Lord and Saviour.                     */
/*                                                                      */
/*  Permission is granted to freely use and modify this code for non-   */
/*  profit use on the condition that this notice is not removed. All    */
/*  other rights are reserved. No warranty, etc.                        */
/*                                                                      */
/*  This code is still under development; expect some rough edges.      */
/*                                                                      */
/************************************************************************/

#ifndef _E2INUTIL_H_
#define _E2INUTIL_H_

#include "os2head.h"

USHORT InitAddBaseUnit (PADDEntryPoint pADDEntry, PADAPTERINFO pAdapterInfo);
USHORT InitRemoveBaseUnit (void);
USHORT InitFilterBaseUnit (int Index);
int InitAddVirtualUnit (ULONG StartRBA, ULONG NumSectors, UCHAR SysIndicator);

USHORT InitReadDevTable (PADDEntryPoint pAddEP, PDEVICETABLE pDevTable,
                         USHORT DevTableLen);
USHORT InitAllocateUnit (PADDEntryPoint pAddEP, USHORT UnitHandle);
USHORT InitFreeUnit (PADDEntryPoint pAddEP, USHORT UnitHandle);
USHORT InitChangeUnitInfo (PADDEntryPoint pAddEP, USHORT UnitHandle,
                           PUNITINFO pUnitInfo);
USHORT InitGetGeometry (PADDEntryPoint pAddEP, USHORT UnitHandle,
                        USHORT FAR *NumHeads, USHORT FAR *TrackSectors);
USHORT InitReadSector (PADDEntryPoint pAddEP, USHORT UnitHandle,
                       ULONG SectorRBA, ULONG ppSectorBuf);
USHORT InitSendIORB(PIORB pIORB, PADDEntryPoint pADDEntry);

#endif

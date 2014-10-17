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

#ifndef _E2VIRTIO_H_
#define _E2VIRTIO_H_

#include "os2head.h"

USHORT DoVirtualIO (USHORT Modifier, NPVirtUnitRec pUnitRec, ULONG SectorRBA,
                    PSCATGATENTRY pSGList, USHORT cSGList,ULONG XferOffset);

USHORT ReadFakeSector (NPVirtUnitRec pUnitRec, ULONG SectorRBA,
                       PSCATGATENTRY pSGList, USHORT cSGList,
                       ULONG XferOffset);
USHORT WriteFakeSector (NPVirtUnitRec pUnitRec, ULONG SectorRBA,
                       PSCATGATENTRY pSGList, USHORT cSGList,
                       ULONG XferOffset);

VOID ComputeCHS (ULONG RBA, USHORT GeoNumHead, USHORT GeoTrackSec,
                 USHORT FAR *CylSec, UCHAR FAR *Head);
USHORT CopyToSGList (void FAR *Buffer, ULONG Count, PSCATGATENTRY pSGList,
                     USHORT cSGList, ULONG XferOffset);

#endif

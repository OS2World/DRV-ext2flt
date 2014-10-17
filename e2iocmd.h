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

#ifndef _E2IOCMD_H_
#define _E2IOCMD_H_

#include "os2head.h"

void BadCommand (NPVirtUnitRec pUnitRec, PIORB pIORB);
void PartGetDeviceTable (NPVirtUnitRec pUnitRec, PIORB pIORB);
void PartChangeUnitInfo (NPVirtUnitRec pUnitRec, PIORB pIORB);
void PartGetGeometry (NPVirtUnitRec pUnitRec, PIORB pIORB);
void PartDoIO (NPVirtUnitRec pUnitRec, PIORB pIORB);
void PartGetUnitStatus (NPVirtUnitRec pUnitRec, PIORB pIORB);

#endif

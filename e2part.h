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

#ifndef _E2PART_H_
#define _E2PART_H_

#include "os2head.h"

void PartHandler (PIORB pIORB);
void PartNotify (PIORB pIORB);
void PartFilterIORB (NPVirtUnitRec pUnitRec, PIORB pIORB);
void PartCommandDone (NPVirtUnitRec pUnitRec, PIORB pIORB);

void SetLongInSGList (PSCATGATENTRY pSGList, USHORT cSGList,
                      ULONG Offset,ULONG NewValue);

#endif

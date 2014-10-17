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

#ifndef _E2ROUTER_H_
#define _E2ROUTER_H_

#include "os2head.h"

void E2FilterIORB (PIORB pIORB);
void AddIORBToQueue (NPIORBQueue pQueue, PIORB pIORB);
PIORB GetIORBFromQueue (NPIORBQueue pQueue);
void StartIORBQueue (NPIORBQueue pQueue);

void NotifyDone (PIORB pIORB);
void IORBError (PIORB pIORB, USHORT ErrorCode);

#endif

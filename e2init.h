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

#ifndef _E2INIT_H_
#define _E2INIT_H_

#include "os2head.h"

void E2Init (PRPINITOUT Req);
void InitScanDrivers (void);
void InitScanAdapter (PADDEntryPoint pADDEntry);
void InitScanUnits (PADDEntryPoint pADDEntry, PADAPTERINFO pAdapterInfo);
PADDEntryPoint InitGetDriverEP (USHORT ADDHandle);
USHORT InitScanPartitions (PADDEntryPoint pAddEP);
int InitIsVirtType(UCHAR PartType);
void ProcessCmdline (char FAR *pCmdLine);
void InitGetUnitFS (void);
void InitSortUnits (void);
void InitPrint (char FAR *Message);
void InitPrintVerbose (char FAR *Message);

#endif

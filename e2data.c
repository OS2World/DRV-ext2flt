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

#include "e2data.h"

PFN		DevHelp= NULL;		/* Ptr to DevHelp service routine */
USHORT		ADDHandle= 0;		/* Our adapter device driver handle */
void far *	pDataSeg= NULL;		/* Virtual address of data segment */
ULONG		ppDataSeg= 0L;		/* Physical address of data segment */

int		InstallFlags= 0;
TBaseUnitRec	BaseUnits[MAX_LINUX_PARTITIONS]= {0};
int		NumBaseUnits= 0;
TVirtUnitRec	VirtUnits[MAX_LINUX_PARTITIONS]= {0};
int		NumVirtUnits= 0;
USHORT		MountTable[MAX_LINUX_PARTITIONS]= {0};
int		MountCount= 0;

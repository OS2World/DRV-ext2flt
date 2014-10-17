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

#include "debug.h" 
#include "e2data.h"
#include "e2router.h"
#include "e2wrap.h"
#include "e2inutil.h"
#include "e2init.h"

USHORT	StartInitData= 0;	/* Use addr as pointer to start of init data */

/* Initialization function for the driver. Only called once. */
void E2Init (PRPINITOUT Req)
{
 int		UnitCount;
 char FAR	*pCmdLine;

 /* Initialize some of the important global variables. */
 DevHelp= ((PRPINITIN) Req)->DevHlpEP;		/* Get pointer to DevHelp */
 pDataSeg= (PVOID) &pDataSeg;			/* Get pointer to data seg */
 OFFSETOF(pDataSeg)= 0;
 VirtToPhys (pDataSeg,&ppDataSeg);		/* Get the physical address */
						/* of the data segment */
 for (MountCount= 0; MountCount<MAX_LINUX_PARTITIONS; MountCount++)
  MountTable[MountCount]= MountCount;
 						/* MountCount= max num of */
						/* partitions to mount. */

 pCmdLine= ((PRPINITIN) Req)->InitArgs;		/* Get command line args */
 OFFSETOF(pCmdLine)=(USHORT) ((PDDD_PARM_LIST) pCmdLine)->cmd_line_args;

 ProcessCmdline (pCmdLine);
 InitPrint ("Linux partition filter. (C) Deon van der Westhuysen. v1.2\n\r"
            "Development version (Alpha).\n\r");

 InitScanDrivers();				/* Scan for partitions... */
 InitGetUnitFS();				/* Determine FS for each unit */
 InitSortUnits();				/* Sort the units */

 if (MountCount>NumVirtUnits)			
  MountCount =NumVirtUnits;			/* Get correct number of */
						/* units to be mounted. */

 /* Check that each entry points to a valid unit. If not valid, point to */
 /* first unit. (Which can only be allocated once, thus one drive letter.) */
 for (UnitCount=0;UnitCount<MountCount;UnitCount++)
  if (MountTable[UnitCount]>=NumVirtUnits)
   MountTable[UnitCount]= 0;

 if (!NumVirtUnits)
 {
  InitPrint ("No Linux partitions were found: filter not installed.");
  Req->Unit= 0;
  Req->CodeEnd= 0;				/* No code to keep */
  Req->DataEnd= 0;				/* No data to keep */
  Req->rph.Status= STDON+STERR+ERROR_I24_QUIET_INIT_FAIL;
						/* Indicate failure */
  return;
 }

 ADDHandle= RegisterADD (E2FilterIORBWrapper,FILTER_ADD_NAME);
						/* Register filter */
 if (!ADDHandle)				/* Check registration */
 {
  InitPrint ("Could't register filter. Installation aborted.");
  while (NumBaseUnits) InitRemoveBaseUnit();	/* Free all base units */
  Req->Unit= 0;
  Req->CodeEnd= 0;				/* No code to keep */
  Req->DataEnd= 0;				/* No data to keep */
  Req->rph.Status= STDON+STERR+ERROR_I24_QUIET_INIT_FAIL;
						/* Indicate failure */
  return;
 }

 for (UnitCount= 0; UnitCount<NumBaseUnits; UnitCount++)
  InitFilterBaseUnit (UnitCount);		/* Filter all base units... */

 InitPrintVerbose ("Filter installed.");
 Req->Unit= 0;

 Req->CodeEnd= ((USHORT)E2Init);		/* Pointer to end of code */
 Req->DataEnd= ((USHORT)&StartInitData);	/* Pointer to end of data */
 Req->rph.Status= STDON;			/* Everything is OK */
}

/* Scan the system for installed device drivers. Scan the drivers for */
/* adapters in turn. */
void InitScanDrivers (void)
{
 PADDEntryPoint		pADDEntry;	/* .ADD entry point adapter */
 PDevClassTableStruc	pDCTable;	/* Pointer to list of ADDs */
 PDevClassTableEntry	pDCEntry;	/* Pointer to device entry */
 int			DeviceCount;

 pDCTable= E2GetADDTable();
 InitPrintVerbose ("Scanning Adapter device drivers:");
 for (DeviceCount= pDCTable->DCCount,pDCEntry= pDCTable->DCTableEntries;
      DeviceCount--;
      pDCEntry++)
 {
  InitPrintVerbose (pDCEntry->DCName);
  pADDEntry = (PVOID) MAKEP(pDCEntry->DCSelector,pDCEntry->DCOffset);
  InitScanAdapter (pADDEntry);
 }
}

/* This function scans a driver for all the adapters it controls. */
void InitScanAdapter (PADDEntryPoint pADDEntry)
{
 static UCHAR	DeviceTable[MAX_DEVTABLE_SIZE];	/* Holds info about adapters */
 PADAPTERINFO	pAdapterInfo;			/* Pointer to adapter info */
 int		NumAdapters;
 int		AdapterCount;

 if (InitReadDevTable(pADDEntry,(PDEVICETABLE) &DeviceTable,
                      sizeof(DeviceTable)))	/* Get list of adapters */
 {
  InitPrintVerbose ("Error reading the device table... skipping device.");
  return;
 }

 NumAdapters= ((PDEVICETABLE)&DeviceTable)->TotalAdapters;
 for (AdapterCount= 0;AdapterCount< NumAdapters;AdapterCount++)
 {
  pAdapterInfo= ((PDEVICETABLE) &DeviceTable)->pAdapter[AdapterCount];
  InitPrintVerbose ("Scanning adapter...");
  if (pAdapterInfo->AdapterFlags&AF_ASSOCIATED_DEVBUS)
   InitPrintVerbose ("Skipping associated device bus of the adapter.");
  else
   InitScanUnits (pADDEntry,pAdapterInfo);
 }
}

/* This function scans an adapter for all the units it controls. */
void InitScanUnits (PADDEntryPoint pADDEntry, PADAPTERINFO pAdapterInfo)
{
 PUNITINFO	pUnitInfo;			/* Information about unit */
 USHORT		UnitFlags;			/* Flags for current unit */
 USHORT		UnitType;			/* Type flag for units */
 USHORT		FilterHandle;			/* Handle of unit filter */
 PADDEntryPoint	pUnitADDEntry;			/* Entry point for unit */
 NPBaseUnitRec	pBaseUnit;			/* Pointer to base unit rec */
 int		NumUnits;			/* Num units on adapter */
 int		UnitCount;

 NumUnits= pAdapterInfo->AdapterUnits;
 pUnitInfo= pAdapterInfo->UnitInfo;

 /* Modify some fields in adapter info for later use by base units. */
 memcpy (pAdapterInfo->AdapterName,FILTER_ADD_NAME,sizeof(FILTER_ADD_NAME));
 pAdapterInfo->AdapterUnits= 1;
 pAdapterInfo->AdapterFlags&=~(AF_IBM_SCB|AF_CHS_ADDRESSING);

 for (UnitCount= 0;UnitCount<NumUnits;UnitCount++)
 {
  InitPrintVerbose ("Scanning unit...");
  if (UnitCount)				/* If not first unit: */
   memcpy (pUnitInfo,pAdapterInfo->UnitInfo+UnitCount,sizeof(UNITINFO));
						/* Copy UnitInfo into first */
						/* slot in the array */
  UnitFlags= pUnitInfo->UnitFlags;
  UnitType= pUnitInfo->UnitType;
  FilterHandle= pUnitInfo->FilterADDHandle;
  if (FilterHandle)
   pUnitADDEntry= InitGetDriverEP (FilterHandle);
  else
   pUnitADDEntry= pADDEntry;
  if (UnitType!=UIB_TYPE_DISK)
  {
   InitPrintVerbose ("Unit is not a disk, skipping.");
   continue;
  }
  if (UnitFlags&UF_REMOVABLE)
  {
   InitPrintVerbose ("Unit is removable (=not partitioned), skipping.");
   continue;
  }
  if (UnitFlags&(UF_NODASD_SUPT|UF_DEFECTIVE))
  {
   InitPrintVerbose ("Unit defective or no OS2DASD.DMD support, skipping.");
   continue;
  }
  if (!InitAddBaseUnit(pUnitADDEntry,pAdapterInfo))
   if (!InitScanPartitions(pUnitADDEntry))	/* If no partitions found: */
    InitRemoveBaseUnit();			/* Free base unit again */
 }
}

/* Return the entry point for a driver given its handle */
PADDEntryPoint InitGetDriverEP (USHORT FilterHandle)
{
 PDevClassTableStruc	pDCTable;	/* Pointer to list of ADDs */
 PDevClassTableEntry	pDCEntry;	/* Pointer to device entry points */

 pDCTable= E2GetADDTable();		/* Get device class list */
 pDCEntry= pDCTable->DCTableEntries+FilterHandle-1;
					/* Point to entry for ADDHandle */
 return (PVOID) MAKEP(pDCEntry->DCSelector,pDCEntry->DCOffset);
					/* Return the entry point */
}

/* Scan for partitions on a given unit. */
USHORT InitScanPartitions (PADDEntryPoint pAddEP)
{
 static MBR	BootRecord= {0};
 NPBaseUnitRec	pBaseUnit;
 USHORT		UnitHandle;
 USHORT		Result;
 int		FoundExt;
 int		NumAdded;
 int		Count;
 PARTITIONENTRY	*pPartInfo;
 ULONG		PartRBA= 0;
 ULONG		ExtPartRBA= 0;

 pBaseUnit= BaseUnits+NumBaseUnits-1;
 UnitHandle= pBaseUnit->UnitHandle;
 NumAdded= 0;					/* Number partitions added */
						/* to partition list */
 InitPrintVerbose ("Reading master boot record.");
 do
 {
  FoundExt= 0;
  if (InitReadSector (pAddEP,UnitHandle,PartRBA,
                      ppDataSeg+((USHORT)&BootRecord)))
  {						/* If error reading sector */
   InitPrint ("Read failure... :-(");
   break;					/* Done with this unit */
  }
  if (BootRecord.Signature!=0xAA55)		/* Test MBR signature */
  {
   InitPrint ("Invalid signature in partition table.");
   break;					/* Done with this unit */
  }
  pPartInfo= BootRecord.PartitionTable;		/* Ptr to partition entry */
  
  for (Count=0;Count<4;Count++)		/* Scan for linux partitions... */
   if (InitIsVirtType(pPartInfo[Count].SysIndicator))
   {
    InitPrintVerbose ("Found a partition to virtualize.");
    NumAdded+= InitAddVirtualUnit (PartRBA+pPartInfo[Count].RelativeSectors,
                                   pPartInfo[Count].NumSectors,
                                   pPartInfo[Count].SysIndicator);
   }
  for (Count=0;Count<4;Count++)		/* Scan for extended partitions... */
   if (pPartInfo[Count].SysIndicator==PARTITION_EXTENDED)
   {
    InitPrintVerbose ("Found an extended partition.");
    FoundExt= 1;
    PartRBA= ExtPartRBA+ pPartInfo[Count].RelativeSectors;
    if (!ExtPartRBA) ExtPartRBA= PartRBA;
   }
 } while (FoundExt);
 return NumAdded;
}

/* Return 1 if the partition should be virtualized; else returns 0. */
int InitIsVirtType(UCHAR PartType)
{
 if ((PartType==PARTITION_LINUX)||
     (PartType==(PARTITION_LINUX|PARTITION_HIDDEN)))
  return 1;
 if (!(InstallFlags&FI_ALLPART))
  return 0;
 switch (PartType)
 {
  case PARTITION_DOSSMALL:
  case PARTITION_DOSMED:
  case PARTITION_DOSLARGE:
  case PARTITION_IFS:

  case PARTITION_DOSSMALL | PARTITION_HIDDEN:
  case PARTITION_DOSMED | PARTITION_HIDDEN:
  case PARTITION_DOSLARGE | PARTITION_HIDDEN:
  case PARTITION_IFS | PARTITION_HIDDEN:
		return 1;
  default:	return 0;
 }
}

/* Function to scan the command line and set global variables based on */
/* options specified in the command line. */
void ProcessCmdline (char FAR *pCmdLine)
{
 int	TempInt;
 char	TempChar;

 /* InstallFlags= 0 on entry... */

 while (*pCmdLine)
 {
  if (*(pCmdLine++)!='/')			/* Scan for a '/' on cmdline */
   continue;
  switch (*(pCmdLine++))
  {
   case 'm':
   case 'M':	MountCount= 0;			/* Assume nothing to mount */
		while (1)			/* Loop to read numbers */
		{
		 while ((*pCmdLine==' ')||(*pCmdLine=='\t')) pCmdLine++;
						/* Skip any spaces */
		 if (!*pCmdLine) break;		/* Exit if end-of-line */

		 /* Read a number here... */
		 TempChar= *pCmdLine;
		 TempInt= 0;
		 while ((TempChar>='0')&&(TempChar<='9'))
                 {
		  TempInt= TempInt*10+TempChar-'0';
                  TempChar= *(++pCmdLine);
		 }
                 MountTable[MountCount++]= TempInt;

		 while ((*pCmdLine==' ')||(*pCmdLine=='\t')) pCmdLine++;
						/* Skip any spaces */
		 if (!*pCmdLine) return;	/* Exit if end-of-line */
                 if (*pCmdLine!=',') break;	/* Stop if no next number */
		 pCmdLine++;
                }
		break;
   case 'q':
   case 'Q':	InstallFlags |= FI_QUIET;
		break;
   case 'v':
   case 'V':	InstallFlags |= FI_VERBOSE;
		break;
   case 'a':
   case 'A':	InstallFlags |= FI_ALLPART;
		break;
   case 'w':
   case 'W':	InstallFlags |= FI_ALLOWWRITE;
		break;
   default:	InitPrintVerbose ("Unknown command line option encountered");
		break;
  }
 }
}

void InitGetUnitFS (void)
{
 static UCHAR	SectorBuf[SECTOR_SIZE]= {0};
 int		Count;
 NPVirtUnitRec	pUnitRec;
 NPBaseUnitRec	pSourceRec;

 for (Count= 0;Count<NumVirtUnits; Count++)
 {
  pUnitRec=VirtUnits+Count;
  pSourceRec=pUnitRec->pSourceUnitRec;
  switch ((pUnitRec->PartSysIndicator)&~PARTITION_HIDDEN)
  {
   case PARTITION_DOSSMALL:
   case PARTITION_DOSMED:
   case PARTITION_DOSLARGE:
/*BREAK*/
	pUnitRec->FSType=FS_DOSFAT;		/* Assume this partitions of */
						/* this type will always be */
						/* FAT partitions. */
	if (InitReadSector (pSourceRec->pADDEntry,pSourceRec->UnitHandle,
                            pUnitRec->StartRBA,ppDataSeg+((USHORT)&SectorBuf)))
        					/* Error- shouldn't happen */
	 pUnitRec->Hdr.Flags|=F_ALLOCATED;	/* Allocate so that no one */
						/* else can use this unit */
	else
	 pUnitRec->FATHiddenSectors=*((ULONG *)(SectorBuf+0x1C));
						/* Save num of hidden sectors */
	break;
   default:
	pUnitRec->FSType=FS_UNKNOWN;
				break;
  }
 }
}

void InitSortUnits (void)
{
}

void InitPrint (char FAR *Message)
{
 if (!(InstallFlags&FI_QUIET))
  E2Print (Message);
}

void InitPrintVerbose (char FAR *Message)
{
 if (InstallFlags&FI_VERBOSE)
  InitPrint (Message);
}

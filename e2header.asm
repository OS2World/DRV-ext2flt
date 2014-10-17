;/************************************************************************/
;/*  Linux partition filter (C) Deon van der Westhuysen.                 */
;/*                                                                      */
;/*  Dedicated to Jesus Christ, my Lord and Saviour.                     */
;/*                                                                      */
;/*  Permission is granted to freely use and modify this code for non-   */
;/*  profit use on the condition that this notice is not removed. All    */
;/*  other rights are reserved. No warranty, etc.                        */
;/*                                                                      */
;/*  This code is still under development; expect some rough edges.      */
;/*                                                                      */
;/************************************************************************/

	Title	E2Header- Device driver header and segment ordering.

locals
.386P

include reqpkt.inc
include devcmd.inc
DRIVER_INIT= 1

DGROUP	group _DATA,_BSS

_DATA	segment word public 'DATA' use16
	public DevHeader
DevHeader	dd -1			; Pointer to next driver
		dw 8180h		; Device attributes
		dw offset E2Strategy	; Strategy routine entry point
		dw 0			; IDC routine entry point
		db 'Ext2Flt$'		; Device name
		db 8 dup (0)		; Reserved
		dd 8			; Level 3 device drive capabilities
					; 8= Adapter device driver
	public InitFlags
InitFlags	db 0

_DATA	ends

_BSS	segment word public 'BSS' use16
_BSS	ends

_TEXT	segment byte public 'CODE' use16

	assume cs:_TEXT, ds:DGROUP
	extrn _E2Init: near

	public E2Strategy
E2Strategy	proc far
	cmp byte ptr es:[bx+ReqCommand],CMDInitBase
	jne short @@BadCommand
	test InitFlags,DRIVER_INIT
	jne short @@BadCommand
	or InitFlags,DRIVER_INIT
	push es
	push bx
	call _E2Init
	add sp,4
	jmp short @@Exit
@@BadCommand:
	mov word ptr es:[bx+ReqStatus],8103h	; Done, error
@@Exit:
	retf
E2Strategy	endp

_TEXT	ends

	end

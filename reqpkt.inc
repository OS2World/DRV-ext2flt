; Definition of the request packet header.

ReqPacket	struc
ReqLenght	db ?
ReqUnit		db ?
ReqCommand	db ?
ReqStatus	dw ?
ReqFlags	db ?
		db 3 dup (?)	; Reserved field
ReqLink		dd ?
		ends


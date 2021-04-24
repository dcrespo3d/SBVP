/*Fichero XMS_BUF.CPP*/

#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include "xms_buf.h"
#include "sbvperr.h"

xmsbuffer::xmsbuffer(unsigned nkbytes)
{
	movestruct=NULL;
	if ((movestruct=(_movestruct far *)farcalloc
	(1,sizeof(_movestruct)))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
	}
	last_error=reserva(nkbytes);
}

xmsbuffer::~xmsbuffer()
{
	libera();
	if (movestruct!=NULL)
		farfree(movestruct);
}

unsigned xmsbuffer::GetBiggestBlock(void)
{
	int BigBlockSize;
	char ErrorCode;
	last_error=OK;
	asm{
		MOV AX,0x0800
		CALL [xmsfunc]
		MOV ErrorCode,BL
		MOV BigBlockSize,AX
	}
	switch (ErrorCode){
		case 0x80:
			return 0xFFFF;
		case 0x81:
		case 0xA0:
			return 0;
	}
	return(BigBlockSize);
}

unsigned xmsbuffer::GetFreeMem(void)
{
	int FreeMem;
	char ErrorCode;
	last_error=OK;
	asm{
		MOV AX,0x0800
		CALL [xmsfunc]
		MOV ErrorCode,BL
		MOV FreeMem,DX
	}
	switch (ErrorCode){
		case 0x80:
			return 0xFFFF;
		case 0x81:
		case 0xA0:
			return 0;
	}
	return(FreeMem);
}

int xmsbuffer::reserva(unsigned nkbytes)
{
	int respuesta;
	unsigned char cod_error;
	unsigned temp_handle;
	if (nkbytes>GetFreeMem())
		return 0x00A0;	//No XMS libre
	asm{
		MOV AH,0x09
		MOV DX,nkbytes
		CALL [xmsfunc]
		MOV temp_handle,DX
		MOV respuesta,AX
		MOV cod_error,BL
	}
	handle=temp_handle;
	if (!respuesta)
	{
		last_error=cod_error;
		return(cod_error);
	}
	else
	{
		last_error=OK;
		return(OK);
	}
}

int xmsbuffer::libera(void)
{
	unsigned temp_handle;
	int respuesta;
	unsigned char cod_error;
	temp_handle=handle;
	asm{
		MOV AH,0x0A
		MOV DX,temp_handle
		CALL [xmsfunc]
		MOV respuesta,AX
		MOV cod_error,BL
	}
	if (!respuesta)
	{
		last_error=cod_error;
		return(cod_error);
	}
	else
	{
		last_error=OK;
		return(OK);
	}
}

int xmsbuffer::leer(unsigned char far *destino,unsigned long pos,
	unsigned long nbytes)
{
	_movestruct far *temp_movestruct;
	int respuesta;
	unsigned char cod_error;
	temp_movestruct=movestruct;
	movestruct->sourcehandle=handle;
	movestruct->desthandle=0;
	movestruct->sourceoffset=pos;
	movestruct->destoffset=(unsigned long)destino;
	movestruct->length=nbytes;
	asm{
		PUSH DS
		PUSH SI
		PUSH ES
		PUSH SEG xmsfunc
		PUSH SEG temp_movestruct
		POP DS
		LDS SI,temp_movestruct
		MOV AH,0x0B
		POP ES
		CALL [DWORD PTR ES:xmsfunc]
		POP ES
		POP SI
		POP DS
		MOV respuesta,AX
		MOV cod_error,BL
	}
	if (!respuesta)
	{
		last_error=cod_error;
		return(cod_error);
	}
	else
	{
		last_error=OK;
		return(OK);
	}
}

int xmsbuffer::escribir(unsigned char *origen,unsigned long pos,
	unsigned long nbytes)
{
	_movestruct far *temp_movestruct;
	int respuesta;
	unsigned char cod_error;
	temp_movestruct=movestruct;
	movestruct->sourcehandle=0;
	movestruct->desthandle=handle;
	movestruct->sourceoffset=(unsigned long)origen;
	movestruct->destoffset=pos;
	movestruct->length=nbytes;
	asm{
		PUSH DS
		PUSH SI
		PUSH ES
		PUSH SEG xmsfunc
		PUSH SEG temp_movestruct
		POP DS
		LDS SI,temp_movestruct
		MOV AH,0x0B
		POP ES
		CALL [DWORD PTR ES:xmsfunc]
		POP ES
		POP SI
		POP DS
		MOV respuesta,AX
		MOV cod_error,BL
	}
	if (!respuesta)
	{
		last_error=cod_error;
		return(cod_error);
	}
	else
	{
		last_error=OK;
		return(OK);
	}
}

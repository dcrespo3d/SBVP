/*Fichero XMS_CTRL.CPP - Fichero B sico para la gesti¢n
	de memoria extendida (XMS)*/

#include "xms_ctrl.h"
#include "sbvperr.h"

int RETF=0xCB;	//retf
void far *xmsfunc=(void *) &RETF;	//puntero far a la funci¢n que realiza
					//el control de la memoria extendida

xmscontrol::xmscontrol(void)
{
	unsigned char respuesta;
	last_error=OK;
	asm{
		MOV AX,0x4300
		INT 0x2F
		MOV respuesta,AL
	}
	if (respuesta!=0x80)
	{
		last_error=XMS_NOT_INSTALLED;
	}
	else
	{
		asm{
			MOV AX,0x4310
			INT 0x2F		//obtener un puntero a la
			MOV WORD PTR[xmsfunc],BX		//funcion de
			MOV WORD PTR[xmsfunc+2],ES		//control XMS
		}
	}
}

unsigned xmscontrol::getfree(void)
{
	unsigned free_xms;
	char respuesta;
	last_error=OK;
	asm{
		MOV AX,0x0800
		CALL [xmsfunc]
		MOV respuesta,BL
		MOV free_xms,DX
	}
	switch (respuesta){
		case 0x80:{}	//posibles errores
		case 0x81:{}
		case 0xA0:{last_error=respuesta;break;}
	}
	last_error=OK;
	return(free_xms);
}

unsigned xmscontrol::getversion(void)
{
	unsigned version_number;
	asm{
		MOV AX,0x0000
		CALL [xmsfunc]
		MOV version_number,AX
	}
	return(version_number);
}

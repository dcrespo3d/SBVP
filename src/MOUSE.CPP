#include "mouse.h"
#include "sbvperr.h"
#include <conio.h>

void cursor_flecha(void)
{
	unsigned cur[]={0x3FFF,0x1FFF,0x0FFF,0x07FF,0x03FF,0x01FF,0x00FF,0x007F,
					0x003F,0x001F,0x01FF,0x10FF,0x30FF,0xF87F,0xF87F,0xFC7F,
					0x0000,0x4000,0x6000,0x7000,0x7800,0x7C00,0x7E00,0x7F00,
					0x7F80,0x7C00,0x6C00,0x4600,0x0600,0x0300,0x0300,0x0000};

	unsigned far *cursor;
	cursor=cur;
	asm{
		MOV AX,9
		MOV BX,0
		MOV CX,0
		MOV DX,word ptr[cursor+2]
		MOV ES,DX
		MOV DX,word ptr[cursor]
		INT 0x33
	}
}

void cursor_reloj(void)
{
	unsigned cur[]={0x007F,0x007F,0x80FF,0x80FF,0x80FF,0x80FF,0xC1FF,0xE3FF,
					0xE3FF,0xC1FF,0x80FF,0x80FF,0x80FF,0x80FF,0x007F,0x007F,
					0x0000,0x0000,0x3E00,0x3E00,0x2A00,0x3600,0x1C00,0x0800,
					0x0800,0x1400,0x3E00,0x3600,0x2A00,0x1400,0x0000,0x0000};

	unsigned far *cursor;
	cursor=cur;
	asm{
		MOV AX,9
		MOV BX,0
		MOV CX,0
		MOV DX,word ptr[cursor+2]
		MOV ES,DX
		MOV DX,word ptr[cursor]
		INT 0x33
	}
}

int mouseinstalled(void)
{
	int respuesta;
	asm{
		MOV AX,0x0000
		INT 0x33
		MOV respuesta,AX
	}
	if (!respuesta)
		return (MOUSE_NOT_INSTALLED);
	else
	{
		SetXRange(0,639);
		SetYRange(0,479);
		return (OK);
	}
}

void muestra_cursor(void)
{
	asm{
		MOV AX,0x0001
		INT 0x33
	}
}

void oculta_cursor(void)
{
	asm{
		MOV AX,0x0002
		INT 0x33
	}
}

void leeraton(int *x,int *y,int *button)
{
	static int x0,y0,button0;
	asm{
		MOV AX,0x0003
		INT 0x33
		MOV x0,CX
		MOV y0,DX
		MOV button0,BX
	}
	*x=x0;*y=y0;*button=button0;
}

void esperar_soltar(void)
{
	int button=1;
	while(button)
	{
		asm{
			MOV AX,0x0003
			INT 0x33
			MOV button,BX
		}
	}
}

void SetXRange(int nMinX,int nMaxX)
{
	asm{
		MOV AX,0x0007
		MOV CX,nMinX
		MOV DX,nMaxX
		INT 0x33
	}
}

void SetYRange(int nMinY,int nMaxY)
{
	asm{
		MOV AX,0x0008
		MOV CX,nMinY
		MOV DX,nMaxY
		INT 0x33
	}
}

void LiberarTecla(void)
{
	while (kbhit())
		getch();
}

int TeclaPulsada(void)
{
	if (kbhit())
		return getch();
	else return (0);
}

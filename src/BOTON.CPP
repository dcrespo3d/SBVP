#include <graphics.h>
#include <_null.h>
#include "krpfont.h"
#include "mouse.h"
#include "boton.h"

int boton::permiso_global=1;
//int boton::actualizar=0;

boton::boton(unsigned posx,unsigned posy,unsigned tamx,unsigned tamy,
			char *texto_boton,krpfont *ktextobject)
{
	x1=posx;
	x2=posx+tamx;
	y1=posy;
	y2=posy+tamy;
	centrox=x1+(tamx>>1);
	centroy=y1+(tamy>>1);
	texto=texto_boton;
	pulsado=0;activo=0;
	permiso_local=0;
	permiso_inicial=1;
	activacion=0;
	ktext=ktextobject;
	existeicono=0;
	dibujar_suelto();
}

boton::boton(unsigned posx,unsigned posy,char *texto_boton,
			krpfont *ktextobject,void far *icono_in)
{
	x1=posx;
	x2=posx+100;
	y1=posy;
	y2=posy+60;
	centrox=x1+(50);
	centroy=y1+(50);
	texto=texto_boton;
	pulsado=0;activo=0;
	permiso_local=0;
	permiso_inicial=1;
	activacion=0;
	ktext=ktextobject;
	icono=icono_in;
	if (icono!=NULL)
		existeicono=1;
	else existeicono=0;
	dibujar_suelto();
}

void boton::actualizar(void)
{
	dibujar_suelto();
}

int boton::activado(void)
{
	static int x,y,button;
/*	if (actualizar)
	{
		actualizar--;
		dibujar_suelto();
	}*/
	leeraton(&x,&y,&button);
	if (button==1)
	{
		procesar_raton_pulsado(x,y);
	}
	else
	{
		procesar_raton_suelto(x,y);
	}
	if (activacion)
	{
		activacion=0;
		return(1);
	}
	else
		return(0);
}

void boton::procesar_raton_pulsado(unsigned x,unsigned y)
{
	if (!permiso_inicial) return;
	if (sobre(x,y))
	{
		if (!permiso_local)
		{
			if (boton::permiso_global)
			{
				boton::permiso_global=0;
				permiso_local=1;
			}
			else return;
		}
		if(!pulsado)
		{
			dibujar_pulsado();
			pulsado=1;
			activo=1;
		}
	}
	else
	{
		if(activo)
		{
			if(pulsado)
			{
				dibujar_suelto();
				pulsado=0;
			}
		}
		else
		{
			permiso_inicial=0;
		}
	}
}

void boton::procesar_raton_suelto(unsigned x,unsigned y)
{
	permiso_inicial=1;
	if (boton::permiso_local)
	{
		boton::permiso_global=1;
		permiso_local=0;
	}
	if(activo)
	{
		activo=0;
		dibujar_suelto();
		pulsado=0;
		if (sobre(x,y))
		{
			activacion=1;
		}
	}
}

void boton::dibujar_suelto(void)
{
	oculta_cursor();
	{
		int x,y;
		setcolor(BLACK);
		rectangle(x1-1,y1-1,x2+1,y2+1);
		setfillstyle(SOLID_FILL,LIGHTGRAY);
		bar(x1,y1,x2,y2);
		setcolor(DARKGRAY);
		line(x1,y2,x2,y2);
		line(x2,y1,x2,y2);
		setcolor(WHITE);
		line(x1,y1,x2,y1);
		line(x1,y1,x1,y2);
	}
	if(existeicono)
		putimage(x1+34,y1+4,icono,0);
	ktext->kprintf(texto,centrox-(ktext->longtexto(texto)>>1),centroy-7,0);
	muestra_cursor();
}

void boton::dibujar_pulsado(void)
{
	oculta_cursor();
	{
		int x,y;
		setcolor(BLACK);
		rectangle(x1-1,y1-1,x2+1,y2+1);
		setfillstyle(SOLID_FILL,LIGHTGRAY);
		bar(x1,y1,x2,y2);
		setcolor(DARKGRAY);
		line(x1,y1,x2,y1);
		line(x1,y1,x1,y2);
	}
	if(existeicono)
		putimage(x1+35,y1+5,icono,0);
	ktext->kprintf(texto,centrox-(ktext->longtexto(texto)>>1)+1,centroy-6,0);
	muestra_cursor();
}

unsigned char boton::sobre(unsigned x,unsigned y)
{
	if ((x>x1)&&(x<x2)&&(y>y1)&&(y<y2))
		return (1);
	else
		return(0);
}


#include <graphics.h>
#include <dos.h>
#include <_null.h>
#include "krpfont.h"
#include "boton.h"
#include "retrace.h"
#include "sbvperr.h"
#include "mouse.h"
#include "listbox.h"

listbox::listbox(int argc,char far *argv[],int ancho,int x,int y,
	int elem_a_mostrar,krpfont *krptextobject)
{
	up=down=NULL;
	ult_op=-1;
	texto=argv;
	alto_de_fila=16;
	x1=x;y1=y;
	x2=x+ancho;
	y2=y+1+(elem_a_mostrar*alto_de_fila);
	n_filas=elem_a_mostrar;
	elementos_mostrados=n_filas;
	elementos_totales=argc;
	if (elementos_totales<elementos_mostrados)
	{
		elementos_mostrados=elementos_totales;
		scroll_permitido=0;
	}
	else scroll_permitido=1;
	prim_elem_mostrado=0;
	k=krptextobject;
	oculta_cursor();
	dibujar_marco();
	dibujar();
	init_botones();
	mostrar_texto();
	muestra_cursor();
}

listbox::listbox(char *argv[],int ancho,int x,int y,krpfont *krptextobject)
{
	up=down=NULL;
	elementos_totales=elementos_mostrados=1;
	ult_op=-1;
	texto=argv;
	alto_de_fila=16;
	x1=x;y1=y;
	x2=x+ancho;
	y2=y+1+alto_de_fila;
	n_filas=1;
	scroll_permitido=0;
	prim_elem_mostrado=0;
	k=krptextobject;
	oculta_cursor();
	dibujar_marco();
	dibujar();
	if (argv!=NULL)
		mostrar_texto();
	muestra_cursor();
}

void listbox::actualizar(int argc,char *argv[])
{
	texto=argv;
	elementos_totales=argc;
	elementos_mostrados=n_filas;
	if (elementos_totales<elementos_mostrados)
	{
		elementos_mostrados=elementos_totales;
		scroll_permitido=0;
	}
	else scroll_permitido=1;
	prim_elem_mostrado=0;
	oculta_cursor();
	dibujar();
	mostrar_texto();
	muestra_cursor();
}

void listbox::dibujar_marco(void)
{
	setcolor(BLACK);
	moveto(x1,y1);
	lineto(x2,y1);
	lineto(x2,y2);
	lineto(x1,y2);
	lineto(x1,y1);
}

void listbox::dibujar(void)
{
	setfillstyle(SOLID_FILL,WHITE);
	bar(x1+1,y1+1,x2-1,y2-1);
}

void listbox::mostrar_texto(void)
{
	int posicion;
	int y_centr;
	unsigned oldmaxx=k->getmaxx();
	y_centr=(alto_de_fila-10)>>1;
	y_centr-=2;
	k->setmaxx(x2-5);	//para que el texto no se salga de la caja
	for (posicion=0;posicion<elementos_mostrados;posicion++)
	{
		k->kprintf(texto[posicion+prim_elem_mostrado],x1+5,
			y1+y_centr+(alto_de_fila*posicion),BLACK);
	}
	k->setmaxx(oldmaxx);	//reponemos el valor inicial
}

int listbox::scrollup(void)
{
	if (!scroll_permitido)
		return(SCROLL_NOT_ALLOWED);
	if (prim_elem_mostrado>0)
		prim_elem_mostrado--;
	else
		return(LISTBOX_UP_END);
	dibujar();
	mostrar_texto();
	return(OK);
}

int listbox::scrolldown(void)
{
	if (!scroll_permitido)
		return(SCROLL_NOT_ALLOWED);
	if (prim_elem_mostrado<(elementos_totales-elementos_mostrados))
		prim_elem_mostrado++;
	else
		return(LISTBOX_DOWN_END);
	dibujar();
	mostrar_texto();
	return(OK);
}

int listbox::get_elemento(unsigned *argout)
{
	int x,y,button;
	if (up->activado())
		scrollup();
	if (down->activado())
		scrolldown();
	leeraton(&x,&y,&button);
	op=(y-y1)/alto_de_fila;
	if (x<x1) op=-1;
	else if (x>x2) op=-1;
	else if (y<y1) op=-1;
	else if (y>y2) op=-1;
	if (button==1)
	{
		*argout=op+prim_elem_mostrado;
		if (op<0)
			return (LISTBOX_INVALID_OPTION);
		if (op>=elementos_totales)
			return(LISTBOX_INVALID_OPTION);
		oculta_cursor();
		dibujar();
		mostrar_texto();
		muestra_cursor();
		esperar_soltar();	//esperar a que se suelte el bot¢n del rat¢n
		return(OK);
	}
	else
	{
		if (op!=ult_op)
		{
			ult_op=op;
			wait_for_vretrace();
			oculta_cursor();
			dibujar();
			if ((op>=0)&&(op<elementos_mostrados))
				marcar(op);
			mostrar_texto();
			muestra_cursor();
		}
	}
	return(LISTBOX_INVALID_OPTION);
}

void listbox::marcar(int opcion)
{
	int ydesp,alto_marca;
	ydesp=opcion*alto_de_fila;
	alto_marca=alto_de_fila-2;
	setfillstyle(SOLID_FILL,LIGHTRED);
	bar(x1+5,y1+ydesp+2,x2-5,y1+ydesp+2+alto_marca);
}

void listbox::init_botones(void)
{
	up=new boton(x2+1,y1+1,16,16,"þ",k);
	down=new boton(x2+1,y2-17,16,16,"ÿ",k);
}

int listbox::get_AltoDeFila(void)
{
	return(alto_de_fila);
}

listbox::~listbox(void)
{
	if (up!=NULL)
		delete up;
	if (down!=NULL)
		delete down;
}

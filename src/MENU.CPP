/* Se trata de un men£ desplegable que tiene como
		par metros de entrada:
- No. de elementos del men£
- Array con los nombres de las opciones de men£
- Posici¢n del men£
- Puntero al objeto que realiza el dibujo de los caracteres
		par metros de salida:
- N£mero de opci¢n elegida.
*/

#include <graphics.h>
#include <alloc.h>
#include "mouse.h"
#include "krpfont.h"
#include "sbvperr.h"
#include "retrace.h"
#include "menu.h"

unsigned menu::calcular_long_max(int argc,char *argv[])
{
	int contador;
	unsigned act_long,max_long=0;
	for (contador=1;contador<=argc;contador++)
	{
		act_long=k->longtexto(argv[contador-1]);
		if (act_long>max_long)
		{
			max_long=act_long;
		}
	}
	return (max_long);
}

void menu::ajustar_posicion(unsigned ancho,unsigned alto)
{
	const int max_x=638,max_y=478,min_y=2;
	if ((x+ancho)>max_x)
		x=max_x-ancho;
	if (y<240)
	{
		if ((y+alto)>max_y)
			y=max_y-alto;
	}
	else
	{
		y-=alto;
		if ((y<min_y))
			y=min_y;
	}
}

void menu::dibujar_recuadro(unsigned dx,unsigned dy)
{
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(x,y,x+dx,y+dy);
	setcolor(BLACK);
	moveto(x,y);
	linerel(dx,0);
	linerel(0,dy);
	linerel(-dx,0);
	linerel(0,-dy);
	setcolor(DARKGRAY);
	moveto(x+dx-1,y+dy-1);
	linerel(0,2-dy);
	moveto(x+dx-1,y+dy-1);
	linerel(2-dx,0);
	setcolor(WHITE);
	moveto(x+1,y+1);
	linerel(dx-2,0);
	moveto(x+1,y+1);
	linerel(0,dy-2);
}

int menu::opcion_sobre(unsigned ancho,int *opcion)
{
	int lectx,lecty,boton;
	leeraton(&lectx,&lecty,&boton);
	lectx-=x;
	lecty-=y;
	if ((lectx>0)&&(lectx<ancho))
	{
		*opcion=lecty/altura_opcion;
	}
	else *opcion=-1;
	if (boton==1)
		return(0);
	else
		return(1);
}

void menu::print_text(int argc,char *argv[],unsigned ancho)
{
	int cont;
	for (cont=0;cont<argc;cont++)
		k->kprintf(argv[cont],x+8,y+2+(altura_opcion*cont),BLACK);
	setcolor(DARKGRAY);
	setlinestyle(DOTTED_LINE,0,NORM_WIDTH);
	for (cont=1;cont<argc;cont++)
	{
		moveto(x+8,y+(altura_opcion*cont));
		linerel(ancho-16,0);
	}
	setlinestyle(SOLID_LINE,0,NORM_WIDTH);
}

void menu::marcar(int opcion,unsigned ancho)
{
	int y1,y2;
	if (opcion<0)
		return;
	y1=2;
	y2=altura_opcion-2;
	setfillstyle(SOLID_FILL,LIGHTRED);
	bar(x+7,y+y1+(altura_opcion*opcion),
		x+ancho-7,y+y2+(altura_opcion*opcion));
}

menu::menu(int argc,char *argv[],unsigned x_in,unsigned y_in,
	krpfont *krptextobject,int *argout)
{
	unsigned ancho,alto;	//el ancho real es ancho+1
	int op,ult_op;
	unsigned w_size;	//tama¤o en bytes de la ventana del menu;

	x=x_in;y=y_in;
	altura_opcion=20;
	ult_op=-1;
	k=krptextobject;
	ancho=calcular_long_max(argc,argv);
	ancho+=16;
	alto=(altura_opcion*argc)+2;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		zona=NULL;
		return;
	}
	ajustar_posicion(ancho,alto);
	esperar_soltar();	//esperar a que se suelte el bot¢n del rat¢n
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_recuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	muestra_cursor();
	while (opcion_sobre(ancho,&op))
	{
		if (op>=argc)
			op=-1;
		if(op!=ult_op)
		{
			ult_op=op;
			oculta_cursor();
			wait_for_vretrace();
			dibujar_recuadro(ancho-1,alto-1);
			marcar(op,ancho);
			print_text(argc,argv,ancho);
			muestra_cursor();
		}
	}
	*argout=op;
	esperar_soltar();
	if ((*argout>=argc)||(*argout<0))
	{
		last_error=(MENU_INVALID_OPTION);
		return;
	}
	last_error=OK;
}

menu::~menu()
{
	oculta_cursor();
	putimage(x,y,zona,0);
	muestra_cursor();
	if (zona!=NULL)
		farfree(zona);
}
//#include <conio.h>
#include <_null.h>
#include <string.h>
#include "sbvperr.h"
#include "krpfont.h"
#include "mouse.h"
#include "boton.h"
#include "listbox.h"
#include "querybox.h"

querybox::querybox(int xIn,int yIn,int dxIn,int nmaxch,char *respuesta,
					krpfont *krp)
{
	x=xIn;
	y=yIn;
	dx=dxIn;
	texto=NULL;
	lb=NULL;
	l_text=nmaxch;
	texto=new char[l_text+2];
	if (texto==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	lb=new listbox(&respuesta,dx,x,y,krp);
	if (lb==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	lb->actualizar(1,&respuesta);
	dy=lb->get_AltoDeFila();
	textout=respuesta;
	last_error=OK;
}

int querybox::start(void)
{
	unsigned char ch=0;
	int indice=0;
	texto[0]='_';
	texto[1]=0;
	oculta_cursor();
	lb->actualizar(1,&texto);
	while (ch!=13)
	{
		ch=TeclaPulsada();
		if ((ch>=32)&&(indice<l_text))
		{
			texto[indice++]=ch;
			texto[indice++]='_';
			texto[indice--]=0;
			lb->actualizar(1,&texto);
		}
		else
		{
			if (ch==8)
			{
				if (indice>0)
				{
					indice--;
					texto[indice++]='_';
					texto[indice--]=0;
					lb->actualizar(1,&texto);
				}
			}
		}
	}
	texto[indice]=0;
	lb->actualizar(1,&texto);
	strcpy(textout,texto);
	muestra_cursor();
	return(OK);
}

void querybox::actualizar(char *textoshow)
{
	oculta_cursor();
	lb->actualizar(1,&textoshow);
	strcpy(textout,textoshow);
	muestra_cursor();
}

int querybox::sobre(void)
{
	int cx,cy,boton;
	leeraton(&cx,&cy,&boton);
	if (boton&1)
	if ((cx>x)&&(cx<(x+dx))&&(cy>y)&&(cy<(y+dy)))
		return 1;
	return 0;
}

querybox::~querybox(void)
{
	if (texto!=NULL)
		delete texto;
	if (lb!=NULL)
		delete lb;
}


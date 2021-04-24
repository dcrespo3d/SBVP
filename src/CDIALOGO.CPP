#include <graphics.h>
#include <alloc.h>
//#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "krpfont.h"
#include "sbvperr.h"
#include "mouse.h"
#include "boton.h"
#include "filedlg.h"
//#include "listbox.h"
//#include "xms_buf.h"
//#include "imagen.h"
#include "querybox.h"
#include "filtros.h"
#include "cdialogo.h"


void cdialogo::dibujar_cuadro(unsigned dx,unsigned dy)
{
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(x,y,x+dx,y+dy);
	setfillstyle(SOLID_FILL,RED);
	bar(x+2,y+2,x+dx-2,y+altura_linea-2);
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

unsigned cdialogo::calcular_long_max(int argc,char *argv[])
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


void cdialogo::ajustar_posicion(unsigned ancho,unsigned alto)
{
	const int max_x=640,max_y=480;
	if ((x+ancho)>max_x)
		x=max_x-ancho;
	if ((y+alto)>max_y)
		y=max_y-alto;
}

void cdialogo::print_text(int argc,char *argv[],unsigned ancho)
{
	int cont;
	k->kprintf(argv[0],x+((ancho-k->longtexto(argv[0]))/2),y+2,WHITE);
	for (cont=1;cont<argc;cont++)
		k->kprintf(argv[cont],x+((ancho-k->longtexto(argv[cont]))/2),
			y+2+(altura_linea*cont),BLACK);
}

void cdialogo::centrar(unsigned ancho,unsigned alto)
{
	x=(640-ancho)>>1;
	y=(480-alto)>>1;
}

cdialogo::cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
	krpfont *krptextobject)
{
	unsigned ancho_opcion;
	unsigned w_size;		//tama¤o en bytes de la zona
	unsigned ancho,alto;	//de la zona a preservar

	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	ancho=calcular_long_max(argc,argv);
	ancho+=32;
	alto=(altura_linea*(argc+2))+2;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	muestra_cursor();
	boton aceptar(x-25+(ancho/2),y+10+(altura_linea*argc),
		50,20,"Aceptar",k);
	LiberarTecla();
	esperar_soltar();
	while(!aceptar.activado())
	{
		if (TeclaPulsada()==13) break;
	}
}

cdialogo::cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
	krpfont *krptextobject,int *respuesta)
{
	unsigned ancho_opcion;
	char ch;				//car cter auxiliar
	unsigned ancho,alto;	//zona a preservar

	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	ancho=calcular_long_max(argc,argv);
	ancho+=32;
	alto=(altura_linea*(argc+2))+2;
	if ((zona=(void far *)farcalloc(ancho+1,alto+2))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	muestra_cursor();
	*respuesta=-1;
	boton aceptar(x-65+(ancho/2),y+10+(altura_linea*argc),
		50,20,"Aceptar",k);
	boton cancelar(x+15+(ancho/2),y+10+(altura_linea*argc),
		50,20,"Cancelar",k);
	LiberarTecla();
	esperar_soltar();
	while(*respuesta==-1)
	{
		ch=TeclaPulsada();
		if (aceptar.activado())
			*respuesta=1;
		if (cancelar.activado())
			*respuesta=0;
		if (ch==13)
			*respuesta=1;
		if (ch==27)
			*respuesta=0;
	}
}

cdialogo::cdialogo(unsigned x_in,unsigned y_in,krpfont *krptextobject)
{
	char *argv[]={"Trabajando...","Espere, por favor"};
	int argc=2;
	unsigned ancho_opcion;
	unsigned w_size;		//tama¤o en bytes de la zona
	unsigned ancho,alto;	//de la zona a preservar

	cursor_reloj();
	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	ancho=calcular_long_max(argc,argv);
	ancho+=32;
	alto=(altura_linea*(argc))+2;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		cursor_flecha();
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	muestra_cursor();
	LiberarTecla();
}

cdialogo::cdialogo(unsigned x_in,unsigned y_in,
	krpfont *krptextobject,char *respuesta,char *extension)
{
	unsigned ancho_opcion;
	unsigned w_size;		//tama¤o en bytes de la zona
	unsigned ancho,alto;	//de la zona a preservar
	querybox *query=NULL;
	boton *chname=NULL;
	boton *chdir=NULL;
	boton *aceptar=NULL;
	boton *cancelar=NULL;

	int argc=2;
	char *argv[]={"Guardar archivo","Nombre del archivo (sin extensi¢n): "};
	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	ancho=200;
	alto=120;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	strcat(respuesta,extension);
	query=new querybox(x-75+(ancho/2),y+40,150,8,respuesta,k);
	chname=new boton(x-90+(ancho/2),y+63,90,20,"Cambiar Nombre",k);
	chdir=new boton(x+10+(ancho/2),y+63,80,20,"Cambiar Dir.",k);
	aceptar=new boton(x-60+(ancho/2),y+90,50,20,"Aceptar",k);
	cancelar=new boton(x+10+(ancho/2),y+90,50,20,"Cancelar",k);
	muestra_cursor();
	esperar_soltar();
	LiberarTecla();
	while (1)
	{
		if ((cancelar->activado())||(TeclaPulsada()==27))
		{
			respuesta[0]=0;
			break;
		}
		if ((aceptar->activado())||(TeclaPulsada()==13))
		{
			if (existe(respuesta))
			{
				cdialogo *confirm=NULL;
				int bResp=1;
				char *texto[]={	"Confirmar Sobreescritura de Archivo",
								"El fichero 12345678.123 existe.    ",
								"¨Sobreescribirlo?"};
				sprintf(texto[1],"El fichero %s existe.",respuesta);
				confirm=new cdialogo(3,texto,0,0,k,&bResp);
				if (confirm!=NULL) delete confirm;
				if (bResp) break;
			}
			else break;
		}
		if (chname->activado())
		{
			query->start();
			strupr(respuesta);
			strcat(respuesta,extension);
			query->actualizar(respuesta);
		}
		if (chdir->activado())
		{
			filedialog *fd=NULL;
			char filename[80]=".";
			fd=new filedialog("Cambiar directorio",filename,k);
			if (fd!=NULL)
				delete fd;
		}
	}
	if (query!=NULL) delete query;
	if (chdir!=NULL) delete chdir;
	if (chname!=NULL) delete chname;
	if (aceptar!=NULL) delete aceptar;
	if (cancelar!=NULL) delete cancelar;
	last_error=OK;
}

cdialogo::cdialogo(unsigned x_in,unsigned y_in,krpfont *krptextobject,
		char cAnchoMaskIn,mascara *pMask)
{
	unsigned ancho_opcion;
	unsigned w_size;		//tama¤o en bytes de la zona
	unsigned ancho,alto;	//de la zona a preservar
	char a,b;				//indices
	char xSep,ySep;			//separaci¢n x e y de los cuadros
	char xBorde,yBorde;		//ancho del borde (x e y)
	char cAnchoCuadro,cAltoCuadro;
	char cAnchoMask=cAnchoMaskIn;
	char dx,dy;
	char ncoef,tcoef;		//contador de coefs, coefs totales.
	char respuesta[5]="    ";
	int offset=0;
	int factor=0;
	char OffsetAuto=1;
	char qOffsetText[13];	//"Offset:Auto";
	char FactorAuto=1;
	char qFactorText[13];	//"Escala:Auto";
	char AuxText[13];
	querybox *query[49];
	querybox *qFactor=NULL,*qOffset=NULL;
	boton *aceptar=NULL;
	boton *cancelar=NULL;

	esperar_soltar();		//esperar a que se suelte el boton del raton
	strcpy(qOffsetText,"Offset:Auto");
	strcpy(qFactorText,"Factor:Auto");
	zona=NULL;
	pMask->ancho=cAnchoMask;
	cAltoCuadro=20;
	switch(cAnchoMask)
	{
		case 3:
			xSep=15;	ySep=15;
			xBorde=15;	yBorde=15;
			cAnchoCuadro=30;
			break;
		case 5:
			xSep=10;	ySep=10;
			xBorde=10;	yBorde=10;
			cAnchoCuadro=30;
			break;
		case 7:
			xSep=5;		ySep=5;
			xBorde=5;	yBorde=5;
			cAnchoCuadro=30;
			break;
		default:
			return;
	}
	int argc=2;
	char *argv[]={"Filtro de usuario","Introducir coeficientes:"};
	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	dx=cAnchoCuadro+xSep;
	dy=cAltoCuadro+ySep;
	ancho=cAnchoMask*(dx)+2*xBorde-xSep;
	alto=cAnchoMask*(dy)+2*yBorde-ySep+100;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	qFactor=new querybox(x+(ancho/2)-70,y+alto-60,65,4,qFactorText,k);
	qOffset=new querybox(x+(ancho/2)+5,y+alto-60,65,4,qOffsetText,k);
	aceptar=new boton(x+(ancho/2)-60,y+alto-30,50,20,"Aceptar",k);
	cancelar=new boton(x+(ancho/2)+10,y+alto-30,50,20,"Cancelar",k);
	if ((aceptar==NULL)||(cancelar==NULL))
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	tcoef=cAnchoMask*cAnchoMask;
	for (ncoef=0;ncoef<tcoef;ncoef++)
		pMask->coef[ncoef]=0;
	pMask->coef[tcoef>>1]=1;
	for (b=0;b<cAnchoMask;b++)
	{
		for (a=0;a<cAnchoMask;a++)
		{
			ncoef=a+(cAnchoMask*b);
			sprintf(respuesta,"%3d",pMask->coef[ncoef]);
			query[ncoef]=new querybox(x+xBorde+(a*dx),
				y+yBorde+(b*dy)+40,cAnchoCuadro,4,respuesta,k);
			if (query[ncoef]==NULL)
			{
				last_error=MEM_ALLOC_ERROR;
				return;
			}

		}
	}

	muestra_cursor();
	LiberarTecla();
	while (1)
	{
		for (ncoef=0;ncoef<tcoef;ncoef++)
		{
			if (query[ncoef]->sobre())
			{
				int entrada;
				query[ncoef]->start();
				entrada=atoi(respuesta);
				if (entrada<-128) entrada=0;
				if (entrada>127) entrada=0;
				pMask->coef[ncoef]=entrada;
				sprintf(respuesta,"%3d",pMask->coef[ncoef]);
				query[ncoef]->actualizar(respuesta);
				LiberarTecla();
			}
		}
		if (qFactor->sobre())
		{
			int entrada;
			qFactor->start();
			entrada=atoi(qFactorText);
			if (entrada<-999) entrada=0;
			if (entrada>999) entrada=0;
			pMask->factor=entrada;
			sprintf(qFactorText,"Escala:%3d",entrada);
			qFactor->actualizar(qFactorText);
			FactorAuto=0;
			LiberarTecla();
		}
		if (qOffset->sobre())
		{
			int entrada;
			qOffset->start();
			entrada=atoi(qOffsetText);
			if (entrada<-999) entrada=0;
			if (entrada>999) entrada=0;
			pMask->offset=entrada;
			sprintf(qOffsetText,"Offset:%3d",entrada);
			qOffset->actualizar(qOffsetText);
			OffsetAuto=0;
			LiberarTecla();
		}
		if (aceptar->activado())
		{
			last_error=OK;
			break;
		}
		if (cancelar->activado())
		{
			last_error=CANCELLED_OPERATION;
			break;
		}
	}
	for (ncoef=0;ncoef<tcoef;ncoef++)
	{
		factor+=pMask->coef[ncoef];
		delete query[ncoef];
	}
	if (factor==0)
	{
		factor=1;
		offset=128;
	}
	if (FactorAuto)
		pMask->factor=factor;
	if (OffsetAuto)
		pMask->offset=offset;
	if (aceptar!=NULL) delete aceptar;
	if (cancelar!=NULL) delete cancelar;
	if (qFactor!=NULL) delete qFactor;
	if (qOffset!=NULL) delete qOffset;
}

cdialogo::cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
					krpfont *krptextobject,long *respuesta)
{
	unsigned ancho_opcion;
	unsigned w_size;		//tama¤o en bytes de la zona
	unsigned ancho,alto;	//de la zona a preservar

	querybox *query=NULL;
	boton *aceptar=NULL;
	char psRespuesta[12];

	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	ancho=calcular_long_max(argc,argv);
	ancho+=32;
	alto=(altura_linea*(argc+2));
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	sprintf(psRespuesta,"%ld",*respuesta);
	query=new querybox(x+(ancho/2)-80,y+alto-30,65,10,psRespuesta,k);
	aceptar=new boton(x+(ancho/2)+15,y+alto-30,50,20,"Aceptar",k);
	if ((query==NULL)||(aceptar==NULL))
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	LiberarTecla();
	muestra_cursor();
	while (!aceptar->activado())
	{
		if (TeclaPulsada()==13) break;
		if (query->sobre())
		{
			long interm;
			query->start();
			interm=atol(psRespuesta);
			if (interm>0)
				*respuesta=interm;
			sprintf(psRespuesta,"%ld",*respuesta);
			query->actualizar(psRespuesta);
			LiberarTecla();
		}
	}
	if (aceptar!=NULL)
		delete aceptar;
	if (query!=NULL)
		delete query;
	LiberarTecla();
}

cdialogo::cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
					krpfont *krptextobject,char *respuesta)
{
	unsigned ancho_opcion;
	unsigned w_size;		//tama¤o en bytes de la zona
	unsigned ancho,alto;	//de la zona a preservar

	querybox *query=NULL;
	boton *aceptar=NULL;
	char psRespuesta[12];

	x=x_in;y=y_in;
	altura_linea=20;
	k=krptextobject;
	ancho=calcular_long_max(argc,argv);
	ancho+=32;
	alto=(altura_linea*(argc+2));
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if(!(x||y))	//si x e y valen 0
		centrar(ancho,alto);
	ajustar_posicion(ancho,alto);
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro(ancho-1,alto-1);
	print_text(argc,argv,ancho);
	sprintf(psRespuesta,"%s",respuesta);
	query=new querybox(x+(ancho/2)-80,y+alto-30,65,10,psRespuesta,k);
	aceptar=new boton(x+(ancho/2)+15,y+alto-30,50,20,"Aceptar",k);
	if ((query==NULL)||(aceptar==NULL))
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	LiberarTecla();
	muestra_cursor();
	while (!aceptar->activado())
	{
		if (TeclaPulsada()==13) break;
		if (query->sobre())
		{
			long interm;
			query->start();
			strcpy(respuesta,strupr(psRespuesta));
			sprintf(psRespuesta,"%s",respuesta);
			query->actualizar(psRespuesta);
			LiberarTecla();
		}
	}
	if (aceptar!=NULL)
		delete aceptar;
	if (query!=NULL)
		delete query;
	LiberarTecla();
}

int cdialogo::existe(char *nombre_de_fichero)
{
	FILE *pFile;
	pFile=fopen(nombre_de_fichero,"rb");
	if (pFile==NULL)
		return (0);
	fclose (pFile);
	return (1);
}

cdialogo::~cdialogo()
{
	if (zona!=NULL)
	{
		oculta_cursor();
		putimage(x,y,zona,0);
		cursor_flecha();
		muestra_cursor();
		farfree(zona);
		last_error=OK;
	}
}

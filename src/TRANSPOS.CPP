#include <_null.h>
#include <mem.h>
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"
#include "transpos.h"

#define BUFSIZE 32000

void buftranspose(unsigned char *bin,unsigned char *bout,int ancho,int alto)
{
	unsigned x,y;
	for (y=0;y<alto;y++)
	{
		for (x=0;x<ancho;x++)
		{
			bout[y+alto*x]=bin[x+ancho*y];
		}
	}
}

int transpose(imagen *origen, imagen *(&destino))
{
	unsigned long x,y,ancho,alto,bpp,NLineas,NLineasALeer,NLineasLeidas=0;
	unsigned char *buf1=NULL,*buf2=NULL,*auxbuf;
	imagen::regpal paleta[256];

	ancho=origen->getxsize();
	alto=origen->getysize();
	bpp=origen->getbitsperpixel();
	destino=new imagen (alto,ancho,bpp);
	if (destino==NULL)
		return (MEM_ALLOC_ERROR);
	buf1=new unsigned char[BUFSIZE];
	buf2=new unsigned char[BUFSIZE];
	if ((buf1==NULL)||(buf2==NULL))
		return (MEM_ALLOC_ERROR);
	if (destino->last_error!=OK)
		return (destino->last_error);
	origen->leer_paleta(paleta);
	destino->escribir_paleta(paleta);
	NLineas=BUFSIZE/ancho;
	NLineasALeer=NLineas;
	while (NLineasLeidas<alto)
	{
		NLineasALeer=alto-NLineasLeidas;
		if (NLineasALeer>NLineas)
			NLineasALeer=NLineas;
		auxbuf=buf1;
		for (y=0;y<NLineasALeer;y++)
		{
			origen->leer(auxbuf,(y+NLineasLeidas)*ancho,ancho);
			auxbuf+=ancho;
		}
		buftranspose(buf1,buf2,ancho,NLineas);
		auxbuf=buf2;
		for (y=0;y<ancho;y++)
		{
			destino->escribir(auxbuf,NLineasLeidas+alto*y,NLineasALeer);
			auxbuf+=NLineas;
		}
		NLineasLeidas+=NLineas;
	}
	delete buf2;
	delete buf1;
	return (OK);
}

/*
	int x,y,ancho,alto,bpp,nlineas;
	unsigned char *buf1=NULL,*buf2=NULL;
	imagen::regpal paleta[256];

	ancho=origen->getxsize();
	alto=origen->getysize();
	bpp=origen->getbitsperpixel();
	buf1=new unsigned char[32768];
	buf2=new unsigned char[32768];
	if ((buf1==NULL)||(buf2==NULL)
		return (mem_alloc_error);
	destino=new imagen (alto,ancho,bpp);
	if (destino==NULL)
		return (MEM_ALLOC_ERROR);
	if (destino->last_error!=OK)
		return (destino->last_error);
	origen->leer_paleta(paleta);
	destino->escribir_paleta(paleta);
	for (x=0;x<ancho;x++)
		for (y=0;y<alto;y++)
		{
			origen->leer(&byte,(long)x+((long)y*ancho),1);
			destino->escribir(&byte,(long)y+((long)x*alto),1);
		}
	return (OK);

*/
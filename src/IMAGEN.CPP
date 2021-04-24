/*Fichero IMAGEN.CPP. Definiciones de los miembros de la clase "imagen"*/

#include <_null.h>
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"

imagen::imagen(unsigned x,unsigned y,unsigned char bpp)
{
	unsigned long tamano_en_bytes;
	unsigned long tamano_en_kbytes;
	tamano_en_bytes=(unsigned long)x*(unsigned long)y;
	escala_de_grises=0;
	cuerpo=NULL;
	paleta=NULL;
	switch(bpp)
	{
		case 24:
		{
			tamano_en_bytes*=3;
			break;
		}
		case 8:
		{
			paleta=new regpal [256];
			break;
		}
		default:
		{
			last_error=ILLEGAL_BITS_PER_PIXEL;
			return;
		}
	}
	xsize=x;
	ysize=y;
	bitsperpixel=bpp;
	tamano_en_kbytes=(tamano_en_bytes>>10)+10;
	cuerpo=new xmsbuffer(tamano_en_kbytes);
	if (cuerpo==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	last_error=cuerpo->last_error;
}

int imagen::escribir(unsigned char *origen,unsigned long pos,
	unsigned long nbytes)
{
	return(cuerpo->escribir(origen,pos,nbytes));
}

int imagen::leer(unsigned char *destino,unsigned long pos,
	unsigned long nbytes)
{
	return(cuerpo->leer(destino,pos,nbytes));
}

int imagen::leer_paleta(regpal *destino)
{
	int nreg;
	if ((paleta==NULL)||(destino==NULL))
		return(ILLEGAL_PALETTE_OPERATION);
	for (nreg=0;nreg<256;nreg++)
	{
		destino[nreg].r=paleta[nreg].r;
		destino[nreg].g=paleta[nreg].g;
		destino[nreg].b=paleta[nreg].b;
		destino[nreg].y=paleta[nreg].y;
	}
    return(OK);
}

int imagen::escribir_paleta(regpal *origen)
{
	int nreg;
	if ((paleta==NULL)||(origen==NULL))
		return(ILLEGAL_PALETTE_OPERATION);
	for (nreg=0;nreg<256;nreg++)
	{
		paleta[nreg].r=origen[nreg].r;
		paleta[nreg].g=origen[nreg].g;
		paleta[nreg].b=origen[nreg].b;
		paleta[nreg].y=origen[nreg].y;
	}
	return(OK);
}

int imagen::leer_registro_paleta(unsigned char nreg,unsigned char *r,
	unsigned char *g,unsigned char *b,unsigned char *y)
{
	*r=paleta[nreg].r;
	*g=paleta[nreg].g;
	*b=paleta[nreg].b;
	*y=paleta[nreg].y;
	return (OK);
}

int imagen::escribir_registro_paleta(unsigned char nreg,unsigned char *r,
	unsigned char *g,unsigned char *b,unsigned char *y)
{
	paleta[nreg].r=*r;
	paleta[nreg].g=*g;
	paleta[nreg].b=*b;
	paleta[nreg].y=*y;
	return(OK);
}

void imagen::crear_paleta_gris(void)
{
	int nreg;
	if (paleta==NULL) return;
	for (nreg=0;nreg<256;nreg++)
	{
		paleta[nreg].r=nreg;
		paleta[nreg].g=nreg;
		paleta[nreg].b=nreg;
		paleta[nreg].y=nreg;
	}
}

void imagen::calcular_paleta_gris(void)
{
	int nreg;
	int interm;	//valor intermedio largo para aumentar la precisi¢n
	if (paleta==NULL) return;
	for (nreg=0;nreg<256;nreg++)
	{
		interm=(int)30*paleta[nreg].r;
		interm+=(int)59*paleta[nreg].g;
		interm+=(int)11*paleta[nreg].b;
		interm/=100;
		paleta[nreg].y=interm;
	}
}

void imagen::TestGrayScale(void)
{
	int nreg;
	int interm;	//valor intermedio largo para aumentar la precisi¢n
	escala_de_grises=0;
	if (paleta==NULL) return;
	for (nreg=0;nreg<256;nreg++)
	{
		if (paleta[nreg].r!=nreg) return;
		if (paleta[nreg].g!=nreg) return;
		if (paleta[nreg].b!=nreg) return;
	}
	escala_de_grises=1;
}

unsigned imagen::getxsize(void)
{
	return(xsize);
}

unsigned imagen::getysize(void)
{
	return(ysize);
}

unsigned char imagen::getbitsperpixel(void)
{
	return(bitsperpixel);
}

imagen::~imagen()
{
	if (cuerpo!=NULL)
		delete cuerpo;
	if (paleta!=NULL)
		delete paleta;
}
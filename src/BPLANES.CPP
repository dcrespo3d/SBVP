#include <_null.h>
#include "imagen.h"
#include "bplanes.h"
#include "sbvperr.h"

int GetBitPlane(imagen *img_in,imagen *(&img_out),int nPlane)
{
	unsigned char *Linea,*LineaAux;
	int ancho,alto,bpp;
	int error;
	int x,y;
	unsigned char Interm,AndValue=1<<nPlane;
	ancho=img_in->getxsize();
	alto=img_in->getysize();
	bpp=img_in->getbitsperpixel();
	if ((bpp!=8)&&(img_in->escala_de_grises))
		return(ILLEGAL_IMAGE_CONVERSION);
	Linea=new unsigned char[ancho];
	if (Linea==NULL)
		return(MEM_ALLOC_ERROR);
	img_out=new imagen(ancho,alto,bpp);
	if (img_out==NULL)
	{
		delete Linea;
		return(MEM_ALLOC_ERROR);
	}
	if ((error=img_out->last_error)!=OK)
	{
		delete Linea;
		delete img_out;
		return error;
	}
	for (y=0;y<alto;y++)
	{
		img_in->leer(Linea,(long)y*(long)ancho,ancho);
		LineaAux=Linea;
		for (x=0;x<ancho;x++)
		{
			if (*LineaAux&AndValue)
				*LineaAux++=255;
			else
				*LineaAux++=0;
		}
		img_out->escribir(Linea,(long)y*(long)ancho,ancho);
	}
	img_out->crear_paleta_gris();
	img_out->escala_de_grises=1;
	return (OK);
}

int QuantImg(imagen *img_in,imagen *(&img_out),int nBits)
{
	unsigned char *Linea,*LineaAux;
	int ancho,alto,bpp;
	int error;
	int x,y;
	unsigned char AndValue;
	int Interm;
	ancho=img_in->getxsize();
	alto=img_in->getysize();
	bpp=img_in->getbitsperpixel();
	if ((bpp!=8)&&(img_in->escala_de_grises))
		return(ILLEGAL_IMAGE_CONVERSION);
	Linea=new unsigned char[ancho];
	if (Linea==NULL)
		return(MEM_ALLOC_ERROR);
	img_out=new imagen(ancho,alto,bpp);
	if (img_out==NULL)
	{
		delete Linea;
		return(MEM_ALLOC_ERROR);
	}
	if ((error=img_out->last_error)!=OK)
	{
		delete Linea;
		delete img_out;
		return error;
	}
	AndValue=0xFF;
	AndValue<<=8-nBits;

	for (y=0;y<alto;y++)
	{
		img_in->leer(Linea,(long)y*(long)ancho,ancho);
		LineaAux=Linea;
		for (x=0;x<ancho;x++)
		{
			Interm=*LineaAux&AndValue;
//			Interm*=255;
//			Interm/=AndValue;
			*LineaAux++=Interm;
		}
		img_out->escribir(Linea,(long)y*(long)ancho,ancho);
	}
	img_out->crear_paleta_gris();
	img_out->escala_de_grises=1;
	return (OK);
}

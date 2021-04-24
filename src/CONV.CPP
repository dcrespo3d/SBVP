#include <_null.h>
#include <memory.h>
#include "sbvperr.h"
#include "xms_buf.h"
#include "imagen.h"
#include "conv.h"

int conv8_8g(imagen *origen,imagen *(&destino))
{
	int x,y,bpp;
	int nlinea,hpix;
	unsigned char interm;	//valor intermedio;
	unsigned char *linea=NULL;
	imagen::regpal *convpal;
	x=origen->getxsize();
	y=origen->getysize();
	bpp=origen->getbitsperpixel();
	if (bpp!=8)
		return (ILLEGAL_IMAGE_CONVERSION);
	if (origen->escala_de_grises)
		return (ILLEGAL_IMAGE_CONVERSION);
	destino=new imagen(x,y,bpp);
	if (destino==NULL)
		return (MEM_ALLOC_ERROR);
	if (destino->last_error!=OK)
		return (destino->last_error);
	linea=new unsigned char[x];
	if (linea==NULL)
	{
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	convpal=new imagen::regpal[256];
	if (convpal==NULL)
	{
		delete linea;
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	origen->leer_paleta(convpal);
	calcular_paleta_gris(convpal);
	destino->escribir_paleta(convpal);
	for (nlinea=0;nlinea<y;nlinea++)
	{
		origen->leer(linea,(long)x*nlinea,x);
		for (hpix=0;hpix<x;hpix++)
		{
			interm=linea[hpix];
			linea[hpix]=convpal[interm].y;
		}
		destino->escribir(linea,(long)x*nlinea,x);
	}
	destino->escala_de_grises=1;
	delete convpal;
	delete linea;
	return(OK);
}

int conv8_24(imagen *origen,imagen *(&destino))
{
	int x,y,bpp;
	int nlinea,hpix;
	unsigned char interm;	//valor intermedio;
	unsigned char *linea_in=NULL;
	unsigned char *linea_out=NULL;
	unsigned char *l_in_aux,*l_out_aux;
	imagen::regpal *convpal;
	x=origen->getxsize();
	y=origen->getysize();
	bpp=origen->getbitsperpixel();
	if (bpp!=8)
		return (ILLEGAL_IMAGE_CONVERSION);
	destino=new imagen(x,y,24);
	if (destino==NULL)
		return (MEM_ALLOC_ERROR);
	if (destino->last_error!=OK)
		return (destino->last_error);
	linea_in=new unsigned char[x];
	linea_out=new unsigned char[3*x];
	convpal=new imagen::regpal[256];
	if (linea_in==NULL)
	{
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	if (linea_out==NULL)
	{
		delete linea_in;
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	if (convpal==NULL)
	{
		delete linea_in;
		delete linea_out;
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	origen->leer_paleta(convpal);
	for (nlinea=0;nlinea<y;nlinea++)
	{
		origen->leer(linea_in,(long)x*nlinea,x);
		l_in_aux=linea_in;
		l_out_aux=linea_out;
		for (hpix=0;hpix<x;hpix++)
		{
			interm=*(l_in_aux++);
			*(l_out_aux++)=convpal[interm].b;
			*(l_out_aux++)=convpal[interm].g;
			*(l_out_aux++)=convpal[interm].r;
		}
		destino->escribir(linea_out,(long)x*3*nlinea,3*x);
	}
	delete convpal;
	delete linea_in;
	delete linea_out;
	return(OK);
}

int conv24_8g(imagen *origen,imagen *(&destino))
{
	int x,y,bpp;
	int nlinea,hpix;
	unsigned interm;	//valor intermedio;
	unsigned char *linea_in=NULL;
	unsigned char *linea_out=NULL;
	unsigned char *l_in_aux,*l_out_aux;
	x=origen->getxsize();
	y=origen->getysize();
	bpp=origen->getbitsperpixel();
	if (bpp!=24)
		return (ILLEGAL_IMAGE_CONVERSION);
	destino=new imagen(x,y,8);
	if (destino==NULL)
		return (MEM_ALLOC_ERROR);
	if (destino->last_error!=OK)
		return (destino->last_error);
	linea_in=new unsigned char[3*x];
	linea_out=new unsigned char[x];
	if (linea_in==NULL)
	{
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	if (linea_out==NULL)
	{
		delete linea_in;
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	destino->crear_paleta_gris();
	for (nlinea=0;nlinea<y;nlinea++)
	{
		origen->leer(linea_in,(long)x*3*nlinea,3*x);
		l_in_aux=linea_in;
		l_out_aux=linea_out;
		for (hpix=0;hpix<x;hpix++)
		{
			interm=0;
			interm+=11*(*(l_in_aux++));
			interm+=59*(*(l_in_aux++));
			interm+=30*(*(l_in_aux++));
			interm/=100;
			*(l_out_aux++)=interm;
		}
		destino->escribir(linea_out,(long)x*nlinea,x);
	}
	destino->escala_de_grises=1;
	delete linea_in;
	delete linea_out;
	return(OK);
}

int conv24_8(imagen *origen,imagen *(&destino))
{
	int x,y,bpp;
	int nlinea,hpix;
	unsigned interm,intermr,intermg,intermb;	//valores intermedios;
	unsigned char *linea_in=NULL;
	unsigned char *linea_out=NULL;
	unsigned char *l_in_aux,*l_out_aux;
	imagen::regpal *convpal;

	x=origen->getxsize();
	y=origen->getysize();
	bpp=origen->getbitsperpixel();
	if (bpp!=24)
		return (ILLEGAL_IMAGE_CONVERSION);
	destino=new imagen(x,y,8);
	if ((destino==NULL)||(convpal==NULL))
		return (MEM_ALLOC_ERROR);
	if (destino->last_error!=OK)
		return (destino->last_error);
	linea_in=new unsigned char[3*x];
	if (linea_in==NULL)
	{
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	linea_out=new unsigned char[x];
	if (linea_out==NULL)
	{
		delete linea_in;
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	convpal=new imagen::regpal[256];
	if (convpal==NULL)
	{
		delete linea_out;
		delete linea_in;
		delete destino;
		destino=NULL;
		return (MEM_ALLOC_ERROR);
	}
	origen->leer_paleta(convpal);
	calcular_paleta_rgb8(convpal);
	destino->escribir_paleta(convpal);
	for (nlinea=0;nlinea<y;nlinea++)
	{
		origen->leer(linea_in,(long)x*3*nlinea,3*x);
		l_in_aux=linea_in;
		l_out_aux=linea_out;
		for (hpix=0;hpix<x;hpix++)
		{
			interm=0;
			intermb=*(l_in_aux++);
			intermg=*(l_in_aux++);
			intermr=*(l_in_aux++);
			intermb>>=6;
			intermb&=3;
			intermg>>=3;
			intermg&=28;
			intermr>>=0;
			intermr&=224;
			interm=intermr+intermg+intermb;
			*(l_out_aux++)=interm;
		}
		destino->escribir(linea_out,(long)x*nlinea,x);
	}
	destino->escala_de_grises=0;
	delete linea_in;
	delete linea_out;
	delete convpal;
	return(OK);
}

// conversi¢n de imagen en escala de grises a 1 bit de color
// utilizando el algoritmo de difusi¢n de error de Floyd-Steinberg
// programado por David Crespo el 20/08/2020 en Carmena, Toledo
// 23 a¤os y medio despu‚s del proyecto original
int conv8g_1fs(imagen *img_in,imagen *(&img_out))
{
	unsigned char *Linea1=NULL, *Linea2=NULL;
	int ancho,alto,bpp;
	int error;
	int x,y;
	ancho=img_in->getxsize();
	alto=img_in->getysize();
	bpp=img_in->getbitsperpixel();
	if ((bpp!=8)&&(img_in->escala_de_grises))
		return(ILLEGAL_IMAGE_CONVERSION);
	Linea1=new unsigned char[ancho];
	if (Linea1==NULL)
		return(MEM_ALLOC_ERROR);
	Linea2=new unsigned char[ancho];
	if (Linea2==NULL) {
		delete Linea1;
		return(MEM_ALLOC_ERROR);
	}
	img_out=new imagen(ancho,alto,bpp);
	if (img_out==NULL)
	{
		delete Linea1;
		delete Linea2;
		return(MEM_ALLOC_ERROR);
	}
	if ((error=img_out->last_error)!=OK)
	{
		delete Linea1;
		delete Linea2;
		delete img_out;
		return error;
	}
#define CLAMP0255(v) (v<0?0:(v>255?255:v))
	img_in->leer(Linea2, 0, ancho);
	for (y=0;y<alto;y++)
	{
		memcpy(Linea1, Linea2, ancho);
		if (y+1<alto)
			img_in->leer(Linea2,(long)(y+1)*(long)ancho,ancho);
		for (x=0;x<ancho;x++)
		{
			int ori = Linea1[x];
			int qua = ori < 128 ? 0 : 255;
			int err = ori - qua;
			Linea1[x] = CLAMP0255(qua);
			if (x+1<ancho)
				Linea1[x+1] = CLAMP0255(Linea1[x+1] + err*7/16);
			if (y+1<alto)
			{
				if (x-1>=0)
					Linea2[x-1] = CLAMP0255(Linea2[x-1] + err*3/16);
				Linea2[x] = CLAMP0255(Linea2[x] + err*5/16);
				if (x+1<ancho)
					Linea2[x+1] = CLAMP0255(Linea2[x+1] + err*1/16);
			}
		}
		img_out->escribir(Linea1,(long)y*(long)ancho,ancho);
	}
#undef CLAMP0255
	img_out->crear_paleta_gris();
	img_out->escala_de_grises=1;
	delete Linea1;
	delete Linea2;
	return (OK);
}

void calcular_paleta_gris(imagen::regpal *paleta)
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
		paleta[nreg].r=nreg;
		paleta[nreg].g=nreg;
		paleta[nreg].b=nreg;
	}
}

void calcular_paleta_rgb8(imagen::regpal *paleta)
{
	int nreg;
	int interm;	//valor intermedio largo para aumentar la precisi¢n
	if (paleta==NULL) return;
	for (nreg=0;nreg<256;nreg++)
	{
		interm=nreg&3;			//nreg & 00000011b
		interm<<=6;
		paleta[nreg].b=interm;
		interm=nreg&28;			//nreg & 00011100b
		interm<<=3;
		paleta[nreg].g=interm;
		interm=nreg&224;			//nreg & 11100000b
		paleta[nreg].r=interm;
	}
}

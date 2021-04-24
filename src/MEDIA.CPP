#include <alloc.h>
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"
#include "media.h"

media::media(int ancho_mask_in,imagen *origen,imagen *(&destino))
{
	ancho_mask=ancho_mask_in;
	offset_mask=(ancho_mask-1)/2;
	for (cont=0;cont<ancho_mask;cont++)
		il[cont]=NULL;
	ol=il_aux=NULL;
	ancho=origen->getxsize();
	alto=origen->getysize();
	bpp=origen->getbitsperpixel();
	destino=new imagen(ancho,alto,bpp);
	if (destino==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	imgin=origen;
	imgout=destino;
	switch(bpp)
	{
		case 8:
		{
			if (!imgin->escala_de_grises)
			{
				delete destino;
				last_error=ILLEGAL_IMAGE_CONVERSION;
				return;
			}
			imgout->crear_paleta_gris();
			imgout->escala_de_grises=1;
			if ((error=media8())!=OK)
			{
				delete destino;
				last_error=error;
				return;
			}
			break;
		}
		case 24:
		{
			if ((error=media24())!=OK)
			{
				delete destino;
				last_error=error;
				return;
			}
			break;
		}
	}
	last_error=OK;
}

inline long media::limx(long num)
{
	return ((num<alto) ? num : alto-1);
}


int media::media24(void)
{
	int x,y;
	int inb,ing,inr;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char l_ancho_mask;		//ancho_mask local (eficiencia)
	char l_ancho_mask_1;    //ancho_mask-1 local (eficiencia)
	char l_offset_mask;		//offset_mask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	l_ancho=ancho*3;
	l_ancho_mask=ancho_mask;
	l_ancho_mask_1=l_ancho_mask-1;
	l_offset_mask=offset_mask;
	for (c1=0;c1<l_ancho_mask;c1++)
	{
		il[c1]=new unsigned char[l_ancho+6*l_offset_mask];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	for (c1=0;c1<l_ancho_mask;c1++)
	{
		imgin->leer(il[c1]+3*l_offset_mask,0,l_ancho);
		rellenar_bordes24(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[l_ancho_mask-1];
		for (c1=l_ancho_mask-1;c1>0;c1--)
		{
			il[c1]=il[c1-1];
		}
		il[0]=il_aux;
		imgin->leer(il[l_ancho_mask_1]+3*l_offset_mask,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes24(il[l_ancho_mask_1]);
		for (c1=0;c1<l_ancho_mask;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x+=3)
		{
			inb=ing=inr=0;
			for (c1=0;c1<l_ancho_mask;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<l_ancho_mask;c2++)
				{
					inb+=*(ila_aux++);
					ing+=*(ila_aux++);
					inr+=*(ila_aux++);
				}
				ila[c1]+=3;
			}
			*(ola++)=inb/(l_ancho_mask*l_ancho_mask);
			*(ola++)=ing/(l_ancho_mask*l_ancho_mask);
			*(ola++)=inr/(l_ancho_mask*l_ancho_mask);
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int media::media8(void)
{
	int x,y;
	int interm;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char l_ancho_mask;		//ancho_mask local (eficiencia)
	char l_ancho_mask_1;    //ancho_mask-1 local (eficiencia)
	char l_offset_mask;		//offset_mask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	l_ancho=ancho;
	l_ancho_mask=ancho_mask;
	l_ancho_mask_1=l_ancho_mask-1;
	l_offset_mask=offset_mask;
	for (c1=0;c1<l_ancho_mask;c1++)
	{
		il[c1]=new unsigned char[l_ancho+2*l_offset_mask];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	for (c1=0;c1<l_ancho_mask;c1++)
	{
		imgin->leer(il[c1]+l_offset_mask,0,l_ancho);
		rellenar_bordes8(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[l_ancho_mask-1];
		for (c1=l_ancho_mask-1;c1>0;c1--)
		{
			il[c1]=il[c1-1];
		}
		il[0]=il_aux;
		imgin->leer(il[l_ancho_mask_1]+l_offset_mask,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes8(il[l_ancho_mask_1]);
		for (c1=0;c1<l_ancho_mask;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x++)
		{
			interm=0;
			for (c1=0;c1<l_ancho_mask;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<l_ancho_mask;c2++)
				{
					interm+=*(ila_aux++);
				}
				ila[c1]++;
			}
			*(ola++)=interm/(l_ancho_mask*l_ancho_mask);
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);


}

void media::rellenar_bordes8(unsigned char *linea)
{
	int l_ancho=ancho;
	char l_offset_mask=offset_mask;		//offset_mask local;
	char c1;	//contador local
	unsigned char *li,*lf;	//punteros al inicio y final de linea
	lf=linea;	//ahora actua como puntero auxiliar;
	li=lf+l_offset_mask;
	for (c1=0;c1<l_offset_mask;c1++)
	{
		*(lf++)=*(li);
	}
	lf+=l_ancho;
	li=lf;
	lf--;
	for (c1=0;c1<l_offset_mask;c1++)
	{
		*(li++)=*(lf);
	}
}

void media::rellenar_bordes24(unsigned char *linea)
{
	int l_ancho=3*ancho;
	char l_offset_mask=offset_mask;		//offset_mask local;
	char c1;	//contador local
	unsigned char *li,*lf;	//punteros al inicio y final de linea
	lf=linea;	//ahora actua como puntero auxiliar;
	li=lf+3*l_offset_mask;
	for (c1=0;c1<l_offset_mask;c1++)
	{
		*(lf++)=*(li++);
		*(lf++)=*(li++);
		*(lf++)=*(li);
		li-=2;
	}
	lf+=l_ancho;
	li=lf;
	for (c1=0;c1<l_offset_mask;c1++)
	{
		lf-=3;
		*(li++)=*(lf++);
		*(li++)=*(lf++);
		*(li++)=*(lf++);
	}
}

media::~media(void)
{
	for (cont=0;cont<ancho_mask;cont++)
	{
		if (il[cont]!=NULL)
			delete il[cont];
	}
	if (ol!=NULL)
		delete ol;
}

/*	if (l1!=NULL)
		delete l1;
	if (l2!=NULL)
		delete l2;
	if (l3!=NULL)
		delete l3;
	if (l!=NULL)
		delete l;*/

/*
int media::media24(void)
{
	int x,y;
//	int interm,x_aux;
	int inb,ing,inr;
	long cont,decision;
	unsigned char *l1a,*l2a,*l3a,*la;
	ancho*=3;

	l1=new unsigned char [ancho+6];
	l2=new unsigned char [ancho+6];
	l3=new unsigned char [ancho+6];
	l=new unsigned char [ancho+6];
	if ((l==NULL)||(l1==NULL)||(l2==NULL)||(l3==NULL))
		return (MEM_ALLOC_ERROR);
	imgin->leer(l3+3,0,ancho+6);
	imgin->leer(l1+3,0,ancho+6);
	rellenar_bordes24(l3);
	rellenar_bordes24(l1);
	for (y=0;y<alto;y++)
	{
		l_aux=l3;
		l3=l2;
		l2=l1;
		l1=l_aux;
		imgin->leer(l3+3,(limx(y+1))*ancho,ancho+6);
		rellenar_bordes24(l3);
		l1a=l1;l2a=l2;l3a=l3;la=l+3;
		for (x=0;x<ancho;x+=3)
		{
			inb=ing=inr=0;
			inb+=*(l1a++);
			ing+=*(l1a++);
			inr+=*(l1a++);
			inb+=*(l1a++);
			ing+=*(l1a++);
			inr+=*(l1a++);
			inb+=*(l1a++);
			ing+=*(l1a++);
			inr+=*(l1a++);
			l1a-=6;
			inb+=*(l2a++);
			ing+=*(l2a++);
			inr+=*(l2a++);
			inb+=*(l2a++);
			ing+=*(l2a++);
			inr+=*(l2a++);
			inb+=*(l2a++);
			ing+=*(l2a++);
			inr+=*(l2a++);
			l2a-=6;
			inb+=*(l3a++);
			ing+=*(l3a++);
			inr+=*(l3a++);
			inb+=*(l3a++);
			ing+=*(l3a++);
			inr+=*(l3a++);
			inb+=*(l3a++);
			ing+=*(l3a++);
			inr+=*(l3a++);
			l3a-=6;
			*(la++)=inb/9;
			*(la++)=ing/9;
			*(la++)=inr/9;
		}
		imgout->escribir(l+3,limx(y)*ancho,ancho);
	}
	return(OK);
}

*/
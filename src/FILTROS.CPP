#include <stdlib.h>
#include <mem.h>
#include <_null.h>
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"
#include "filtros.h"

filtro::filtro(int nAnchoMaskIn,imagen *origen,imagen *(&destino))
{
	BorrarDestino=1;
	imgout=NULL;
	nAnchoMask=nAnchoMaskIn;
	nOffsetMask=(nAnchoMask-1)/2;
	for (cont=0;cont<nAnchoMask;cont++)
		il[cont]=NULL;
	ol=il_aux=t_mn=t_mnr=t_mng=t_mnb=NULL;
	ancho=origen->getxsize();
	alto=origen->getysize();
	bpp=origen->getbitsperpixel();
	destino=new imagen(ancho,alto,bpp);
	if (destino==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if (destino->last_error!=OK)
	{
		last_error=destino->last_error;
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
				last_error=ILLEGAL_IMAGE_CONVERSION;
				return;
			}
			imgout->crear_paleta_gris();
			imgout->escala_de_grises=1;
			break;
		}
		case 24:
		{
			break;
		}
		default:
		{
			last_error=ILLEGAL_BITS_PER_PIXEL;
			return;
		}
	}
	last_error=OK;
}


int filtro::media(void)
{
	int error;
	switch(bpp)
	{
		case 8:
			if ((error=media8())==OK)
				break;
			else
				return (error);
		case 24:
			if ((error=media24())==OK)
				break;
			else
				return (error);
		default:
			return (ILLEGAL_BITS_PER_PIXEL);
	}
	BorrarDestino=0;
	return (OK);
}

int filtro::mediana(void)
{
	int error;
	switch(bpp)
	{
		case 8:
			if ((error=mediana8())==OK)
				break;
			else
				return (error);
		case 24:
			if ((error=mediana24())==OK)
				break;
			else
				return (error);
		default:
			return (ILLEGAL_BITS_PER_PIXEL);
	}
	BorrarDestino=0;
	return (OK);
}

int filtro::moda(void)
{
	int error;
	switch(bpp)
	{
		case 8:
			if ((error=moda8())==OK)
				break;
			else
				return (error);
		case 24:
			if ((error=moda24())==OK)
				break;
			else
				return (error);
		default:
			return (ILLEGAL_BITS_PER_PIXEL);
	}
	BorrarDestino=0;
	return (OK);
}

int filtro::usuario(mascara *pMaskIn)
{
	int error;
	int(filtro::*fusuario)(void);
	pMask=pMaskIn;
	if (pMask->ancho!=nAnchoMask)
		return (USER_MASK_SIZE_MISMATCH);
	switch(bpp)
	{
		case 8:
			fusuario=&filtro::usuario8;
			break;
		case 24:
			fusuario=&filtro::usuario24;
			break;
		default:
			return (ILLEGAL_BITS_PER_PIXEL);
	}
	if ((error=(this->*fusuario)())==OK)
	{
		BorrarDestino=0;
		return (OK);
	}
	else
		return (error);
}

inline long filtro::limx(long num)
{
	return ((num<alto) ? num : alto-1);
}


int filtro::media24(void)
{
	int x,y;
	int inb,ing,inr;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	l_ancho=ancho*3;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+6*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+3*cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes24(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+3*cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes24(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x+=3)
		{
			inb=ing=inr=0;
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					inb+=*(ila_aux++);
					ing+=*(ila_aux++);
					inr+=*(ila_aux++);
				}
				ila[c1]+=3;
			}
			*(ola++)=inb/(cAnchoMaskLocal*cAnchoMaskLocal);
			*(ola++)=ing/(cAnchoMaskLocal*cAnchoMaskLocal);
			*(ola++)=inr/(cAnchoMaskLocal*cAnchoMaskLocal);
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int filtro::media8(void)
{
	int x,y;
	int interm;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	l_ancho=ancho;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+2*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes8(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes8(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x++)
		{
			interm=0;
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					interm+=*(ila_aux++);
				}
				ila[c1]++;
			}
			*(ola++)=interm/(cAnchoMaskLocal*cAnchoMaskLocal);
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);


}

void filtro::rellenar_bordes8(unsigned char *linea)
{
	int l_ancho=ancho;
	char cOffsetMaskLocal=nOffsetMask;		//nOffsetMask local;
	char c1;	//contador local
	unsigned char *li,*lf;	//punteros al inicio y final de linea
	lf=linea;	//ahora actua como puntero auxiliar;
	li=lf+cOffsetMaskLocal;
	for (c1=0;c1<cOffsetMaskLocal;c1++)
	{
		*(lf++)=*(li);
	}
	lf+=l_ancho;
	li=lf;
	lf--;
	for (c1=0;c1<cOffsetMaskLocal;c1++)
	{
		*(li++)=*(lf);
	}
}

void filtro::rellenar_bordes24(unsigned char *linea)
{
	int l_ancho=3*ancho;
	char cOffsetMaskLocal=nOffsetMask;		//nOffsetMask local;
	char c1;	//contador local
	unsigned char *li,*lf;	//punteros al inicio y final de linea
	lf=linea;	//ahora actua como puntero auxiliar;
	li=lf+3*cOffsetMaskLocal;
	for (c1=0;c1<cOffsetMaskLocal;c1++)
	{
		*(lf++)=*(li++);
		*(lf++)=*(li++);
		*(lf++)=*(li);
		li-=2;
	}
	lf+=l_ancho;
	li=lf;
	for (c1=0;c1<cOffsetMaskLocal;c1++)
	{
		lf-=3;
		*(li++)=*(lf++);
		*(li++)=*(lf++);
		*(li++)=*(lf++);
	}
}

filtro::~filtro(void)
{
	for (cont=0;cont<nAnchoMask;cont++)
	{
		if (il[cont]!=NULL)
			delete il[cont];
	}
	if (ol!=NULL)
		delete ol;
	if (t_mn!=NULL)
		delete t_mn;
	if (t_mnr!=NULL)
		delete t_mn;
	if (t_mng!=NULL)
		delete t_mn;
	if (t_mnb!=NULL)
		delete t_mn;
	if (BorrarDestino)
	{
		if(imgout!=NULL)
		{
			delete imgout;
			imgout=NULL;
		}
	}
}

int filtro::mediana8(void)
{
	int x,y;
	int interm;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	unsigned char *t_mn00;		//punteros auxiliares para mediana
	unsigned long *lt_mn00;
	int cont,cont_comp,val_comp;
	l_ancho=ancho;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	val_comp=(cAnchoMaskLocal*cAnchoMaskLocal)>>1;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+2*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	t_mn=new unsigned char[256];
	if(t_mn==NULL) return(MEM_ALLOC_ERROR);
	t_mn00=t_mn;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes8(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes8(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x++)
		{
			memset(t_mn,0,256);
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					t_mn[*(ila_aux++)]++;
				}
				ila[c1]++;
			}
			{
				cont_comp=0;
				t_mn00=t_mn;
				for (cont=0;cont<256;cont++)
				{
					cont_comp += *(t_mn00++);
					if (cont_comp>=val_comp)
						break;
				}
				*(ola++)=cont;
			}
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int filtro::mediana24(void)
{
	int x,y;
	int inb,ing,inr;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	unsigned char *t_mn00;		//punteros auxiliares para mediana
	unsigned long *lt_mn00;
	int cont,cont_comp,val_comp;
	l_ancho=ancho*3;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	val_comp=(cAnchoMaskLocal*cAnchoMaskLocal)>>1;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+6*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	t_mnr=new unsigned char[256];
	t_mng=new unsigned char[256];
	t_mnb=new unsigned char[256];
	if((t_mnr==NULL)||(t_mng==NULL)||(t_mnb==NULL))
		return(MEM_ALLOC_ERROR);
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+3*cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes24(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+3*cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes24(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x+=3)
		{
			memset(t_mnr,0,256);
			memset(t_mng,0,256);
			memset(t_mnb,0,256);
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					t_mnb[*(ila_aux++)]++;
					t_mng[*(ila_aux++)]++;
					t_mnr[*(ila_aux++)]++;
				}
				ila[c1]+=3;
			}
			{
				cont_comp=0;
				t_mn00=t_mnb;
				for (cont=0;cont<256;cont++)
				{
					cont_comp += *(t_mn00++);
					if (cont_comp>=val_comp)
						break;
				}
				*(ola++)=cont;

				cont_comp=0;
				t_mn00=t_mng;
				for (cont=0;cont<256;cont++)
				{
					cont_comp += *(t_mn00++);
					if (cont_comp>=val_comp)
						break;
				}
				*(ola++)=cont;

				cont_comp=0;
				t_mn00=t_mnr;
				for (cont=0;cont<256;cont++)
				{
					cont_comp += *(t_mn00++);
					if (cont_comp>=val_comp)
						break;
				}
				*(ola++)=cont;
			}
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int filtro::moda8(void)
{
	int x,y;
	int interm;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	unsigned char *t_mn00;		//punteros auxiliares para mediana
	unsigned long *lt_mn00;
	int cont,cont_comp;
	l_ancho=ancho;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+2*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	t_mn=new unsigned char[256];
	if(t_mn==NULL) return(MEM_ALLOC_ERROR);
	t_mn00=t_mn;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes8(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes8(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x++)
		{
			memset(t_mn,0,256);
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					t_mn[*(ila_aux++)]++;
				}
				ila[c1]++;
			}
			{
				int elem;
				int max=0;
				int moda=0;
				long tempmoda=0;
				int num=0;
				t_mn00=t_mn;
				for (cont=0;cont<256;cont++)
				{
					elem=*t_mn00++;
					if (elem>max)
						max=elem;
				}
				t_mn00=t_mn;
				for (cont=0;cont<256;cont++)
				{
					elem=*t_mn00++;
					if(elem==max)
					{
						tempmoda+=cont;
						num++;
					}
				}
				moda=tempmoda/num;
				*(ola++)=moda;
			}
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int filtro::moda24(void)
{
	int x,y;
	int inb,ing,inr;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	unsigned char *t_mn00;		//punteros auxiliares para mediana
	unsigned long *lt_mn00;
	int cont,cont_comp;
	l_ancho=ancho*3;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+6*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	t_mnr=new unsigned char[256];
	t_mng=new unsigned char[256];
	t_mnb=new unsigned char[256];
	if((t_mnr==NULL)||(t_mng==NULL)||(t_mnb==NULL))
		return(MEM_ALLOC_ERROR);
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+3*cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes24(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+3*cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes24(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x+=3)
		{
			memset(t_mnr,0,256);
			memset(t_mng,0,256);
			memset(t_mnb,0,256);
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					t_mnb[*(ila_aux++)]++;
					t_mng[*(ila_aux++)]++;
					t_mnr[*(ila_aux++)]++;
				}
				ila[c1]+=3;
			}
			{
				int elem;
				int maxr=0,maxg=0,maxb=0;
				int modar=0,modag=0,modab=0;
				long tempmodar=0,tempmodag=0,tempmodab=0;
				int numr=0,numg=0,numb=0;
				for (cont=0;cont<256;cont++)
				{
					elem=t_mnr[cont];
					if (elem>maxr)
						maxr=elem;
					elem=t_mng[cont];
					if (elem>maxg)
						maxg=elem;
					elem=t_mnb[cont];
					if (elem>maxb)
						maxb=elem;
				}
				for (cont=0;cont<256;cont++)
				{
					elem=t_mnr[cont];
					if(elem==maxr)
					{
						tempmodar+=cont;
						numr++;
					}
					elem=t_mng[cont];
					if(elem==maxg)
					{
						tempmodag+=cont;
						numg++;
					}
					elem=t_mnb[cont];
					if(elem==maxb)
					{
						tempmodab+=cont;
						numb++;
					}
				}
				modab=tempmodab/numb;
				*(ola++)=modab;
				modag=tempmodag/numg;
				*(ola++)=modag;
				modar=tempmodar/numr;
				*(ola++)=modar;
			}
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int filtro::usuario24(void)
{
	int x,y;
	long lInb,lIng,lInr;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	mascara *pMaskLocal=pMask;
	int nFactor=(pMask->factor);
	int nOffset=pMask->offset;
	int *pCoefMask;	//puntero local a los coefs.de la mascara

	if (nFactor==0)
		return (DIVIDE_BY_ZERO);
	l_ancho=ancho*3;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+6*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+3*cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes24(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+3*cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes24(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x+=3)
		{
			lInb=lIng=lInr=0;
			pCoefMask=pMaskLocal->coef;
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					lInb+=((int)*(ila_aux++))*(*(pCoefMask));
					lIng+=((int)*(ila_aux++))*(*(pCoefMask));
					lInr+=((int)*(ila_aux++))*(*(pCoefMask++));
				}
				ila[c1]+=3;
			}
			lInb/=nFactor;
			lIng/=nFactor;
			lInr/=nFactor;
			lInb+=nOffset;
			lIng+=nOffset;
			lInr+=nOffset;
			if (lInb<0) lInb=0;
			if (lInb>255) lInb=255;
			if (lIng<0) lIng=0;
			if (lIng>255) lIng=255;
			if (lInr<0) lInr=0;
			if (lInr>255) lInr=255;
			*(ola++)=lInb;
			*(ola++)=lIng;
			*(ola++)=lInr;
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

int filtro::usuario8(void)
{
	int x,y;
	long lInterm;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	mascara *pMaskLocal=pMask;
	int nFactor=(pMask->factor);
	int nOffset=pMask->offset;
	int *pCoefMask;	//puntero local a los coefs.de la mascara

	if (nFactor==0)
		return (DIVIDE_BY_ZERO);
	l_ancho=ancho;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+2*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	if(ol==NULL) return(MEM_ALLOC_ERROR);
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes8(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[0];
		for (c1=0;c1<cAnchoMaskLocal_1;c1++)
		{
			il[c1]=il[c1+1];
		}
		il[cAnchoMaskLocal_1]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes8(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x++)
		{
			lInterm=0;
			pCoefMask=pMaskLocal->coef;
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					lInterm+=((int)*(ila_aux++))*(*(pCoefMask++));
				}
				ila[c1]++;
			}
			lInterm/=nFactor;
			lInterm+=nOffset;
			if (lInterm<0) lInterm=0;
			if (lInterm>255) lInterm=255;
			*(ola++)=lInterm;
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

/*void filtro::ref_mascara(mascara *pMaskIn)
{
	pMask=pMaskIn;
}*/

/*
int filtro::usuario8(void)
{
	int x,y;
	long lInterm;
	char c1,c2;		//contadores locales para mayor eficiencia

	int l_ancho;            //ancho local para mayor eficiencia
	char cAnchoMaskLocal;		//cAnchoMask local (eficiencia)
	char cAnchoMaskLocal_1;    //cAnchoMask-1 local (eficiencia)
	char cOffsetMaskLocal;		//nOffsetMask local (eficiencia)
	unsigned char *ila[7];	//lineas de entrada locales
	unsigned char *ola,*ila_aux;		//lineas de entrada locales
	mascara *pMaskLocal=pMask;
	unsigned wFactor=(pMask->factor);
	char cOffset=pMask->offset;
	int *pCoefMask;	//puntero local a los coefs.de la mascara

	l_ancho=ancho;
	cAnchoMaskLocal=nAnchoMask;
	cAnchoMaskLocal_1=cAnchoMaskLocal-1;
	cOffsetMaskLocal=nOffsetMask;
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		il[c1]=new unsigned char[l_ancho+2*cOffsetMaskLocal];
		if (il[c1]==NULL)
			return(MEM_ALLOC_ERROR);
	}
	ol=new unsigned char[l_ancho];
	for (c1=0;c1<cAnchoMaskLocal;c1++)
	{
		imgin->leer(il[c1]+cOffsetMaskLocal,0,l_ancho);
		rellenar_bordes8(il[c1]);
	}
	for (y=0;y<alto;y++)
	{
		il_aux=il[cAnchoMaskLocal_1];
		for (c1=cAnchoMaskLocal_1;c1>0;c1--)
		{
			il[c1]=il[c1-1];
		}
		il[0]=il_aux;
		imgin->leer(il[cAnchoMaskLocal_1]+cOffsetMaskLocal,
			(limx(y+1))*l_ancho,l_ancho);
		rellenar_bordes8(il[cAnchoMaskLocal_1]);
		for (c1=0;c1<cAnchoMaskLocal;c1++)
			ila[c1]=il[c1];
		ola=ol;
		for (x=0;x<l_ancho;x++)
		{
			lInterm=0;
			pCoefMask=pMaskLocal->coef;
			for (c1=0;c1<cAnchoMaskLocal;c1++)
			{
				ila_aux=ila[c1];
				for (c2=0;c2<cAnchoMaskLocal;c2++)
				{
					lInterm+=((int)*(ila_aux++))*(*(pCoefMask++));
				}
				ila[c1]++;
			}
			lInterm/=wFactor;
			lInterm+=cOffset;
			if (lInterm<0) lInterm=0;
			if (lInterm>255) lInterm=255;
			*(ola++)=lInterm;
		}
		imgout->escribir(ol,limx(y)*l_ancho,l_ancho);
	}
	return(OK);
}

*/

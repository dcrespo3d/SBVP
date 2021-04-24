#include <_null.h>
#include <math.h>
#include <mem.h>
#include "complejo.h"
#include "fftclass.h"
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"
#include "fft2d.h"

float comp_factor=1000;		//factor de compensaci¢n
						//en el c lculo del logaritmo
						//del m¢dulo de la FFT

int transpose(xmsbuffer *origen, xmsbuffer *destino,int AnchoIn,int AltoIn);

int centrar(xmsbuffer *origen, xmsbuffer *destino,int AnchoIn,int AltoIn);

fft2d::fft2d(imagen *imgin,imagen *(&imgout))
{
	long MemAReservar;

	BorrarDestino=1;
	destino=NULL;
	cuerpo1=NULL;
	cuerpo2=NULL;
	fft1d_X=NULL;
	fft1d_Y=NULL;
	CompLinea=NULL;
	CompLineaAux=NULL;
	UCLinea=NULL;
	imgin->TestGrayScale();
	if ((!imgin->escala_de_grises)||(imgin->getbitsperpixel()!=8))
	{
		last_error=NOT_8BITS_GRAYSCALE;
		return;
	}
	nAncho=imgin->getxsize();
	nAlto=imgin->getysize();
	nAnchoComp=nAncho;
	nAltoComp=nAlto;
	RoundP2(nAnchoComp,Nx);
	RoundP2(nAltoComp,Ny);
	imgout=new imagen(nAnchoComp,nAltoComp,8);
	origen=imgin;
	destino=imgout;
	if (imgout==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if ((last_error=imgout->last_error)!=OK)
		return;
	MemAReservar=(long)nAnchoComp*(long)nAltoComp;
	MemAReservar*=sizeof(complejo);
	MemAReservar>>=10;
	MemAReservar+=10;
	cuerpo1=new xmsbuffer(MemAReservar);
	if (cuerpo1==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if ((last_error=cuerpo1->last_error)!=OK)
		return;
	cuerpo2=new xmsbuffer(MemAReservar);
	if(cuerpo2==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if ((last_error=cuerpo2->last_error)!=OK)
		return;
	last_error=OK;
}

void fft2d::RoundP2(int &numero,int &cont)
{
	cont=0;
	numero--;
	while (numero)
	{
		numero>>=1;
		cont++;
	}
	numero=1;
	numero<<=cont;
}

int fft2d::CalcFFT(void)
{
	int y;
	int nAnchoLinea;	//ancho de la l¡nea=max(anchoX,anchoY)
	if (nAnchoComp>nAltoComp)
		nAnchoLinea=nAnchoComp;
	else nAnchoLinea=nAltoComp;
	CompLinea=new complejo[nAnchoLinea];
	if (CompLinea==NULL)
		return (MEM_ALLOC_ERROR);
	CompLineaAux=new complejo[nAnchoLinea];
	if (CompLineaAux==NULL)
		return (MEM_ALLOC_ERROR);
	UCLinea=new unsigned char[nAnchoLinea];
	if (UCLinea==NULL)
		return (MEM_ALLOC_ERROR);
	fft1d_X=new fft(Nx);
	fft1d_Y=new fft(Ny);
	if ((fft1d_X==NULL)||(fft1d_Y==NULL))
		return (MEM_ALLOC_ERROR);
	destino->crear_paleta_gris();
	destino->escala_de_grises=1;
				//Conversi¢n a nos. complejos y fft horizontal
	y=0;
	while(y<nAlto)
	{
		origen->leer(UCLinea,(long)y*nAncho,nAncho);
		LineaUCharToComplejo(nAncho,nAnchoComp);
		fft1d_X->calc_fft(CompLinea,CompLineaAux);
		cuerpo1->escribir((unsigned char *)CompLineaAux,
						(long)y*nAnchoComp*sizeof(complejo),
						nAnchoComp*sizeof(complejo));
		y++;
	}
	memset(UCLinea,0,nAncho);
	LineaUCharToComplejo(nAncho,nAnchoComp);
	fft1d_X->calc_fft(CompLinea,CompLineaAux);
	while(y<nAltoComp)
	{
		cuerpo1->escribir((unsigned char *)CompLineaAux,
						(long)y*nAnchoComp*sizeof(complejo),
						nAnchoComp*sizeof(complejo));
		y++;
	}
	transpose(cuerpo1,cuerpo2,nAnchoComp,nAltoComp);
//fft vertical
	y=0;
	while(y<nAnchoComp)
	{
		cuerpo2->leer((unsigned char *)CompLinea,
						(long)y*nAltoComp*sizeof(complejo),
						nAltoComp*sizeof(complejo));
		fft1d_Y->calc_fft(CompLinea,CompLineaAux);
		cuerpo1->escribir((unsigned char *)CompLineaAux,
						(long)y*nAltoComp*sizeof(complejo),
						nAltoComp*sizeof(complejo));
		y++;
	}
	transpose(cuerpo1,cuerpo2,nAltoComp,nAnchoComp);
//Centrar las "bajas frecuencias"
	centrar(cuerpo2,cuerpo1,nAnchoComp,nAltoComp);
//Conversi¢n a enteros, c lculo del m¢dulo de la fft
	y=0;
	while(y<nAltoComp)
	{
		cuerpo1->leer((unsigned char *)CompLinea,
						(long)y*nAnchoComp*sizeof(complejo),
						nAnchoComp*sizeof(complejo));
		LineaComplejoToUChar(nAnchoComp);
		destino->escribir(UCLinea,(long)y*nAnchoComp,nAnchoComp);
		y++;
	}
	BorrarDestino=0;
	return(OK);
}

float fft2d::Modulo(complejo CompNum)
{
	CompNum.re*=CompNum.re;
	CompNum.im*=CompNum.im;
	return sqrt(CompNum.re+CompNum.im);
}

void fft2d::LineaUCharToComplejo(int nAnchoUCIn,int nAnchoCompIn)
{
	complejo CompAux;
	complejo *pCompAux=CompLinea;
	unsigned char *pUCAux=UCLinea;

	if (nAnchoUCIn>nAnchoCompIn)
		return;
	int cont=0;
	CompAux.im=0;
	while (cont<nAnchoUCIn)
	{
		CompAux.re=*pUCAux++;
		*pCompAux++=CompAux;
		cont++;
	}
	CompAux.re=0;
	CompAux.im=0;
	while(cont<nAnchoCompIn)
	{
		*pCompAux++=CompAux;
		cont++;
	}
}

void fft2d::LineaComplejoToUChar(int nAnchoCompIn)
{
	complejo CompAux;
	float FloatAux,FloatAux2;
	complejo *pCompAux=CompLinea;
	unsigned char *pUCAux=UCLinea;

	int cont=0;
	CompAux.im=0;
	while (cont<nAnchoCompIn)
	{
		CompAux=*pCompAux++;
		FloatAux=Modulo(CompAux);
		FloatAux2=log10(1+FloatAux);
		FloatAux=FloatAux2*comp_factor;
		if (FloatAux>255) FloatAux=255;
//		if (FloatAux<0) FloatAux=0;
		*pUCAux++=FloatAux;
		cont++;
	}
}

fft2d::~fft2d()
{
	if (fft1d_Y!=NULL)
		delete fft1d_Y;
	if (fft1d_X!=NULL)
		delete fft1d_X;
	if (UCLinea!=NULL)
		delete UCLinea;
	if (CompLineaAux!=NULL)
		delete CompLineaAux;
	if (CompLinea!=NULL)
		delete CompLinea;
	if (cuerpo2!=NULL)
	{
//		cuerpo2->libera();
		delete cuerpo2;
	}
	if (cuerpo1!=NULL)
	{
//		cuerpo1->libera();
		delete cuerpo1;
	}
	if (BorrarDestino)
		if (destino!=NULL)
		{
			delete destino;
			destino=NULL;
		}
}

void buftranspose(complejo *bin,complejo *bout,int ancho,int alto)
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

int transpose(xmsbuffer *origen, xmsbuffer *destino,int AnchoIn,int AltoIn)
{
	unsigned long x,y,ancho,alto,bpp,NLineas,NLineasALeer,NLineasLeidas=0;
	complejo *buf1=NULL,*buf2=NULL,*auxbuf;
	#define BUFSIZE 2048	//­Ojo! no. de elementos, no bytes

	ancho=AnchoIn;
	alto=AltoIn;
	buf1=new complejo[BUFSIZE];
	buf2=new complejo[BUFSIZE];
	if ((buf1==NULL)||(buf2==NULL))
		return (MEM_ALLOC_ERROR);
	NLineas=BUFSIZE;
//	NLineas*=sizeof(complejo);
	NLineas/=ancho;
	NLineasALeer=NLineas;
	while (NLineasLeidas<alto)
	{
		NLineasALeer=alto-NLineasLeidas;
		if (NLineasALeer>NLineas)
			NLineasALeer=NLineas;
		auxbuf=buf1;
		for (y=0;y<NLineasALeer;y++)
		{
			origen->leer((unsigned char *)auxbuf,
						(y+NLineasLeidas)*ancho*sizeof(complejo)
						,ancho*sizeof(complejo));
			auxbuf+=ancho;
		}
		buftranspose(buf1,buf2,ancho,NLineas);
		auxbuf=buf2;
		for (y=0;y<ancho;y++)
		{
			destino->escribir((unsigned char *)auxbuf,
							(NLineasLeidas+alto*y)*sizeof(complejo)
							,NLineasALeer*sizeof(complejo));
			auxbuf+=NLineas;
//			auxbuf+=alto;
		}
		NLineasLeidas+=NLineas;
	}
	delete buf2;
	delete buf1;
	return (OK);
}

int centrar(xmsbuffer *origen, xmsbuffer *destino,int AnchoIn,int AltoIn)
{
	complejo *MediaLinea;
	long ancho=AnchoIn;
	long alto=AltoIn;
	long x,y;
	MediaLinea=new complejo[ancho/2];
	if (MediaLinea==NULL)
		return (MEM_ALLOC_ERROR);
	for (y=0;y<alto/2;y++)
	{
		origen->leer((unsigned char *)MediaLinea,
					(y*ancho)*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		destino->escribir((unsigned char *)MediaLinea,
					((y+(alto/2))*ancho+(ancho/2))*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		origen->leer((unsigned char *)MediaLinea,
					(y*ancho+(ancho/2))*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		destino->escribir((unsigned char *)MediaLinea,
					((y+(alto/2))*ancho)*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		origen->leer((unsigned char *)MediaLinea,
					((y+(alto/2))*ancho+(ancho/2))*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		destino->escribir((unsigned char *)MediaLinea,
					(y*ancho)*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		origen->leer((unsigned char *)MediaLinea,
					((y+(alto/2))*ancho)*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
		destino->escribir((unsigned char *)MediaLinea,
					(y*ancho+(ancho/2))*sizeof(complejo),
					(ancho/2)*sizeof(complejo));
	}
	delete MediaLinea;
	return (OK);
}

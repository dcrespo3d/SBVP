/*Fichero VESA.CPP*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <dos.h>
#include <io.h>
#include <alloc.h>
#include "xms_buf.h"
#include "imagen.h"
#include "vesa.h"
#include "sbvperr.h"
#include "mouse.h"

static unsigned bank=0;
static unsigned long Granularity;
static unsigned BPScanLine;			//bytes per scan line
static unsigned window_segment=0xA000;
static unsigned centerpixels;

	struct VideoMode
	{
		unsigned xpix;
		unsigned ypix;
		unsigned num8bpp;
		unsigned num24bpp;
	}static VideoModes[5]={320,200,0x13,0x10F,
					640,480,0x101,0x112,
					800,600,0x103,0x115,
					1024,768,0x105,0x118,
					1280,1024,0x107,0x11B};


vesa_video_subsystem::vesa_video_subsystem()
{
	vib=NULL;
	mib=NULL;
	last_error=start();
}

vesa_video_subsystem::~vesa_video_subsystem()
{
	if (vib!=NULL)
		delete vib;
	if (mib!=NULL)
		delete mib;
}

int vesa_video_subsystem::start(void)
{
	vib=new vesainfoblock;
	if(vib==NULL)
		return(MEM_ALLOC_ERROR);
	mib=new modeinfoblock;
	if(mib==NULL)
		return(MEM_ALLOC_ERROR);
	getvesainfoblock(vib);
	if (memcmp(vib->vesa_signature,"VESA",4))
		return(VESA_NOT_SUPPORTED);
	if (vib->vesa_version<0x0102)
		return(VESA_VBE12_NOT_SUPP);
	if (getmode(&initial_mode)!=OK)
		initial_mode=0x0003;	//modo texto
	return(OK);
}

int vesa_video_subsystem::initmode(unsigned mode)
{
	bank=0;
	getmodeinfoblock(mode,mib);
	Granularity=(mib->wingranularity);
	Granularity<<=10;
	BPScanLine=(mib->bytesperscanline);
	window_segment=mib->winasegment;
	return setmode(mode);
}

int vesa_video_subsystem::setmode(unsigned mode)
{
	int status;
	asm{
		MOV AX,0X4F02
		MOV BX,mode
		INT 0X10
		MOV status,AX
	}
	if (status==0x004F)
		return(OK);
	else
		return(VESA_STATUS_ERROR);
}

int vesa_video_subsystem::getmode(unsigned *mode)
{
	unsigned status;
	unsigned _mode;
	asm{
		MOV AX,0x4F03
		INT 0X10
		MOV _mode,BX
		MOV status,AX
	}
	*mode=_mode;
	if (status==0x004F)
		return(OK);
	else
		return(VESA_STATUS_ERROR);
}

int vesa_video_subsystem::getvesainfoblock
	(vesainfoblock far *ib)
{
	int status;
	asm{
		MOV AX,0x4F00
		MOV DX,word ptr[ib]
		MOV DI,DX
		MOV DX,word ptr[ib+2]
		MOV ES,DX
		INT 0x10
		MOV status,AX
	}
	if (status==0x004F)
		return(OK);
	else
		return(VESA_STATUS_ERROR);
}

int vesa_video_subsystem::getmodeinfoblock
	(unsigned modo,modeinfoblock far *ib)
{
	int status;
	asm{
		MOV AX,0x4F01
		MOV CX,modo
		MOV DX,word ptr[ib]
		MOV DI,DX
		MOV DX,word ptr[ib+2]
		MOV ES,DX
		INT 0x10
		MOV status,AX
	}
	return(status);
}

void wrlinea(unsigned nlinea,unsigned char far *linea,unsigned centeroffset);

void nada(unsigned char *linea,unsigned corr_size)
{
	(void)linea;
	(void)corr_size;
}

void (*corregir)(unsigned char *linea,unsigned corr_size);

char far *vesa_video_subsystem::GetSVGABaseDir(void)
{
	if (mib->winaattributes&1)		//si ventana A soportada
		if (mib->winaattributes&4)	//si ventana A escribible
			return ((char far *)((unsigned long)mib->winasegment<<16));
	if (mib->winbattributes&1)		//si ventana B soportada
		if (mib->winbattributes&4)	//si ventana B escribible
			return ((char far *)((unsigned long)mib->winbsegment<<16));
	return (char far *)0xA0000000;
}

int vesa_video_subsystem::ShowVGA13h(imagen *img)
{
	unsigned char *Linea320;
	unsigned AnchoImg=img->getxsize();
	unsigned AltoImg=img->getysize();
	unsigned y,yoffset,xoffset;
	unsigned char far *VGAMem=(unsigned char *)0xA0000000;
	Linea320=new unsigned char[320];
	if (Linea320==NULL) return (MEM_ALLOC_ERROR);
	memset(Linea320,0,320);
	yoffset=(200-AltoImg)/2;
	xoffset=(320-AnchoImg)/2;
	ponpaleta(img);
	for (y=0;y<AltoImg;y++)
	{
		img->leer(Linea320,(long)AnchoImg*y,AnchoImg);
		memcpy(VGAMem+xoffset+((y+yoffset)*320),Linea320,320);
	}
	delete Linea320;
	return (OK);
}


void corregir_linea(unsigned char *linea,unsigned corr_size);

void vesa_video_subsystem::setbank(int Bank)
{
	asm{
		MOV AX,0x4F05
		MOV BX,0X0000
		MOV DX,Bank
		INT 0x10
	}
}

int vesa_video_subsystem::show(imagen *scrimg)
{
	unsigned mode;		//no. de modo VESA
	unsigned modex;		//resoluci¢n horizontal del modo
	unsigned modey;		//resoluci¢n vertical del modo; centrado vertical
	unsigned y;			//no. de l¡nea horizontal
	unsigned XCenterOffset;	//bytes a desplazar para el centrado horiz.
	unsigned YCenterOffset; //bytes a desplazar para el centrado vert.
	unsigned width;				//anchura de la imagen en pixels
	unsigned height;			//altura de la imagen en pixels
	unsigned bytesperimgxline;	//anchura de la imagen en bytes
	unsigned char *linea;
	unsigned char truebpp;	//truebytesperpixel;
	mode=setreqmode(scrimg->getxsize(),scrimg->getysize(),scrimg->getbitsperpixel());
	if (mode==0) return VESA_REQ_MODE_NOT_SUPPORTED;
	if (mode==0x13)
	{
		int error;
		error=ShowVGA13h(scrimg);
		if (error==OK)
		{
			int x,y,boton;
			while (!TeclaPulsada())
			{
				leeraton(&x,&y,&boton);
				if (boton) break;
			}
		}
		setmode(initial_mode);
		return error;
	}
	modex=mib->xresolution;
	modey=mib->yresolution;
	height=scrimg->getysize();
	width=scrimg->getxsize();
	XCenterOffset=modex;
	XCenterOffset-=width;
	XCenterOffset>>=1;
	YCenterOffset=modey;
	YCenterOffset-=height;
	YCenterOffset>>=1;
	linea=new unsigned char [BPScanLine];
	if (linea==NULL)
		return(MEM_ALLOC_ERROR);
	memset(linea,0,BPScanLine);
	bank=((long)YCenterOffset*BPScanLine)
		/(Granularity);
	bytesperimgxline=width;
	if (scrimg->getbitsperpixel()==24)
	{
		bytesperimgxline=width*3;
		if (mib->bitsperpixel==32)
		{
			XCenterOffset*=4;
			corregir=corregir_linea;
		}
		else
			XCenterOffset*=3;
	}
	else
	{
		ponpaleta(scrimg);
		corregir=nada;
	}
	{
		setbank(bank);
		unsigned long btcb;	//bytes hasta cambiar de banco
		char far *SVGABase=GetSVGABaseDir();
		char far *SVGAPoint;
		for (y=0;y<height;y++)
		{
			scrimg->leer(linea+XCenterOffset,(long)bytesperimgxline*y,
				bytesperimgxline);
			corregir(linea+XCenterOffset,width);
			btcb=(long)Granularity;
			btcb-=((long)(y+YCenterOffset)*BPScanLine)&(Granularity-1);
			SVGAPoint=SVGABase;
			SVGAPoint+=(int)((y+YCenterOffset)*BPScanLine)&(Granularity-1);
			if ((btcb<=BPScanLine)&&(btcb>0))
			{
				memcpy(SVGAPoint,linea,btcb);
				setbank(++bank);
				memcpy(SVGABase,linea+btcb,BPScanLine-btcb);
//				memcpy(SVGAPoint+btcb,linea+btcb,BPScanLine-btcb);
//				btcb=granular-(bytesperimgxline-btcb);
			}
			else
			{
				memcpy(SVGAPoint,linea,BPScanLine);
//				btcb-=bytesperimgxline;
			}
		}
	}
	delete linea;
	LiberarTecla();
	{
		int x,y,boton;
		while (!TeclaPulsada())
		{
			leeraton(&x,&y,&boton);
			if (boton) break;
		}
	}
	setmode(initial_mode);
	return(OK);
}

unsigned vesa_video_subsystem::setreqmode(unsigned x,unsigned y,unsigned char bpp)
{
	int cont;
	int error;
	unsigned RetMode;
	for (cont=0;cont<5;cont++)
	{
		if ((x<=VideoModes[cont].xpix)&&(y<=VideoModes[cont].ypix))
		{
			RetMode=(bpp==8)?VideoModes[cont].num8bpp:VideoModes[cont].num24bpp;
			error=initmode(RetMode);
			if (error==OK) return RetMode;
		}
	}
	return 0;
}

int vesa_video_subsystem::ponpaleta(imagen *palimg)
{
	unsigned color;
	unsigned char r,g,b,y;
	for(color=0;color<256;color++)
	{
		palimg->leer_registro_paleta(color,&r,&g,&b,&y);
		outportb(0x3C8,color);
		outportb(0x3C9,r/4);
		outportb(0x3C9,g/4);
		outportb(0x3C9,b/4);
	}
	return(OK);
}

void corregir_linea(unsigned char *linea,unsigned corr_size)
{
	int cont,cont_x_4,cont_x_3;
	unsigned char *lcx4,*lcx3;
	lcx4=linea;
	lcx3=linea;
	lcx4+=4*(corr_size-1)+3;
	lcx3+=3*(corr_size-1)+2;
	for (cont=corr_size-1;cont>0;cont--)
	{
		*(lcx4--)=0;
		*(lcx4--)=*(lcx3--);
		*(lcx4--)=*(lcx3--);
		*(lcx4--)=*(lcx3--);
	}
}

char *vesa_video_subsystem::GetVerNumber(void)
{
	char *version="XXXX";
	unsigned MajorVersion,MinorVersion;
	MajorVersion=vib->vesa_version&0xFF00;
	MajorVersion>>=8;
	MinorVersion=vib->vesa_version&0xFF;
	sprintf(version,"%1x.%1x",MajorVersion,MinorVersion);
	return version;
}

char *vesa_video_subsystem::GetVendorName(void)
{
	char *VendorName=vib->oemstringptr;
	return VendorName;
}

unsigned *vesa_video_subsystem::GetModeList(void)
{
	unsigned *ModeList=vib->videomodeptr;
	return ModeList;
}

int vesa_video_subsystem::GetModeInfo(unsigned modo,char *Lista[6])
{
	int status;
	status=getmodeinfoblock(modo,mib);
	if (status!=0x004F)
		return 0;
	sprintf(Lista[0],"Modo 0x%X",modo);
	sprintf(Lista[1],"Resoluci¢n: %d x %d",mib->xresolution,
			mib->yresolution);
	if (mib->modeattributes&16)
		sprintf(Lista[2],"(Modo de gr ficos)");
	else
		sprintf(Lista[2],"(Modo de texto)");
	sprintf(Lista[3],"Bits por pixel: %d",mib->bitsperpixel);
	sprintf(Lista[4],"Granularidad: %d Kb.",mib->wingranularity);
	sprintf(Lista[5],"Bytes por l¡nea scan: %d",mib->bytesperscanline);
	return 1;
}


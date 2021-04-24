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

static unsigned bank=0;
static unsigned granular;
static unsigned bytesperscanline;
static unsigned window_segment=0xA000;
static unsigned centerpixels;

vesa_video_subsystem::vesa_video_subsystem()
{
	last_error=start();
}

int vesa_video_subsystem::start(void)
{
	return(OK);
}

int vesa_video_subsystem::initmode(unsigned mode)
{
	setmode(mode);
	return (OK);
}

int vesa_video_subsystem::setmode(unsigned mode)
{
	asm{
		MOV AX,mode
		INT 0X10
	}
	return(OK);
}

int vesa_video_subsystem::getmode(unsigned *mode)
{
	return(OK);
}

void wrlinea(unsigned nlinea,unsigned char far *linea,unsigned centeroffset);

void (*corregir)(unsigned char *linea,unsigned corr_size);
void nada(unsigned char *linea,unsigned corr_size)
{
	(void)linea;
	(void)corr_size;
}

void corregir_linea(unsigned char *linea,unsigned corr_size);

void ponpixel(unsigned x, unsigned y, unsigned char bpp);

void ponpaletagris(void);

int vesa_video_subsystem::show(imagen *scrimg)
{
	unsigned mode;		//no. de modo VESA
	unsigned modex;		//resoluci¢n horizontal del modo
	unsigned modey;		//resoluci¢n vertical del modo; centrado vertical
	unsigned char y;			//no. de l¡nea horizontal
	unsigned x;
	unsigned centerbytes;	//bytes a desplazar para el centrado horiz.
	unsigned width;				//anchura de la imagen en pixels
	unsigned height;			//altura de la imagen en pixels
	unsigned bytesperimgxline;	//anchura de la imagen en bytes
	unsigned char *linea=NULL,*l_aux;
	unsigned char truebpp;	//truebytesperpixel;
	width=scrimg->getxsize();
	height=scrimg->getysize();
	truebpp=scrimg->getbitsperpixel();
	if (width>320) return (VESA_STATUS_ERROR);
	if (height>200) return (VESA_STATUS_ERROR);
	modex=320;
	modey=200;
	switch(truebpp)
	{
		case 8:
		{
			linea=new unsigned char[width];
			if (linea==NULL)
				return (MEM_ALLOC_ERROR);
			initmode(0x0013);
			ponpaleta(scrimg);
			for (y=0;y<height;y++)
			{
				scrimg->leer(linea,(long)width*y,width);
				l_aux=linea;
				for(x=0;x<width;x++)
				{
					ponpixel(x,y,*(l_aux++));
//					ponpixel(x,y,x+y);
				}
			}
		break;
		}
		case 24:
		{
			unsigned interm;
			linea=new unsigned char[width*3];
			if (linea==NULL)
				return (MEM_ALLOC_ERROR);
			initmode(0x0013);
			ponpaletagris();
			for (y=0;y<height;y++)
			{
				scrimg->leer(linea,(long)width*y*3,width*3);
				l_aux=linea;
				for(x=0;x<width;x++)
				{
					interm=0;
					interm+=11*(*(l_aux++));
					interm+=59*(*(l_aux++));
					interm+=30*(*(l_aux++));
					interm/=100;
					ponpixel(x,y,interm);
				}
			}
		break;
		}
	}
	getch();
	initmode(0x0003);
	if (linea!=NULL)
		delete linea;
	return(OK);
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

void ponpaletagris(void)
{
	unsigned color;
	for(color=0;color<256;color++)
	{
		outportb(0x3C8,color);
		outportb(0x3C9,color/4);
		outportb(0x3C9,color/4);
		outportb(0x3C9,color/4);
	}
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

void ponpixel(unsigned x, unsigned y, unsigned char color)
{
	asm{
		MOV AX,0xA000
		MOV ES,AX
		MOV BX,y
		XCHG BH,BL
		MOV AX,BX
		SHR BX,2
		ADD BX,AX
		MOV AX,x
		ADD BX,AX
		MOV AL,color
		MOV ES:[BX],AL
	}
}

void wrgrmem(unsigned donde,unsigned char que)
{
	asm{
		MOV AX,0xA000
		MOV ES,AX
		MOV DX,donde
		MOV DI,DX
		MOV AL,que
		MOV ES:[DI],AL
	}
}
#include <alloc.h>
#include <conio.h>
#include <mem.h>
#include "sbvperr.h"
#include "krpfont.h"
#include "mouse.h"
#include "boton.h"
#include "xms_buf.h"
#include "imagen.h"
#include "graphics.h"
#include "histogrm.h"

void reset_tecla(void)
{
	while (kbhit()) getch();
}

int get_tecla(void)
{
	if (kbhit())
		return getch();
	else return 0;
}
histogram::histogram()
{
	histo_data=new long[256];
	if (histo_data==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	last_error=OK;
}

int histogram::EqualizeHisto(imagen *img_in,imagen *(&img_out))
{
	int cont;
	int ancho,alto,bpp;
	unsigned char map[256];
	long v=0;
	img_out=NULL;
	unsigned char *map_aux=map;
	long *histo_aux=histo_data;
	switch(img_in->getbitsperpixel())
	{
		case 8:
			img_in->TestGrayScale();
			if (img_in->escala_de_grises)
				last_error=CalcHistoGray(img_in);
			else
				last_error=NOT_8BITS_GRAYSCALE;
			break;
		default:
			last_error=NOT_8BITS_GRAYSCALE;
	}
	if (last_error!=OK)
		return (last_error);
	ancho=img_in->getxsize();
	alto=img_in->getysize();
	bpp=img_in->getbitsperpixel();
	img_out=new imagen(ancho,alto,bpp);
	if (img_out==NULL)
	{
		return MEM_ALLOC_ERROR;
	}
	if (img_out->last_error!=OK)
	{
		last_error=img_out->last_error;
		delete img_out;
		return last_error;
	}
	TotalPixels=(long)ancho*(long)alto;
	int temp;
	long max=0,min=65530;
	float aux;
	for (cont=1;cont<256;cont++)
	{
		histo_data[cont]=histo_data[cont-1]+histo_data[cont];
		if (histo_data[cont]>max) max=histo_data[cont];
		if (histo_data[cont]<min) min=histo_data[cont];
	}
	for (cont=0;cont<256;cont++)
	{
		aux=(float)histo_data[cont]-min;
		aux=aux/(float)max-(float)min;
		aux=aux*255;
		map[cont]=(int)aux;
	}
/*	for (cont=0;cont<256;cont++)
	{
		v += histo_aux[cont];
		temp = (255*v)/TotalPixels;
		map_aux[cont] = temp;
		v += histo_aux[255-cont];
		temp = ((v<<8)-v)/TotalPixels;
		map_aux[255-cont] = 255-temp;
	}*/
	last_error=AdjustHistoGray(img_in,img_out,map);
	if (last_error!=OK)
		delete img_out;
	return last_error;
}

int histogram::ShowHistogram(imagen *img_in,krpfont *k_in)
{
	void far *zona;
	long w_size;
	int zoom=1;
	k=k_in;
	altura_linea=20;
	zona=NULL;
	dx=300;
	dy=350;
	w_size=imagesize(x,y,x+dx,y+dy);
	if ((zona=(void far *)farcalloc(1,w_size))==NULL)
		return MEM_ALLOC_ERROR;
	centrar();
	oculta_cursor();
	getimage(x,y,x+dx,y+dy,zona);
	dibujar_cuadro();
	print_text("Histograma");
	switch(img_in->getbitsperpixel())
	{
		case 8:
			img_in->TestGrayScale();
			if (img_in->escala_de_grises)
				last_error=CalcHistoGray(img_in);
			else
				last_error=CalcHistoPal(img_in);
			break;
		case 24:
			last_error=CalcHistoRGB(img_in);
			break;
		default:
			last_error=ILLEGAL_BITS_PER_PIXEL;
	}
	if (last_error==OK)
		ActualizarHisto(1);
	else
	{
		oculta_cursor();
		farfree(zona);
		muestra_cursor();
		return last_error;
	}
	boton aceptar(x-25+(dx/2),y+dy-30,
		50,20,"Aceptar",k);
	boton arriba(x+dx-55,y+dy-30,20,20,"þ",k);
	boton abajo(x+dx-30,y+dy-30,20,20,"ÿ",k);
	muestra_cursor();
	reset_tecla();
	esperar_soltar();
	while(!aceptar.activado())
	{
		if (get_tecla()==13) break;
		if (arriba.activado())
		{
			zoom++;
			ActualizarHisto(zoom);
		}
		if ((abajo.activado())&&(zoom>1))
		{
			zoom--;
			ActualizarHisto(zoom);
		}
	}
	oculta_cursor();
	putimage(x,y,zona,0);
	muestra_cursor();
	farfree(zona);
	return OK;
}

histogram::~histogram()
{
	if (histo_data!=NULL)
		delete histo_data;
}

void histogram::dibujar_cuadro(void)
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

void histogram::print_text(char *texto)
{
	int cont;
	k->kprintf(texto,x+((dx-k->longtexto(texto))/2),y+2,WHITE);
}

void histogram::centrar(void)
{
	x=(640-dx)>>1;
	y=(480-dy)>>1;
}

int histogram::CalcHistoGray(imagen *img)
{
	int x,y;
	unsigned char *linea,*linea_aux;
	int ancho=img->getxsize();
	int alto=img->getysize();
	long *histo_aux=histo_data;

	linea=new unsigned char [ancho];
	if (linea==NULL)
		return (MEM_ALLOC_ERROR);
	memset(histo_aux,0,256*sizeof(long));
	for (y=0;y<alto;y++)
	{
		img->leer(linea,(long)ancho*y,ancho);
		linea_aux=linea;
		for (x=0;x<ancho;x++)
		{
			(*(histo_aux+(*linea_aux++)))++;
		}
	}
	maximo=0;
	for (x=0;x<256;x++)
	{
		if (*histo_aux>maximo)
			maximo=*histo_aux;
		histo_aux++;
	}
	delete linea;
	return (OK);
}

int histogram::AdjustHistoGray(imagen *img_in,imagen *img_out,
	unsigned char *map)
{
	int x,y;
	unsigned char *linea,*linea_aux;
	unsigned char valor;
	int ancho=img_in->getxsize();
	int alto=img_in->getysize();
	int bpp=img_in->getbitsperpixel();
	if (bpp!=8)
		return NOT_8BITS_GRAYSCALE;
	img_out->escribir_paleta(img_in->paleta);
	linea=new unsigned char [ancho];
	if (linea==NULL)
		return (MEM_ALLOC_ERROR);
	for (y=0;y<alto;y++)
	{
		img_in->leer(linea,(long)ancho*y,ancho);
		linea_aux=linea;
		for (x=0;x<ancho;x++)
		{
			valor=*linea_aux;
			*linea_aux++ = *(map+valor);
		}
		img_out->escribir(linea,(long)ancho*y,ancho);
	}
	maximo=0;
	delete linea;
	return (OK);
}

int histogram::CalcHistoPal(imagen *img)
{
	imagen::regpal *paleta_local;
	int x,y;
	int luminancia;
	unsigned char *linea,*linea_aux;
	int ancho=img->getxsize();
	int alto=img->getysize();
	long *histo_aux=histo_data;

	linea=new unsigned char [ancho];
	if (linea==NULL)
		return (MEM_ALLOC_ERROR);
	paleta_local=new imagen::regpal [256];
	if (paleta_local==NULL)
		return (MEM_ALLOC_ERROR);
	img->calcular_paleta_gris();
	img->leer_paleta(paleta_local);
	memset(histo_aux,0,256*sizeof(long));
	for (y=0;y<alto;y++)
	{
		img->leer(linea,(long)ancho*y,ancho);
		linea_aux=linea;
		for (x=0;x<ancho;x++)
		{
			luminancia=paleta_local[*linea_aux++].y;
			(*(histo_aux+luminancia))++;
		}
	}
	maximo=0;
	for (x=0;x<256;x++)
	{
		if (*histo_aux>maximo)
			maximo=*histo_aux;
		histo_aux++;
	}
	delete paleta_local;
	delete linea;
	return (OK);
}

int histogram::CalcHistoRGB(imagen *img)
{
	int x,y;
	unsigned char *linea,*linea_aux;
	int ancho=img->getxsize();
	int alto=img->getysize();
	long *histo_aux=histo_data;
	unsigned interm;

	linea=new unsigned char [ancho*3];
	if (linea==NULL)
		return (MEM_ALLOC_ERROR);
	memset(histo_aux,0,256*sizeof(long));
	for (y=0;y<alto;y++)
	{
		img->leer(linea,(long)ancho*3*y,ancho*3);
		linea_aux=linea;
		for (x=0;x<ancho;x++)
		{
			interm=0;
			interm+=11*(*linea_aux++);
			interm+=59*(*linea_aux++);
			interm+=30*(*linea_aux++);
			interm/=100;
			(*(histo_aux+interm))++;
		}
	}
	maximo=0;
	for (x=0;x<256;x++)
	{
		if (*histo_aux>maximo)
			maximo=*histo_aux;
		histo_aux++;
	}
	delete linea;
	return (OK);
}

void histogram::ActualizarHisto(int zoom)
{
	int dx2=dx/2;
	int nX;
	long longitud;
	oculta_cursor();
	setcolor(BLACK);
	rectangle(x+dx2-129,y+39,x+dx2+129,y+39+258);
	setfillstyle(SOLID_FILL,WHITE);
	bar(x+dx2-128,y+40,x+dx2+128,y+40+256);
	k->kprintf("0",x+dx2-133,y+40+256,BLACK);
	k->kprintf("255",x+dx2+118,y+40+256,BLACK);
	setviewport(x+dx2-128,y+40,x+dx2+128,y+40+256,1);
	for (nX=0;nX<256;nX++)
	{
		moveto(nX,256);
		longitud=histo_data[nX];
		longitud*=256;
		longitud*=zoom;
		longitud/=maximo;
		if (longitud>255)
			longitud=255;
		linerel(0,-longitud);
	}
	setviewport(0,0,639,479,0);
	muestra_cursor();
}
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include "sbvperr.h"
#include "krpfont.h"

krpfont::krpfont()
{
	maxx=639;
	maxy=479;
	inicializar_fuente();
}

krpfont::~krpfont()
{
	liberar_fuente();
}

unsigned krpfont::getmaxx(void)
{
	return(maxx);
}

void krpfont::setmaxx(unsigned x)
{
	if (x<640) maxx=x;
}

void krpfont::kprintf(const unsigned char *texto,unsigned x_inicial,
		unsigned y_inicial,unsigned char color)
{
	unsigned x_actual,y_actual,x,y;
	unsigned offset_buffer;
	unsigned ancho_caracter,dato;
	const unsigned *direccion;

	x_actual=x_inicial;y_actual=y_inicial;
	while (*texto)
	{
		if ((x_actual>maxx)||(y_actual>maxy)||(*texto<32)) break;
		direccion=buffer_fuente;
		offset_buffer=((*texto)-32)<<4;
		direccion+=offset_buffer;
		ancho_caracter=*(direccion++);
		for(y=0;y<15;y++)
		{
			dato=*(direccion++);
			for(x=0;x<=ancho_caracter;x++)
			{
				if (dato&0x8000)
				{
					putpixel(x+x_actual,y+y_actual,color);
				}
				dato<<=1;
			}
		}
		x_actual+=ancho_caracter+1;
		texto++;
	}
}

unsigned krpfont::longtexto(const unsigned char *texto)
{
	unsigned offset_buffer;
	unsigned ancho_caracter,long_texto=0;
	const unsigned *direccion;

	while (*texto)
	{
		direccion=buffer_fuente;
		offset_buffer=((*texto)-32)<<4;
		direccion+=offset_buffer;
		ancho_caracter=*(direccion);
		long_texto+=ancho_caracter;
		long_texto++;
		texto++;
	}
	long_texto--;
	if (long_texto>maxx)
		long_texto=1;
	return(long_texto);
}

void krpfont::inicializar_fuente()
{
	FILE *infont;
	if((buffer_fuente=(unsigned *)calloc(32,224))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	if((infont=fopen("kresp32.fon","rb"))==NULL)
	{
		last_error=FONT_FILE_NOT_FOUND;
		return;
	}
	fread(buffer_fuente,32,224,infont);
	fclose(infont);
	last_error=OK;
}

void krpfont::liberar_fuente()
{
	free (buffer_fuente);
}

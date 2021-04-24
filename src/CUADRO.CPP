#include <alloc.h>
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"
#include "cuadro.h"

int cuadro(imagen *(&img))
{
	unsigned char *linea;
	long x,y,decision;
	int ancho=256,alto=256,bpp=8;
	img=new imagen(ancho,alto,bpp);
	if (img==NULL)
		return (MEM_ALLOC_ERROR);
	if (img->last_error!=OK)
		return (img->last_error);
	linea=(unsigned char *)calloc(ancho,1);
	if (linea==NULL)
		return (MEM_ALLOC_ERROR);
	img->crear_paleta_gris();
	img->escala_de_grises=1;
	for(y=0;y<alto;y++)
	{
		for(x=0;x<ancho;x++)
		{
			if ((y>120)&&(y<136)&&(x>120)&&(x<136))
				linea[x]=255;
			else
				linea[x]=0;
		}
		img->escribir(linea,y*ancho,ancho);
	}
	free(linea);
	return(OK);
}

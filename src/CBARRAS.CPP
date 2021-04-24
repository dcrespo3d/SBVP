#include <alloc.h>
#include "xms_buf.h"
#include "imagen.h"
#include "sbvperr.h"
#include "cbarras.h"

int cbarras(imagen *(&img))
{
	unsigned char *linea,*l_aux;
	long cont,decision;
	int ancho=640,alto=480,bpp=24;
	img=new imagen(ancho,alto,bpp);
	if (img==NULL)
		return (MEM_ALLOC_ERROR);
	if (img->last_error!=OK)
		return (img->last_error);
	linea=(unsigned char *)calloc(ancho,3);
	if (linea==NULL)
		return (MEM_ALLOC_ERROR);
	l_aux=linea;
	for(cont=0;cont<ancho;cont++)
	{
		decision=cont;
		decision<<=3;
		decision/=640;
		*(l_aux++)=decision&1 ? 20 : 240;
		*(l_aux++)=decision&4 ? 20 : 240;
		*(l_aux++)=decision&2 ? 20 : 240;
	}
	for(cont=0;cont<alto;cont++)
	{
		img->escribir(linea,cont*3*ancho,3*ancho);
	}
	free(linea);
	return(OK);
}

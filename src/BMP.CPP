/*Fichero BMP.CPP*/

#include <stdio.h>
#include <stdlib.h>
#include "xms_buf.h"
#include "imagen.h"
#include "bmp.h"
#include "sbvperr.h"

bmp::bmp()
{
	bmpbuffer=NULL;
	last_error=OK;
}

int bmp::leer_de_fichero(char *nombre_fichero,imagen *(&bmpimg))
{
	#define READBUFSIZE 32768
	unsigned char *read_buffer;
	unsigned bytes_leidos=READBUFSIZE;
	unsigned long bytes_leidos_totales=0;
	if ((bmpfile=fopen(nombre_fichero,"rb"))==NULL)
	{
		return(FILE_OPEN_ERROR);
	}
	read_buffer=new unsigned char [READBUFSIZE];
	if (read_buffer==NULL)
	{
		fclose(bmpfile);
		return(MEM_ALLOC_ERROR);
	}
	fread(&bmpheader,sizeof(bmpheader),1,bmpfile);
	if (bmpheader.bftype!=0x4D42)
	{
		delete read_buffer;
		fclose(bmpfile);
		return(BMP_NOT_BMP);
	}
	if (bmpheader.bicompres!=0)
	{
		delete read_buffer;
		fclose(bmpfile);
		return(BMP_RLE_NOT_SUPPORTED);
	}
	getrealdata();
	unsigned long request_size=real_size;
	request_size>>=10;		//convertimos el tama¤o de la imagen a kbytes
	request_size+=10;      //y a¤adimos 10 kb de m s al buffer
	bmpbuffer=new xmsbuffer(request_size);
	if (bmpbuffer==NULL)
	{
		delete read_buffer;
		fclose(bmpfile);
		return(MEM_ALLOC_ERROR);
	}
	if (bmpbuffer->last_error!=OK)
	{
		delete read_buffer;
		fclose(bmpfile);
		return(bmpbuffer->last_error);
	}
	fseek(bmpfile,bmpheader.bfoffbits,0);
	while (bytes_leidos==READBUFSIZE)
	{
		bytes_leidos=fread(read_buffer,1,READBUFSIZE,bmpfile);
		bmpbuffer->escribir(read_buffer,bytes_leidos_totales,bytes_leidos);
		bytes_leidos_totales+=bytes_leidos;
	}
	delete read_buffer;
	if (bytes_leidos_totales<(real_size-2))
	{
		fclose (bmpfile);
		return (BMP_FILE_ERROR);
	}
	bmpimg=new imagen(bmpheader.biwidth,bmpheader.biheight,imgbpp);
	if (bmpimg==NULL) return(MEM_ALLOC_ERROR);
	if (bmpimg->last_error!=OK) return (bmpimg->last_error);
	switch (bmpheader.bibitcount)
	{
		case 24:
		{
			last_error=decode24(bmpimg);
			break;
		}
		case 8:
		{
			last_error=decode8(bmpimg);
			break;
		}
		case 4:
		{
			last_error=decode4(bmpimg);
			break;
		}
		case 1:
		{
			last_error=decode1(bmpimg);
			break;
		}
		default:
		{
			last_error=BMP_FILE_ERROR;
			break;
		}
	}
	fclose(bmpfile);
	return(last_error);
}

int bmp::decode24(imagen *bmpimg)
{
	unsigned long bmppos;	//posici¢n en el buffer XMS del BMP
	unsigned long imgpos;	//posici¢n en el buffer XMS de la imagen
	unsigned imgy;			//no. de linea horizontal
	unsigned imghbytes; 	//no. de bytes por linea horizontal (imagen)
	unsigned bmphbytes;		//no. de bytes por linea horizontal (BMP)
	unsigned char *buf_linea;	//buffer de linea
	imghbytes=3*bmpheader.biwidth;
	bmphbytes=real_x;
	if ((buf_linea=(unsigned char *)calloc(bmphbytes,1))==NULL)
	{
		return(MEM_ALLOC_ERROR);
	}
	for (imgy=0;imgy<bmpheader.biheight;imgy++)
	{
		bmppos=(long)bmphbytes*((long)real_y-imgy-1);
		imgpos=(long)imghbytes*(long)imgy;
		bmpbuffer->leer(buf_linea,bmppos,bmphbytes);
		bmpimg->escribir(buf_linea,imgpos,imghbytes);
	}
	free(buf_linea);
	return(OK);
}

int bmp::decode8(imagen *bmpimg)
{
	unsigned long bmppos;	//posici¢n en el buffer XMS del BMP
	unsigned long imgpos;	//posici¢n en el buffer XMS de la imagen
	unsigned imgy;			//no. de linea horizontal
	unsigned imghbytes; 	//no. de bytes por linea horizontal (imagen)
	unsigned bmphbytes;		//no. de bytes por linea horizontal (BMP)
	unsigned char *buf_linea;	//buffer de linea

	paleta_ncolores=256;
	imghbytes=bmpheader.biwidth;
	bmphbytes=real_x;
	if ((buf_linea=(unsigned char *)calloc(bmphbytes,1))==NULL)
		return(MEM_ALLOC_ERROR);
	if (leer_paleta(bmpimg)!=OK)
		return (MEM_ALLOC_ERROR);
	bmpimg->TestGrayScale();
	for (imgy=0;imgy<bmpheader.biheight;imgy++)
	{
		bmppos=(long)bmphbytes*((long)real_y-imgy-1);
		imgpos=(long)imghbytes*(long)imgy;
		bmpbuffer->leer(buf_linea,bmppos,bmphbytes);
		bmpimg->escribir(buf_linea,imgpos,imghbytes);
	}

	free(buf_linea);
	return(OK);
}

int bmp::decode4(imagen *bmpimg)
{
	unsigned long bmppos;	//posici¢n en el buffer XMS del BMP
	unsigned long imgpos;	//posici¢n en el buffer XMS de la imagen
	unsigned imgy;			//no. de linea horizontal
	unsigned imghbytes; 	//no. de bytes por linea horizontal (imagen)
	unsigned bmphbytes;		//no. de bytes por linea horizontal (BMP)
	unsigned char *buf_linea_img;	//buffer de linea (BMP)
	unsigned char *buf_linea_bmp;	//buffer de linea (BMP)
	unsigned char dato;		//para convertir de 4 a 8 bits
	imghbytes=bmpheader.biwidth;
	bmphbytes=real_x;
	paleta_ncolores=16;
	if ((buf_linea_bmp=(unsigned char *)calloc(bmphbytes,1))==NULL)
		return(MEM_ALLOC_ERROR);
	if ((buf_linea_img=(unsigned char *)calloc(imghbytes,1))==NULL)
	{
		free(buf_linea_bmp);
		return(MEM_ALLOC_ERROR);
	}
	if (leer_paleta(bmpimg)!=OK)
		return (MEM_ALLOC_ERROR);

	for (imgy=0;imgy<bmpheader.biheight;imgy++)
	{
		bmppos=(long)bmphbytes*((long)real_y-imgy-1);
		imgpos=(long)imghbytes*(long)imgy;
		bmpbuffer->leer(buf_linea_bmp,bmppos,bmphbytes);
		for (int cont=0;cont<imghbytes;cont++)
		{
			dato=buf_linea_bmp[cont>>1];
			if (cont&1)
			{
				dato&=0x0F;	//nibble bajo;
				buf_linea_img[cont]=dato;
			}
			else
			{
				dato>>=4;	//nibble alto;
				buf_linea_img[cont]=dato;
			}
		}
		bmpimg->escribir(buf_linea_img,imgpos,imghbytes);
	}
	free(buf_linea_img);
	free(buf_linea_bmp);
	return(OK);
}

int bmp::decode1(imagen *bmpimg)
{
	unsigned long bmppos;	//posici¢n en el buffer XMS del BMP
	unsigned long imgpos;	//posici¢n en el buffer XMS de la imagen
	unsigned imgy;			//no. de linea horizontal
	unsigned imghbytes; 	//no. de bytes por linea horizontal (imagen)
	unsigned bmphbytes;		//no. de bytes por linea horizontal (BMP)
	unsigned char *buf_linea_img;	//buffer de linea (BMP)
	unsigned char *buf_linea_bmp;	//buffer de linea (BMP)
	unsigned char dato;		//para convertir de 4 a 8 bits
	int cont;
	imghbytes=bmpheader.biwidth;
	bmphbytes=real_x;
	paleta_ncolores=2;
	if ((buf_linea_bmp=(unsigned char *)calloc(bmphbytes,1))==NULL)
		return(MEM_ALLOC_ERROR);
	if ((buf_linea_img=(unsigned char *)calloc(real_x,8))==NULL)
	{
		free(buf_linea_bmp);
		return(MEM_ALLOC_ERROR);
	}
	if (leer_paleta(bmpimg)!=OK)
		return (MEM_ALLOC_ERROR);

	for (imgy=0;imgy<bmpheader.biheight;imgy++)
	{
		bmppos=(long)bmphbytes*((long)real_y-imgy-1);
		imgpos=(long)imghbytes*(long)imgy;
		bmpbuffer->leer(buf_linea_bmp,bmppos,bmphbytes);
		for (cont=0;cont<bmphbytes;cont++)
		{
			dato=buf_linea_bmp[cont];
			for (int bit=0;bit<8;bit++)
			{
				if (dato&0x80)
					buf_linea_img[(8*cont)+bit]=1;
				else buf_linea_img[(8*cont)+bit]=0;
				dato<<=1;
			}
		}
		bmpimg->escribir(buf_linea_img,imgpos,imghbytes);
	}
	free(buf_linea_img);
	free(buf_linea_bmp);
	return(OK);
}

void bmp::getrealdata(void)
{
	real_y=bmpheader.biheight;
	real_x=bmpheader.biwidth;
	real_x*=bmpheader.bibitcount;	//multiplicamos por el n de bits/pix
	real_x+=0x001F;			//convertimos el ancho de la imagen a un valor
	real_x&=0xFFE0;		//m£ltiplo de 32 (bits) mayor o igual que ‚l.
	real_x>>=3;		//dividimos entre 8 para obtener bytes/pixel;
	real_size=real_x*real_y;
	if (bmpheader.bibitcount==24)
	{
		imgbpp=24;
		total_size=real_size+54;
	}
	else
	{
		imgbpp=8;
		total_size=real_size+54+1024;
	}
}

int bmp::leer_paleta(imagen *bmpimg)
{
	unsigned register color;
    regpal reg;
	fseek(bmpfile,54,0);
	for (color=0;color<paleta_ncolores;color++)
	{
		reg.b=fgetc(bmpfile);
		reg.g=fgetc(bmpfile);
		reg.r=fgetc(bmpfile);
		reg.y=fgetc(bmpfile);
		bmpimg->escribir_registro_paleta(color,&(reg.r),
			&(reg.g),&(reg.b),&(reg.y));
	}
	return(OK);
}

int bmp::escribir_a_fichero(char *nombre_fichero,imagen *bmpimg)
{
	#define WRITEBUFSIZE 32768
	unsigned char *write_buffer;
	unsigned bytes_a_escribir=WRITEBUFSIZE;
	unsigned long bytes_escritos_totales=0;

	bmpheader.biwidth=bmpimg->getxsize();
	bmpheader.biheight=bmpimg->getysize();
	bmpheader.biplanes=1;
	bmpheader.bibitcount=bmpimg->getbitsperpixel();
	getrealdata();
	bmpheader.bftype=0x4D42;
	bmpheader.bfsize=total_size;
	bmpheader.bfreserved1=0;
	bmpheader.bfreserved2=0;
	bmpheader.bfoffbits=total_size-real_size;
	bmpheader.bisize=40;
	bmpheader.bicompres=0;
	bmpheader.bisizimage=real_size;
	bmpheader.bixpelsxmet=2834;
	bmpheader.biypelsxmet=2834;
	bmpheader.biclrused=0;
	bmpheader.biclrimport=0;
	unsigned long request_size=real_size;
	request_size>>=10;		//convertimos el tama¤o de la imagen a kbytes
	request_size+=10;      //y a¤adimos 10 kb de m s al buffer
	bmpbuffer=new xmsbuffer(request_size);
	if (bmpbuffer==NULL)
		return(MEM_ALLOC_ERROR);
	if (bmpbuffer->last_error!=OK)
		return(bmpbuffer->last_error);
	if ((bmpfile=fopen(nombre_fichero,"wb"))==NULL)
		return(FILE_OPEN_ERROR);
	fwrite(&bmpheader,sizeof(bmpheader),1,bmpfile);
	switch (bmpheader.bibitcount)
	{
		case 24:
		{
			last_error=encode24(bmpimg);
			break;
		}
		case 8:
		{
			last_error=encode8(bmpimg);
			break;
		}
		default:
		{
			last_error=BMP_FILE_ERROR;
			break;
		}
	}
	if (last_error!=OK)
	{
		fclose(bmpfile);
		return(last_error);
	}
	if ((write_buffer=(unsigned char *)calloc(WRITEBUFSIZE,1))==NULL)
	{
		fclose(bmpfile);
		return(MEM_ALLOC_ERROR);
	}
	fseek(bmpfile,bmpheader.bfoffbits,0);
	while (bytes_escritos_totales<real_size)
	{
		if ((real_size-bytes_escritos_totales)<WRITEBUFSIZE)
			bytes_a_escribir=real_size-bytes_escritos_totales;
		else bytes_a_escribir=WRITEBUFSIZE;
		bmpbuffer->leer(write_buffer,bytes_escritos_totales,bytes_a_escribir);
		fwrite(write_buffer,1,bytes_a_escribir,bmpfile);
		bytes_escritos_totales+=bytes_a_escribir;
	}
	free(write_buffer);
	fclose(bmpfile);
	return(OK);
}

int bmp::encode24(imagen *bmpimg)
{
	unsigned long bmppos;	//posici¢n en el buffer XMS del BMP
	unsigned long imgpos;	//posici¢n en el buffer XMS de la imagen
	unsigned imgy;			//no. de linea horizontal
	unsigned imghbytes; 	//no. de bytes por linea horizontal (imagen)
	unsigned bmphbytes;		//no. de bytes por linea horizontal (BMP)
	unsigned char *buf_linea;	//buffer de linea
	imghbytes=3*bmpheader.biwidth;
	bmphbytes=real_x;
	if ((buf_linea=(unsigned char *)calloc(bmphbytes,1))==NULL)
	{
		return(MEM_ALLOC_ERROR);
	}
	for (imgy=0;imgy<bmpheader.biheight;imgy++)
	{
		bmppos=(long)bmphbytes*((long)real_y-imgy-1);
		imgpos=(long)imghbytes*(long)imgy;
		bmpimg->leer(buf_linea,imgpos,imghbytes);
		bmpbuffer->escribir(buf_linea,bmppos,bmphbytes);
	}
	free(buf_linea);
	return(OK);
}

int bmp::encode8(imagen *bmpimg)
{
	unsigned long bmppos;	//posici¢n en el buffer XMS del BMP
	unsigned long imgpos;	//posici¢n en el buffer XMS de la imagen
	unsigned imgy;			//no. de linea horizontal
	unsigned imghbytes; 	//no. de bytes por linea horizontal (imagen)
	unsigned bmphbytes;		//no. de bytes por linea horizontal (BMP)
	unsigned char *buf_linea;	//buffer de linea

	paleta_ncolores=256;
	imghbytes=bmpheader.biwidth;
	bmphbytes=real_x;
	if ((buf_linea=(unsigned char *)calloc(bmphbytes,1))==NULL)
		return(MEM_ALLOC_ERROR);
	if (escribir_paleta(bmpimg)!=OK)
		return (MEM_ALLOC_ERROR);
	for (imgy=0;imgy<bmpheader.biheight;imgy++)
	{
		bmppos=(long)bmphbytes*((long)real_y-imgy-1);
		imgpos=(long)imghbytes*(long)imgy;
		bmpimg->leer(buf_linea,imgpos,imghbytes);
		bmpbuffer->escribir(buf_linea,bmppos,bmphbytes);
	}
	free(buf_linea);
	return(OK);
}


int bmp::escribir_paleta(imagen *bmpimg)
{
	unsigned register color;
	regpal reg;
	fseek(bmpfile,54,0);
	for (color=0;color<paleta_ncolores;color++)
	{
		bmpimg->leer_registro_paleta(color,&(reg.r),
			&(reg.g),&(reg.b),&(reg.y));
		fputc(reg.b,bmpfile);
		fputc(reg.g,bmpfile);
		fputc(reg.r,bmpfile);
		fputc(!reg.y,bmpfile);
	}
	return(OK);
}

bmp::~bmp()
{
	if (bmpbuffer!=NULL)
		delete (bmpbuffer);
}
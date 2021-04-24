#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <graphics.h>
#include <alloc.h>
#include <io.h>
#include "xms_ctrl.h"
#include "xms_buf.h"
#include "imagen.h"
#include "mouse.h"
#include "sbvperr.h"
#include "krpfont.h"
#include "boton.h"
#include "menu.h"
#include "cdialogo.h"
#include "listbox.h"
#include "filedlg.h"

#include "bmp.h"
#include "vesa.h"

#include "cbarras.h"
#include "media.h"

int inigraf(void)
{
	int driver=VGA;
	int modo=VGAHI;
	initgraph(&driver,&modo,"");
	return(graphresult());
}

void fingraf(void)
{
	closegraph();
}

void ventana_general(krpfont *k)
{
	int left=0,right=639,up=0,down=479;
	int ancho_titulo=30,ancho_borde=5,ancho_barra_inferior=80;
	char *titulo="SBVP 1.00 - Proyecto Fin de Carrera por "
		"David Crespo Tasc¢n, E.U.I.T.T.";

	cleardevice();
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(left,up,right,down);
	setfillstyle(SOLID_FILL,RED);
	bar(left,up,right,up+1+ancho_titulo);
	setcolor(DARKGRAY);
	line(left,down,right,down);
	line(right,down,right,up);
	line(left+ancho_borde+1,up+ancho_borde+2+ancho_titulo,
		right-ancho_borde-1,up+ancho_borde+2+ancho_titulo);
	line(left+ancho_borde+1,up+ancho_borde+2+ancho_titulo,
		left+ancho_borde+1,down-ancho_borde-1);
	setcolor(WHITE);
	line(left,up,right,up);
	line(left,up,left,down);
	line(left+ancho_borde+1,down-ancho_borde-1,
		right-ancho_borde-1,down-ancho_borde-1);
	line(right-ancho_borde-1,down-ancho_borde-1,
		right-ancho_borde-1,up+ancho_borde+2+ancho_titulo);
	k->kprintf(titulo,((right-left)>>1)-((k->longtexto(titulo))>>1),
		up+(ancho_titulo>>1)-6,WHITE);
	setcolor(DARKGRAY);
	line(left+2*ancho_borde,down-2*ancho_borde,
		right-2*ancho_borde,down-2*ancho_borde);
	line(right-2*ancho_borde,down-2*ancho_borde-ancho_barra_inferior,
		right-2*ancho_borde,down-2*ancho_borde);
	setcolor(WHITE);
	line(left+2*ancho_borde,down-2*ancho_borde-ancho_barra_inferior,
		right-2*ancho_borde,down-2*ancho_borde-ancho_barra_inferior);
	line(left+2*ancho_borde,down-2*ancho_borde-ancho_barra_inferior,
		left+2*ancho_borde,down-2*ancho_borde);
	{
		FILE *photo;
		unsigned size;
		void far *imagen;
		if ((photo=fopen("sbvplogo.bgp","rb"))==NULL) return;
		size=filelength(fileno(photo));
		if ((imagen=(void far *)calloc(1,size))==NULL) return;
		fread (imagen,size,1,photo);
		putimage(90,80,imagen,0);
		free(imagen);
		fclose(photo);
	}
}

void getpiccoords(int numpic,int *x,int *y)
{
	div_t result;
	result=div(numpic,5);
	*x=120*result.rem;
	*y=80*result.quot;
	*x+=30;
	*y=300-*y;
}


int interfaz(void)
{
	#define N_ICONOS 20
	struct icono_imagen{
		char nombre[13];
		int valido;
		int x,y;
		boton *bot;
		imagen *img;
	}pic[N_ICONOS];
	void far *icono;
	krpfont krp;
	int x,y,button;

	xmscontrol xms;

	{
		int cont;
		for (cont=0;cont<N_ICONOS;cont++)
			pic[cont].valido=0;
	}

	{
		FILE *infile;
		unsigned size;
		if ((infile=fopen("icon.bgp","rb"))!=NULL)
		{
			size=filelength(fileno(infile));
			if ((icono=(void far *)calloc(1,size))!=NULL)
			{
				fread (icono,size,1,infile);
			}
			fclose(infile);
		}
	}

	vesa_video_subsystem vesa;

	if(registerbgidriver(EGAVGA_driver)<0)
		return(BGI_DRIVER_ERROR);
	inigraf();
	if (mouseinstalled()!=OK)
	{
		fingraf();
		printf("\nError: no existe controlador de rat¢n cargado");
		return(MOUSE_NOT_INSTALLED);
	}
	switch(vesa.last_error){
		case MEM_ALLOC_ERROR:
		{
			char *lista[3]={"ERROR",
				"Error en asignaci¢n de memoria",
				"Programa inestable. Saliendo..."};
			cdialogo alloc_err(3,lista,0,0,&krp);
			return(MEM_ALLOC_ERROR);
		}
		case VESA_NOT_SUPPORTED:
		{
			char *lista[3]={"ERROR",
				"Esta tarjeta gr fica no soporta modos VESA",
				"No se podr n visualizar im genes"};
			cdialogo vesa_err(3,lista,0,0,&krp);
			break;
		}
		case VESA_VBE12_NOT_SUPP:
		{
			char *lista[3]={"ERROR",
				"Esta tarjeta gr fica no soporta la especificaci¢n VBE 1.2",
				"Utilice un driver VESA antes de cargar este programa"};
			cdialogo vbe12_err(3,lista,0,0,&krp);
			break;
		}
	}
	ventana_general(&krp);

	muestra_cursor();
	boton nueva(25,420,80,20,"Nueva Imagen",&krp);
	boton cargar(125,420,80,20,"Cargar Imagen",&krp);
	boton info(225,420,80,20,"Informaci¢n",&krp);
	boton opciones(325,420,80,20,"Opciones",&krp);
	boton acercade(425,420,80,20,"Acerca de...",&krp);
	boton salir(525,420,80,20,"Salir",&krp);

	while(1)
	{
		if(nueva.activado())
		{
			int cont=0;
			while((pic[cont].valido!=0)&&(cont<N_ICONOS)) cont++;
			if (pic[cont].valido==0)
			{
				if (cbarras(pic[cont].img)==OK)
				{
					strcpy(pic[cont].nombre,"cbarras.bmp");
					getpiccoords(cont,&pic[cont].x,&pic[cont].y);
					pic[cont].bot=new boton(pic[cont].x,pic[cont].y,
						pic[cont].nombre,&krp,icono);
					pic[cont].valido=1;
				}
			}
			if (cont>=N_ICONOS)
			{
				char *lista[2]={"ERROR",
					"No se pueden cargar m s im genes"};
				cdialogo alloc_err(2,lista,0,0,&krp);
			}
		}
		if(cargar.activado())
		{
			char filename[100];
			char name[13];
			filedialog *fd;
			fd=new filedialog("Cargar Imagen",filename,name,&krp);
			if (fd!=NULL)
				delete fd;
			if (filename[0]!=0)
			{
				int cont=0;
				while((pic[cont].valido!=0)&&(cont<N_ICONOS)) cont++;
				if (pic[cont].valido==0)
				{
					bmp bmpfile;
					int error;
					if ((error=bmpfile.leer_de_fichero(filename,pic[cont].img))==OK)
					{
						strcpy(pic[cont].nombre,name);
						getpiccoords(cont,&pic[cont].x,&pic[cont].y);
						pic[cont].bot=new boton(pic[cont].x,pic[cont].y,
							pic[cont].nombre,&krp,icono);
						pic[cont].valido=1;
					}
					else
					{
						char *texto[2]={"ERROR","1234567890123456789012345678901234567890"};
						get_error(error,texto[1]);
						cdialogo errdlg(2,texto,0,0,&krp);
					}
				}
				if (cont>=N_ICONOS)
				{
					char *lista[2]={"ERROR",
						"No se pueden cargar m s im genes"};
					cdialogo alloc_err(2,lista,0,0,&krp);
				}
			}
		}
		if(info.activado())
		{
			int x,y,opcion;
			char *opciones[]={"Memoria libre","Informaci¢n VESA",
								"Verificaci¢n del Heap"};
			leeraton(&x,&y,&opcion);
			menu info(3,opciones,x,y,&krp,&opcion);
			switch (opcion)
			{
				case 0:
				{
					char *lista[3]={"Memoria Libre",
									"123456789012346578901234567890",
									"123465789012345678901234567890"};
					sprintf(lista[1],"Convencional: %u KB",coreleft()>>10);
					sprintf(lista[2],"Extendida (XMS): %u KB",xms.getfree());
					cdialogo memoria(3,lista,0,0,&krp);
					break;
				}
				case 1:
				{
					break;
				}
				case 2:
				{
					oculta_cursor();
					while (kbhit()) getch();	//vaciar el buffer de teclado
					fingraf();
					{
						struct heapinfo hi;
						hi.ptr = NULL;
						printf( "  Tama¤o  Estado\n" );
						printf( "  ------  ------\n" );
						while( heapwalk( &hi ) == _HEAPOK )
							printf( "%7u    %u\n", hi.size, hi.in_use );
						printf("\nEstado: 0=Sin usar, 1=Usado\n");
						if( heapcheck() == _HEAPCORRUPT )
							printf( "Heap is corrupted.\n" );
						else
							printf( "Heap is OK.\n" );
					}
					getch();
					inigraf();
					ventana_general(&krp);
					boton::actualizar=N_ICONOS+6;
					mouseinstalled();
					muestra_cursor();
					break;
				}
			}
		}
		if(opciones.activado())
		{
			char *lista[2]={"Funcion no Implementada",
							"Pr¢ximamente..."};
			cdialogo memoria(2,lista,0,0,&krp);
		}
		if(acercade.activado())
		{
			char *texto[]={	"Acerca de...",
							"SBVP - Super Bitmap View & Process",
							"Ì 1996/1997 David Crespo Tasc¢n"};
			cdialogo acercade(3,texto,0,0,&krp);
		}
		if(salir.activado())
		{
			char *texto[]={	"Salir de SBVP",
							"¨Seguro que quieres salir?"};
			int respuesta;
			cdialogo salir(2,texto,0,0,&krp,&respuesta);
			if (respuesta)
			{
				oculta_cursor();
				for (int cont=0;cont<N_ICONOS;cont++)
				{
					if (pic[cont].valido!=0)
					{
						delete pic[cont].bot;
						delete pic[cont].img;
					}
				}
				free(icono);
				fingraf();
				return(OK);
			}
		}
		for (int cont=0;cont<N_ICONOS;cont++)
		{
			if (pic[cont].valido!=0)
			{
				if (pic[cont].bot->activado())
				{
					int x,y,opcion;
					char *opciones[]={"Visualizar","Informaci¢n",
						"Filtro de Media","Guardar Imagen","Eliminar Imagen"};
					leeraton(&x,&y,&opcion);
					menu opts(5,opciones,x,y,&krp,&opcion);
					switch (opcion)
					{
						case 0:
						{
							oculta_cursor();
							while (kbhit()) getch();	//vaciar el buffer de teclado
							fingraf();
							vesa.show(pic[cont].img);
							inigraf();
							ventana_general(&krp);
							boton::actualizar=N_ICONOS+6;
							mouseinstalled();
							muestra_cursor();
							break;
						}
						case 1:
						{
							char *lista[4]={"Informaci¢n de la Imagen",
											"Nombre: 12345678.123",
											"Resoluci¢n: 12345x12345",
											"N§ de bits/pixel: 24 "};
							sprintf(lista[1],"Nombre: %s",pic[cont].nombre);
							sprintf(lista[2],"Resoluci¢n: %dx%d",
								pic[cont].img->getxsize(),pic[cont].img->getysize());
							sprintf(lista[3],"N§ de bits/pixel: %d",
								pic[cont].img->getbitsperpixel());
							cdialogo info(4,lista,0,0,&krp);
							break;
						}
						case 2:
						{
							int orig;
							orig=cont;
							cont=0;
							while((pic[cont].valido!=0)&&(cont<N_ICONOS)) cont++;
							if (pic[cont].valido==0)
							{
								if (media(pic[orig].img,pic[cont].img)==OK)
								{
									strcpy(pic[cont].nombre,"media");
									getpiccoords(cont,&pic[cont].x,&pic[cont].y);
									pic[cont].bot=new boton(pic[cont].x,pic[cont].y,
										pic[cont].nombre,&krp,icono);
									pic[cont].valido=1;
								}
							}
							if (cont>=N_ICONOS)
							{
								char *lista[2]={"ERROR",
									"No se pueden cargar m s im genes"};
								cdialogo alloc_err(2,lista,0,0,&krp);
							}
							break;
						}
						case 3:
						{
							bmp bmpfile;
							int error;
							if ((error=bmpfile.escribir_a_fichero("TEST.BMP",pic[cont].img))!=OK)
							{
								char *texto[2]={"ERROR","1234567890123456789012345678901234567890"};
								get_error(error,texto[1]);
								cdialogo errdlg(2,texto,0,0,&krp);
							}
							break;
						}
						case 4:
						{
							delete pic[cont].bot;
							delete pic[cont].img;
							pic[cont].valido=0;
							oculta_cursor();
							ventana_general(&krp);
							boton::actualizar=N_ICONOS+6;
							muestra_cursor();
						}
					}
				}
			}
		}
		{
			static una_vez=1;
			if (una_vez)
			{
				if( heapcheck() == _HEAPCORRUPT )
				{
					char *texto[2]={"ERROR","­HEAP CORRUPTO!"};
					cdialogo errdlg(2,texto,0,0,&krp);
					una_vez=0;
				}
			}
		}
	}
}



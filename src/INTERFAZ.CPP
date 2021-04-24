#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
#include "listbox.h"
#include "filedlg.h"

#include "bmp.h"
#include "vesa.h"

#include "cbarras.h"
#include "cuadro.h"
#include "filtros.h"
#include "conv.h"

#include "cdialogo.h"
#include "querybox.h"

#include "complejo.h"
#include "fftclass.h"
#include "fft2d.h"
#include "histogrm.h"
#include "bplanes.h"

#include "interfaz.h"

static cdialogo *cdlg;

extern float comp_factor;		//factor de compensaci¢n
								//en el c lculo del logaritmo
								//del m¢dulo de la FFT

extern int DebugMode;			//si es distinto de 0, estamos
								//en modo de depuraci¢n

interfaz::interfaz(char *ver_in)
{
	xms=new xmscontrol;		//memoria extendida
	if (xms==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	last_error=xms->last_error;
	if (last_error!=OK)
		return;
	vesa=new vesa_video_subsystem;	//sistema gr fico
	if (vesa==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	last_error=vesa->last_error;
	if ((last_error!=OK)&&(DebugMode==0))
		return;
	k=new krpfont;					//tipo de letra
	if (k==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	last_error=k->last_error;
	if (k->last_error!=OK)
		return;
	last_error=cargar_logo(logo_fondo,"SBVPLOGO.BGP");
	if (last_error!=OK) return;
	last_error=cargar_logo(logo_icono,"ICON.BGP");
	if (last_error!=OK) return;
	if (mouseinstalled()!=OK)
	{
		last_error=MOUSE_NOT_INSTALLED;
		return;
	}
	if(registerbgidriver(EGAVGA_driver)<0)
	{
		last_error=BGI_DRIVER_ERROR;
		return;
	}
	for (int cont=0;cont<N_ICONOS;cont++)
		pic[cont].valido=0;
	for (cont=0;cont<N_MAX_MENUS;cont++)
		menuptr[cont]=NULL;
	m_ac=0;
	ver=ver_in;
	cdlg=NULL;
	last_error=NULL;
}

int interfaz::inigraf(void)
{
	int driver=VGA;
	int modo=VGAHI;
	initgraph(&driver,&modo,"");
	return(graphresult());
}

void interfaz::errores_iniciales(void)
{
	switch(vesa->last_error){
		case MEM_ALLOC_ERROR:
		{
			char *lista[3]={"ERROR",
				"Error en asignaci¢n de memoria",
				"Programa inestable."};
			cdlg=new cdialogo(3,lista,0,0,k);
			delcdlg();
			break;
		}
		case VESA_NOT_SUPPORTED:
		{
			char *lista[3]={"ERROR",
				"Esta tarjeta gr fica no soporta modos VESA",
				"No se podr n visualizar im genes"};
			cdlg=new cdialogo(3,lista,0,0,k);
			delcdlg();
			break;
		}
		case VESA_VBE12_NOT_SUPP:
		{
			char *lista[3]={"ERROR",
				"Esta tarjeta gr fica no soporta la especificaci¢n VBE 1.2",
				"Utilice un driver VESA antes de cargar este programa"};
			cdlg=new cdialogo(3,lista,0,0,k);
			break;
		}
	}
}

void interfaz::delcdlg(void)
{
	if (cdlg!=NULL)
	{
		delete cdlg;
		cdlg=NULL;
	}
}

void interfaz::fingraf(void)
{
	closegraph();
}

void interfaz::ventana_general(void)
{
	int left=0,right=639,up=0,down=479;
	int ancho_titulo=30,ancho_borde=5,ancho_barra_inferior=80;
	char titulo[200];

	sprintf(titulo,"SBVP v%s - Programa de tratamiento digital "
		"de la imagen",ver);

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
	putimage(90,80,logo_fondo,0);
}

void interfaz::getpiccoords(int numpic,int *x,int *y)
{
	div_t result;
	result=div(numpic,5);
	*x=120*result.rem;
	*y=80*result.quot;
	*x+=30;
	*y=300-*y;
}

int interfaz::cargar_logo(void far *(&logo),char *nombre)
{
	FILE *logofile;
	unsigned size;
	if ((logofile=fopen(nombre,"rb"))==NULL)
		return(FILE_OPEN_ERROR);
	size=filelength(fileno(logofile));
	if ((logo=(void far *)calloc(1,size))==NULL)
		return(MEM_ALLOC_ERROR);
	fread (logo,size,1,logofile);
	fclose(logofile);
	return(OK);
}


int interfaz::start(void)
{
	int x,y,button;
	if (inigraf()!=OK)
	{
		printf("Error al inicializar el subsistema gr fico");
		return (BGI_DRIVER_ERROR);
	}
	ventana_general();

	muestra_cursor();
	cursor_flecha();
	BarraBoton[0]=new boton (25,420,80,20,"Nueva Imagen",k);
	if (BarraBoton[0]==NULL) return (MEM_ALLOC_ERROR);
	BarraBoton[1]=new boton (125,420,80,20,"Cargar Imagen",k);
	if (BarraBoton[1]==NULL) return (MEM_ALLOC_ERROR);
	BarraBoton[2]=new boton (225,420,80,20,"Informaci¢n",k);
	if (BarraBoton[2]==NULL) return (MEM_ALLOC_ERROR);
	BarraBoton[3]=new boton (325,420,80,20,"Opciones",k);
	if (BarraBoton[3]==NULL) return (MEM_ALLOC_ERROR);
	BarraBoton[4]=new boton (425,420,80,20,"Acerca de...",k);
	if (BarraBoton[4]==NULL) return (MEM_ALLOC_ERROR);
	BarraBoton[5]=new boton (525,420,80,20,"Salir",k);
	if (BarraBoton[5]==NULL) return (MEM_ALLOC_ERROR);

	while(HeapOk())
	{
		if(BarraBoton[0]->activado())
		{
			int error=OK;
			char *szCbarras="C_BARRAS";
			char *szCuadrado="CUADRADO";
			char *szNombre;
			imagen *newimg=NULL;
			int (*nuevo)(imagen *(&))=NULL;	//funci¢n que crea nueva imagen
			int x,y,opcion;
			char *opciones[]={"Carta de barras","Cuadrado"};
			leeraton(&x,&y,&opcion);
			menuptr[m_ac++]=new menu(2,opciones,x,y,k,&opcion);
			switch(opcion)
			{
				case 0:
					nuevo=cbarras;
					szNombre=szCbarras;
					break;
				case 1:
					nuevo=cuadro;
					szNombre=szCuadrado;
					break;
			}
			cerrar_menus();
			if (nuevo!=NULL)
			{
				if (getfreepics())
				{
					error=nuevo(newimg);
					if (error==OK)
						newpic(newimg,szNombre);
				}
				else error=ALL_ICONS_OPEN;
				mostrar_error(error);
			}
		}
		if(BarraBoton[1]->activado())
		{
			char RutaNombreExt[80];	//ruta y nombre completo (para abrir)
			char NombreExt[13];	//nombre completo(con extension)
			char Nombre[13];		//nom. sin ext. para poner en icono
			int error;
			filedialog *fd;
			fd=new filedialog("Cargar Imagen",RutaNombreExt,NombreExt,k);
			if (fd!=NULL) delete fd;
			error=fd->last_error;
			if (error==OK)
			{
				imagen *newimg=NULL;
				if (getfreepics())
				{
					bmp *bmpfile;
					bmpfile=new bmp;
					if (bmpfile!=NULL)
					{
						cdlg=new cdialogo(0,0,k);
						error=bmpfile->leer_de_fichero(RutaNombreExt,newimg);
						if ((error)==OK)
						{
							get_nombre(Nombre,NombreExt);
							newpic(newimg,Nombre);
						}
						delcdlg();
						delete bmpfile;
					}
					else error=MEM_ALLOC_ERROR;
				}
				else error=ALL_ICONS_OPEN;
			}
			mostrar_error(error);
		}
		if(BarraBoton[2]->activado())
		{
			int x,y,opcion;
			char *opciones[]={"Memoria libre","Informaci¢n VESA ý",
								"Verificaci¢n del Heap"};
			leeraton(&x,&y,&opcion);
			menuptr[m_ac++]=new menu(DebugMode?3:2,opciones,x,y,k,&opcion);
			switch (opcion)
			{
				case 0:
				{
					mostrar_memoria_libre();
					break;
				}
				case 1:
				{
					mostrar_info_vesa();
					break;
				}
				case 2:
				{
					mostrar_heap();
					break;
				}
			}
			cerrar_menus();
		}
		if(BarraBoton[3]->activado())
		{
			char *nada;
			char *lista[3]={"Ajuste de la FFT",
					"Introduce el factor corrector para el",
					"c lculo del logaritmo del m¢dulo de la FFT"};

			long valor=comp_factor;
			cdlg=new cdialogo(3,lista,0,0,k,&valor);
			comp_factor=valor;
			delcdlg();
		}
		if(BarraBoton[4]->activado())
		{
			char *texto[]={	"Acerca de...",
							"SBVP - Super Bitmap View & Process",
							"Proyecto Fin de Carrera",
							"Autor: David Crespo Tasc¢n",
							"Ì 1996/1997 "};
			cdlg=new cdialogo(5,texto,0,0,k);
			delcdlg();
		}
		if((BarraBoton[5]->activado())||(TeclaPulsada()==27))
		{
			char *texto[]={	"Salir de SBVP",
							"¨Seguro que quieres salir?"};
			int respuesta;
			cdlg=new cdialogo(2,texto,0,0,k,&respuesta);
			delcdlg();
			if (respuesta)
			{
				break;
			}
		}
		for (int cont=0;cont<N_ICONOS;cont++)
		{
			if (pic[cont].valido!=0)
			{
				if (pic[cont].bot->activado())
				{
					int x,y,opcion;
					imagen *baseimg=pic[cont].img;
					char *baseimgname=pic[cont].nombre;
					char *opciones[]={"Visualizar","Informaci¢n","Histograma ý",
					"Filtros ý","FFT","Convertir Imagen ý","Planos de bit ý",
					"Cuantificar ý","Guardar Imagen","Eliminar Imagen"};
					leeraton(&x,&y,&opcion);
					menuptr[m_ac++]=new menu(10,opciones,x,y,k,&opcion);
					switch (opcion)
					{
						case 0:
						{
							oculta_cursor();
							while (kbhit()) getch();	//vaciar el buffer de teclado
							fingraf();
							int error=vesa->show(baseimg);
							inigraf();
							ventana_general();
							mouseinstalled();
							muestra_cursor();
							ActualizarBotones();
							mostrar_error(error);
							break;
						}
						case 1: // informacion
						{
							char *lista[5]={"Informaci¢n de la Imagen",
											"Nombre: 12345678.123",
											"Resoluci¢n: 12345x12345",
											"N§ de bits/pixel: 24 ",
											"Escala de grises:    "};
							sprintf(lista[1],"Nombre: %s",baseimgname);
							sprintf(lista[2],"Resoluci¢n: %dx%d",
								baseimg->getxsize(),baseimg->getysize());
							sprintf(lista[3],"N§ de bits/pixel: %d",
								baseimg->getbitsperpixel());
							sprintf(lista[4],"Escala de grises: %2s ",
								baseimg->escala_de_grises?"SI":"NO");
							cerrar_menus();
							cdlg=new cdialogo((baseimg->getbitsperpixel()==24)
								?4:5,lista,0,0,k);
							delcdlg();
							break;
						}
						case 2: // histograma
						{
							int error=OK;
							imagen *newimg=NULL;
							char *histopts[3]={"Ver Histograma",
								"Ecualizar Histograma (normal)",
								"Ecualizar Histograma (corregido)"};
							histogram *histo=NULL;
							leeraton(&x,&y,&opcion);
							menuptr[m_ac++]=new menu(baseimg->escala_de_grises?3:1,
										histopts,x,y,k,&opcion);
							switch (opcion)
							{
								case 0:
									cerrar_menus();
									histo=new histogram();
									if (histo==NULL)
										error=MEM_ALLOC_ERROR;
									else
									{
										error=histo->last_error;
										if (error==OK)
											error=histo->ShowHistogram(baseimg,k);
										delete histo;
									}
									if (error!=OK)
										mostrar_error(error);
									break;
								case 1:
									cerrar_menus();
									if (getfreepics())
									{
										histo=new histogram();
										if (histo==NULL)
											error=MEM_ALLOC_ERROR;
										else
										{
											if (histo->last_error!=OK)
												error=histo->last_error;
											else
											{
												error=histo->EqualizeHisto(baseimg,newimg);
												if (error==OK)
												{
													strcpy(aux_filename,baseimgname);
													anadir_tag('h',aux_filename);
													newpic(newimg,aux_filename);
												}
											}
											delete histo;
										}
									}
									else error=ALL_ICONS_OPEN;
									mostrar_error(error);
									break;
								case 2:
									cerrar_menus();
									if (getfreepics())
									{
										histo=new histogram();
										if (histo==NULL)
											error=MEM_ALLOC_ERROR;
										else
										{
											if (histo->last_error!=OK)
												error=histo->last_error;
											else
											{
												error=histo->EqualizeHistoCorr(baseimg,newimg);
												if (error==OK)
												{
													strcpy(aux_filename,baseimgname);
													anadir_tag('h',aux_filename);
													newpic(newimg,aux_filename);
												}
											}
											delete histo;
										}
									}
									else error=ALL_ICONS_OPEN;
									mostrar_error(error);
									break;
							}
							break;
						}
						case 3: // filtros
						{
							int error;
							error=filtros(baseimg,baseimgname);
							mostrar_error(error);
							break;
						}
						case 4: // fft
						{
							imagen *newimg=NULL;
							fft2d *imgFFT=NULL;
							int error;
							cerrar_menus();
							if (getfreepics())
							{
								imgFFT=new fft2d(baseimg,newimg);
								if (imgFFT==NULL)
									error=MEM_ALLOC_ERROR;
								else
								{
									if (imgFFT->last_error!=OK)
									{
										error=imgFFT->last_error;
										delete imgFFT;
									}
									else
									{
										cdlg=new cdialogo(0,0,k);
										error=imgFFT->CalcFFT();
										delete imgFFT;
										delcdlg();
										if ((error)==OK)
										{
											strcpy(aux_filename,baseimgname);
											anadir_tag('f',aux_filename);
											newpic(newimg,aux_filename);
										}
									}
								}
							}
							else error=ALL_ICONS_OPEN;
							mostrar_error(error);
							break;
						}
						case 5: // convertir imagen
						{
							convertir(baseimg,baseimgname);
							break;
						}
						case 6: // planos de bit
						{
							int error;
							error=BitPlanes(baseimg,baseimgname);
							mostrar_error(error);
							ActualizarBotones();
							break;
						}
						case 7: // cuantificar
						{
							int error;
							error=Quant(baseimg,baseimgname);
							mostrar_error(error);
							ActualizarBotones();
							break;
						}
						case 8: // guardar imagen
						{
							bmp *bmpfile;
							int error;

							cerrar_menus();
							bmpfile=new bmp;
							if (bmpfile==NULL)
							{
								mostrar_error(MEM_ALLOC_ERROR);
								break;
							}
							strcpy(aux_filename,pic[cont].nombre);
							cdlg=new cdialogo(0,0,k,aux_filename,".BMP");
							delcdlg();
							if (aux_filename[0])
							{
								if ((error=bmpfile->escribir_a_fichero(aux_filename,pic[cont].img))!=OK)
									mostrar_error(error);
								else
								{
									get_nombre(aux_filename,aux_filename);
									strcpy(pic[cont].nombre,aux_filename);
								}
							}
							delete bmpfile;
							ActualizarBotones();
							break;
						}
						case 9: // eliminar imagen
						{
							cerrar_menus();
							delete pic[cont].bot;
							delete pic[cont].img;
							pic[cont].valido=0;
							oculta_cursor();
							ventana_general();
							ActualizarBotones();
							muestra_cursor();
							break;
						}
					}
					cerrar_menus();
				}
			}
		}
	}
	oculta_cursor();
	for (int cont=0;cont<N_ICONOS;cont++)
	{
		if (pic[cont].valido!=0)
		{
			delete pic[cont].bot;
			delete pic[cont].img;
		}
	}
	for (cont=0;cont<6;cont++)
	{
		delete BarraBoton[cont];
	}
	fingraf();
	return(OK);
}

int interfaz::HeapOk(void)
{
	if( heapcheck() == _HEAPCORRUPT )
	{
		mostrar_error(HEAP_CORRUPT);
		return (0);
	}
	else return (1);
}

void interfaz::ActualizarBotones(void)
{
	int cont;
	for (cont=0;cont<6;cont++)
	{
		BarraBoton[cont]->actualizar();
	}
	for (cont=0;cont<N_ICONOS;cont++)
	{
		if (pic[cont].valido)
			pic[cont].bot->actualizar();
	}
}

int interfaz::getfreepics(void)
{
	int cont,freepics=0;
	for (cont=0;cont<N_ICONOS;cont++)
		if (pic[cont].valido==0)
			freepics++;
	return(freepics);
}

int interfaz::getpicnum(void)
{
	int cont=0;
	for (cont=0;cont<N_ICONOS;cont++)
		if (pic[cont].valido==0)
			return(cont);
	return(-1);
}

int interfaz::newpic(imagen *newimg,char *nombre)
{
	int num;
	num=getpicnum();
	if (num==-1)
	{
		if (newimg!=NULL)
			delete newimg;
		return ALL_ICONS_OPEN;
	}
	strcpy(pic[num].nombre,nombre);
	getpiccoords(num,&pic[num].x,&pic[num].y);
	pic[num].bot=new boton(pic[num].x,pic[num].y,
		pic[num].nombre,k,logo_icono);
	if (pic[num].bot==NULL)
		return(MEM_ALLOC_ERROR);
	pic[num].img=newimg;
	pic[num].valido=1;
	pic[num].img->TestGrayScale();
	return (OK);
}

void interfaz::mostrar_error(int error)
{
	if (error==OK)
		return;
	char *texto[2]={"ERROR"};
//	texto[1]=new char[100];
//	if (texto[1]!=NULL)
	get_error(error,texto[1]);
	cdlg=new cdialogo(2,texto,0,0,k);
	delcdlg();
	if (texto[1]!=NULL)
		delete texto[1];
}

void interfaz::mostrar_memoria_libre(void)
{
	char *lista[3]={"Memoria Libre"};
	lista[1]=new char[40];
	lista[2]=new char[40];
	cerrar_menus();
	if(lista[1]!=NULL)
	{
		if(DebugMode)
			sprintf(lista[1],"Convencional: %lu bytes",coreleft());
		else sprintf(lista[1],"Convencional: %u KB",coreleft()>>10);
	}
	if(lista[2]!=NULL)
		sprintf(lista[2],"Extendida (XMS): %u KB",xms->getfree());
	cdlg=new cdialogo(3,lista,0,0,k);
	delcdlg();
	if(lista[1]!=NULL) delete lista[1];
	if(lista[2]!=NULL) delete lista[2];
}

void interfaz::mostrar_heap(void)
{
	oculta_cursor();

    LiberarTecla();
	fingraf();
	{
		struct heapinfo hi;
		int NAssBlks=0;
		hi.ptr = NULL;
		printf("\nVerificaci¢n del Heap: Bloques asignados\n");
		while( heapwalk( &hi ) == _HEAPOK )
		{
			printf( " %7u %7u", hi.size, hi.in_use );
			NAssBlks++;
		}
		if( heapcheck() == _HEAPCORRUPT )
			printf("\nEl Heap est  corrupto.");
		else
			printf("\nEl Heap est  correctamente.");
		printf("\nN£mero de bloques asignados: %u",NAssBlks);
	}
	getch();
	inigraf();
	ventana_general();
	ActualizarBotones();
	mouseinstalled();
	muestra_cursor();
}

void interfaz::mostrar_info_vesa(void)
{
	int x,y,opcion;
	char *opciones[]={"N£mero de versi¢n","Fabricante",
					  "Modos gr ficos soportados ý"};
	esperar_soltar();	//esperar a que se suelte el bot¢n del rat¢n
	leeraton(&x,&y,&opcion);
	menuptr[m_ac++]=new menu(3,opciones,x,y,k,&opcion);
	switch (opcion)
	{
		case 0:
		{
			cerrar_menus();
			char *lista[2]={"Informaci¢n VESA",
							"N£mero de versi¢n: XXXX"};
			char *version=vesa->GetVerNumber();
			char *aux=lista[1]+19;
			strcpy(aux,version);
			cdlg=new cdialogo(2,lista,0,0,k);
			delcdlg();
			break;
		}
		case 1:
		{
			cerrar_menus();
			char *lista[3]={"Informaci¢n VESA",
							"Cadena de identificaci¢n del fabricante:"};
			lista[2]=vesa->GetVendorName();
			cdlg=new cdialogo(3,lista,0,0,k);
			delcdlg();
			break;
		}
		case 2:
		{
			unsigned *modos=vesa->GetModeList();
			int x,y,opcion;
			char *opciones[]={"Ver lista de modos","Info sobre un modo"};
			esperar_soltar();	//esperar a que se suelte el bot¢n del rat¢n
			leeraton(&x,&y,&opcion);
			menuptr[m_ac++]=new menu(2,opciones,x,y,k,&opcion);
			switch (opcion)
			{
				case 0:
				{
					int nmodo=0,nmodos=0;
					int ModosPorLinea=5;
					int ModosEnLinea=5;
					int nlinea=0;
					char *LineaAux;
					char *ListaModos[10]={"Lista de modos gr ficos"};
					unsigned *aux_modos=modos;
					cerrar_menus();
					while ((*aux_modos++!=0xFFFF)&&(nmodos<100))
						nmodos++;
					aux_modos=modos;
					LineaAux=ListaModos[1];
					while (nmodo<nmodos)
					{
						if (ModosEnLinea<ModosPorLinea)
						{
							sprintf(LineaAux,"%X, ",*aux_modos++);
							LineaAux+=5;
							ModosEnLinea++;
							nmodo++;
						}
						else
						{
							nlinea++;
							ListaModos[nlinea]=new char [40];
							LineaAux=ListaModos[nlinea];
							ModosEnLinea=0;
						}
					}
					cdlg=new cdialogo(nlinea+1,ListaModos,0,0,k);
					while(nlinea)	//borrar buffers lineas 1 a nlinea+1
						if (ListaModos[nlinea]!=NULL)
							delete ListaModos[nlinea--];
					delcdlg();
					break;
				}
				case 1:
				{
					long numeromodo=0x100;
					char *lista[3]={"Petici¢n de informaci¢n sobre un modo",
							"Introduzca el n£mero de modo",
							"(en hexadecimal)"};

					char valor[10]="100";
					cerrar_menus();
					cdlg=new cdialogo(3,lista,0,0,k,valor);
					delcdlg();
					numeromodo=ahtol(valor);
					{
						char *ListaInfo[6];
						int nlinea;
						for (nlinea=0;nlinea<6;nlinea++)
							ListaInfo[nlinea]=new char [50];
						if (vesa->GetModeInfo(numeromodo,ListaInfo))
							cdlg=new cdialogo(6,ListaInfo,0,0,k);
						else
						{
							sprintf(ListaInfo[0],"ERROR");
							sprintf(ListaInfo[1],"Modo no soportado");
							sprintf(ListaInfo[2],"Por su tarjeta");
							cdlg=new cdialogo(3,ListaInfo,0,0,k);
						}
						for (nlinea=0;nlinea<6;nlinea++)
							if (ListaInfo[nlinea])
								delete ListaInfo[nlinea];
						delcdlg();
					}
					break;
				}
			}
			break;
		}
	}
	cerrar_menus();
	/*
			cdlg=new cdialogo(4,lista,0,0,k);
			delcdlg();
			break;
	*/
}

void interfaz::cerrar_menus(void)
{
	int cont;
	for (cont=m_ac;cont>=0;--cont)
	{
		if (menuptr[cont]!=NULL)
		{
			delete menuptr[cont];
			menuptr[cont]=NULL;
		}
	}
	m_ac=0;
}

int interfaz::filtros(imagen *baseimg,char *baseimgname)
{
	imagen *newimg=NULL;
	char *filtros[]={"Filtro de Media ý",
					 "Filtro de Mediana ý",
					 "Filtro de Moda ý",
					 "Filtro de Usuario ý"};
	char *NoDisponible[]={"(No disponible)"};
	char *tamanos[]={"3x3","5x5","7x7"};
	int x,y,opcion,tamano;
	filtro *flt=NULL;
	cdialogo *pausa=NULL;
	int error;
	int filtrable=0;
	if (baseimg->getbitsperpixel()==24)
		filtrable=1;
	else
	if (baseimg->getbitsperpixel()==8)
	{
		if(baseimg->escala_de_grises!=0)
			filtrable=1;
		else
			filtrable=0;
	}
	int num;

	leeraton(&x,&y,&opcion);
	menuptr[m_ac++]=new menu(filtrable?4:1,filtrable?filtros:NoDisponible,
							x,y,k,&opcion);
	if (!filtrable)
	{
		if (opcion==0)
		{
			cerrar_menus();
			return (ILLEGAL_IMAGE_CONVERSION);
		}
		else
		{
			cerrar_menus();	//si no se ha pulsado sobre la opci¢n,
			return (OK);	//no mostrar mensaje de error;
		}
	}
	if ((opcion<0)||(opcion>3))
	{
		cerrar_menus();
		return (OK);
	}
	leeraton(&x,&y,&tamano);
	menuptr[m_ac++]=new menu(3,tamanos,x,y,k,&tamano);
	switch(tamano){
		case 0:{tamano=3;break;}
		case 1:{tamano=5;break;}
		case 2:{tamano=7;break;}
		default:
			cerrar_menus();
			return(OK);
	}
	switch(opcion)
	{
		case 0:
		{
			cerrar_menus();
			if (getfreepics())
			{
				flt=new filtro(tamano,baseimg,newimg);
				pausa=new cdialogo(0,0,k);
				if ((flt==NULL)||(pausa==NULL))
					error=MEM_ALLOC_ERROR;
				else
				{
					if ((error=flt->last_error)==OK)
					{
						error=flt->media();
					}
				}
				if (pausa!=NULL) delete pausa;
			}
			else error=ALL_ICONS_OPEN;
			break;
		}
		case 1:
		{
			cerrar_menus();
			if (getfreepics())
			{
				flt=new filtro(tamano,baseimg,newimg);
				pausa=new cdialogo(0,0,k);
				if ((flt==NULL)||(pausa==NULL))
					error=MEM_ALLOC_ERROR;
				else
				{
					if ((error=flt->last_error)==OK)
					{
						error=flt->mediana();
					}
				}
				if (pausa!=NULL) delete pausa;
			}
			else error=ALL_ICONS_OPEN;
			break;
		}
		case 2:
		{
			cerrar_menus();
			if (getfreepics())
			{
				flt=new filtro(tamano,baseimg,newimg);
				pausa=new cdialogo(0,0,k);
				if ((flt==NULL)||(pausa==NULL))
					error=MEM_ALLOC_ERROR;
				else
				{
					if ((error=flt->last_error)==OK)
					{
						error=flt->moda();
					}
				}
				if (pausa!=NULL) delete pausa;
			}
			else error=ALL_ICONS_OPEN;
			break;
		}
		case 3:
		{
			cerrar_menus();
			if (getfreepics())
			{
				mascara testmask;
//					int coef[9]={-1,-1,-1,-1,15,-1,-1,-1,-1};
//					mascara testmask={3,coef,7,0};
				flt=new filtro(tamano,baseimg,newimg);
				if (flt==NULL)
					error=MEM_ALLOC_ERROR;
				else
				{
					if ((error=flt->last_error)==OK)
					{
						cdlg=new cdialogo(0,0,k,tamano,&testmask);
						error=cdlg->last_error;
						delcdlg();
						if (error==OK)
						{
							pausa=new cdialogo(0,0,k);
							error=flt->usuario(&testmask);
						}
					}
				}
				if (pausa!=NULL) delete pausa;
			}
			else error=ALL_ICONS_OPEN;
			break;
		}
	}
	if (error==OK)
	{
		strcpy(aux_filename,baseimgname);
		anadir_tag('m',aux_filename);
		newpic(newimg,aux_filename);
	}
	if (flt!=NULL)
		delete flt;
	cerrar_menus();
	return(error);
}

int interfaz::BitPlanes(imagen *origen, char *baseimgname)
{
	imagen *newimg=NULL;
	int error;
	int Disponible=0;
	int x,y,plano;
	char *NoDisponible[]={"(No disponible)"};
	char *BitOpts[]={"Plano 7 (MSB)","Plano 6","Plano 5","Plano 4","Plano 3"
					,"Plano 2","Plano 1","Plano 0 (LSB)"};
	if (origen->getbitsperpixel()==8)
		if (origen->escala_de_grises)
			Disponible=1;
	leeraton(&x,&y,&plano);
	menuptr[m_ac++]=new menu(Disponible?8:1,Disponible?BitOpts:NoDisponible,
							x,y,k,&plano);
	cerrar_menus();
	if (!Disponible)
	{
		if (plano==0)
			return (ILLEGAL_IMAGE_CONVERSION);
		else
			return (OK);	//no mostrar mensaje de error;
	}
	if (plano==-1)
		return (OK);
	plano=7-plano;
	if ((plano<0)||(plano>7)) plano=7;
	error=GetBitPlane(origen,newimg,plano);
	if (error==OK)
	{
		strcpy(aux_filename,baseimgname);
		anadir_tag(plano+'0',aux_filename);
		newpic(newimg,aux_filename);
	}
	return error;
}

int interfaz::Quant(imagen *origen, char *baseimgname)
{
	imagen *newimg=NULL;
	int error;
	int Disponible=0;
	int x,y,NBitsQuant;
	char *NoDisponible[]={"(No disponible)"};
	char *BitOpts[]={"8 bits por pixel","7 bits por pixel","6 bits por pixel",
					"5 bits por pixel","4 bits por pixel","3 bits por pixel",
					"2 bits por pixel","1 bit por pixel"};
	if (origen->getbitsperpixel()==8)
		if (origen->escala_de_grises)
			Disponible=1;
	leeraton(&x,&y,&NBitsQuant);
	menuptr[m_ac++]=new menu(Disponible?8:1,Disponible?BitOpts:NoDisponible,
							x,y,k,&NBitsQuant);
	cerrar_menus();
	if (!Disponible)
	{
		if (NBitsQuant==0)
			return (ILLEGAL_IMAGE_CONVERSION);
		else
			return (OK);	//no mostrar mensaje de error;
	}
	if (NBitsQuant==-1)
		return (OK);
	NBitsQuant=8-NBitsQuant;
	if ((NBitsQuant<1)||(NBitsQuant>8)) NBitsQuant=8;
	error=QuantImg(origen,newimg,NBitsQuant);
	if (error==OK)
	{
		strcpy(aux_filename,baseimgname);
		anadir_tag(NBitsQuant+'0',aux_filename);
		newpic(newimg,aux_filename);
	}
	return error;
}

void interfaz::get_nombre(char *nombre,char *nombre_dos_8_3)
{
	int indice;
	for (indice=0;indice<8;indice++)
	{
		if (nombre_dos_8_3[indice]=='.')
			break;
		else
			nombre[indice]=nombre_dos_8_3[indice];
	}
	nombre[indice]=0;
}

void interfaz::get_extens(char *extens,char *nombre_dos_8_3)
{
	int indice;
	for (indice=0;indice<8;indice++)
	{
		nombre_dos_8_3++;
		if (*nombre_dos_8_3=='.')
			break;
	}
	strcpy(extens,nombre_dos_8_3);
}

void interfaz::anadir_tag(char tag,char *nombre)
{
	int cont;
	for (cont=6;cont>=0;cont--)
	{
		nombre[cont+2]=nombre[cont];
	}
	nombre[0]=tag;
	nombre[1]='_';
	nombre[8]=0;
}


int interfaz::convertir(imagen *baseimg,char *baseimgname)
{
	switch (baseimg->getbitsperpixel())
	{
		case 24:
		{
			int x,y,accion;
			char *opciones[]={"A 8 bits (color)","A 8 bits (grises)"};
			leeraton(&x,&y,&accion);
			menuptr[m_ac++]=new menu(2,opciones,x,y,k,&accion);
			cerrar_menus();
			switch(accion)
			{
				case 0:
				{
					procesar('c',baseimg,baseimgname,conv24_8);
					break;
				}
				case 1:
				{
					procesar('c',baseimg,baseimgname,conv24_8g);
					break;
				}
			}
			break;
		}
		case 8:
		{
			int x,y,accion,n_opciones=2;
			char *opciones1[]={"A 24 bits","A 8 bits (grises)"};
			char *opciones2[]={"A 24 bits","A 1 bit (Floyd-Steinberg)"};
			char **opciones = opciones1;
			if (baseimg->escala_de_grises)
				opciones = opciones2;
			leeraton(&x,&y,&accion);
			menuptr[m_ac++]=new menu(n_opciones,opciones,x,y,k,&accion);
			cerrar_menus();
			switch(accion)
			{
				case 0:
				{
					procesar('c',baseimg,baseimgname,conv8_24);
					break;
				}
				case 1:
				{
					if (!baseimg->escala_de_grises)
						procesar('c',baseimg,baseimgname,conv8_8g);
					else
						procesar('c',baseimg,baseimgname,conv8g_1fs);
						
					break;
				}
			}
			break;
		}
	}
	return(OK);
}

void interfaz::procesar(char tag,imagen *baseimg,char *baseimgname,
		int (*func)(imagen *,imagen *(&)))
{
	imagen *newimg;
	if (getfreepics())
	{
		int error;
		{
			cdlg=new cdialogo(0,0,k);
			error=func(baseimg,newimg);
			delcdlg();
		}
		if ((error)==OK)
		{
			strcpy(aux_filename,baseimgname);
			anadir_tag(tag,aux_filename);
			newpic(newimg,aux_filename);
		}
		else mostrar_error(error);
	}
	else mostrar_error(ALL_ICONS_OPEN);
}

long ahtol(char *hexstring)
{
	long ret=0;
	int cifra=0;
	char ch;
	int ncifras=strlen(hexstring);
	while(cifra<ncifras)
	{
		ch=hexstring[cifra++];
		if (isdigit(ch))
		{
			ret+=(ch-'0')<<(4*(ncifras-cifra));
		}
		else
		{
			if (isxdigit(ch))
			{
				ret+=(ch+10-'A')<<(4*(ncifras-cifra));
			}
			else return 0L;
		}
	}
	return ret;
}


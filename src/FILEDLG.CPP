#include <alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <bios.h>

#include "sbvperr.h"
#include "krpfont.h"
#include "mouse.h"
#include "boton.h"
#include "menu.h"
#include "listbox.h"
#include "xms_buf.h"
#include "cdialogo.h"
#include "filedlg.h"

int testdrive(int n_unidad)
{
	char *buffer=NULL;
	buffer=new char[512];
	if (buffer==NULL)
		return 1;
	if((n_unidad==0)||(n_unidad==1))
	{
		int result;
		result = biosdisk(4,n_unidad,0,0,0,1,buffer);
		result &= 0x02;
		delete buffer;
		if (result)
			return 1;
		else return 0;
	}
	delete buffer;
	return 1;
}

char get_letra_unidad(char *nombre_unidad)
{
	nombre_unidad[9]='A'+getdisk();
	return nombre_unidad[9];
}

void CuadroUnidadNoLista(krpfont *k)
{
	char *lista[3]={"Unidad no preparada",
					"Inserte un disquete en la unidad",
					"y vuelva a intentarlo"};
	cdialogo *UnidadNoLista=NULL;
	UnidadNoLista=new cdialogo(3,lista,0,0,k);
	if (UnidadNoLista!=NULL)
		delete UnidadNoLista;
}

filedialog::filedialog(char *titulo,char *filename,char *name,
	krpfont *krptextobject)
{
	char *extension={"Extensi¢n: BMP"};
	char *unidad={"Unidad: [C:]"};
	unsigned w_size;


	x=110,y=60,ancho=400,alto=320,altura_titulo=20;
	k=krptextobject;
	pathname=filename;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)calloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro();
	print_title(titulo);
	boton Aceptar(x+ancho-70,y+30,50,20,"Aceptar",k);
	boton Cancelar(x+ancho-70,y+60,50,20,"Cancelar",k);
//	boton ChExt(x+ancho-120,y+alto-120,100,20,"Cambiar...",k);
	boton ChUni(x+ancho-120,y+alto-70,100,20,"Cambiar...",k);
	listbox ext(&extension,100,x+ancho-120,y+alto-140,k);
	get_letra_unidad(unidad);
	listbox uni(&unidad,100,x+ancho-120,y+alto-90,k);
	k->kprintf("Archivos",x+30,y+30,BLACK);
	listbox arch(0,archivos,100,x+20,y+50,14,k);
	k->kprintf("Directorios",x+170,y+30,BLACK);
	listbox dir(0,archivos,100,x+150,y+50,14,k);
	listbox ruta(NULL,ancho-20,x+10,y+alto-30,k);
	muestra_cursor();
	leer_unidades();
	leer_archivos();
	arch.actualizar(n_archs,archivos);
	leer_directorios();
	dir.actualizar(n_dirs,directorios);
	get_pathname();
	ruta.actualizar(1,&pathname);
	while (1)
	{
		unsigned archn;
		unsigned dirn;
		if(dir.get_elemento(&dirn)==OK)
		{
			chdir(directorios[dirn]);
			liberar_directorios();
			liberar_archivos();
			leer_archivos();
			leer_directorios();
			arch.actualizar(n_archs,archivos);
			dir.actualizar(n_dirs,directorios);
			get_pathname();
			ruta.actualizar(1,&pathname);
		}

		if(arch.get_elemento(&archn)==OK)
		{
			get_pathname();
			strcat(pathname,archivos[archn]);
			strcpy(name,archivos[archn]);
			ruta.actualizar(1,&pathname);
		}
		if(Aceptar.activado())	//aceptar
		{
			if (filename[strlen(filename)-1]=='\\')
				last_error=FILE_NOT_SPECIFIED;
			else last_error=OK;
			break;
		}
		if(Cancelar.activado())
		{
			last_error=CANCELLED_OPERATION;
			break;
		}
/*		if(ChExt.activado())
		{
			char *lista[2]={"Funcion no Implementada",
							"S¢lo se soportan ficheros .BMP"};
			cdialogo memoria(2,lista,0,0,k);
		}*/
		if(ChUni.activado())
		{
			int x,y,n_unidad;
			char **lista;
			lista=new char *[n_unidades];
			if (lista==NULL) return;
			for (int cont=0;cont<n_unidades;cont ++)
			{
				lista[cont]=new char[10];
				if (lista[cont]==NULL) break;
				sprintf(lista[cont],"Unidad %c:",unidades[cont]+'A');
			}
			leeraton(&x,&y,&n_unidad);	//n_unidad es dummy
			menu (n_unidades,lista,x,y,k,&n_unidad);
			for (cont=0;cont<n_unidades;cont ++)
				delete (lista[cont]);
			delete (lista);
			if(testdrive(n_unidad))
			{
				if (n_unidad!=-1)
				if (!_chdrive(unidades[n_unidad]+1))
				{
					liberar_directorios();
					liberar_archivos();
					leer_archivos();
					leer_directorios();
					arch.actualizar(n_archs,archivos);
					dir.actualizar(n_dirs,directorios);
					unidad[9]='A'+n_unidad;
					uni.actualizar(1,&unidad);
					get_pathname();
					ruta.actualizar(1,&pathname);
				}
			}
			else
			{
				CuadroUnidadNoLista(k);
			}
		}
	}
	oculta_cursor();
	putimage(x,y,zona,0);
	muestra_cursor();
	liberar_directorios();
	liberar_archivos();
}

filedialog::filedialog(char *titulo,char *filename,
	krpfont *krptextobject)
{
	char *unidad={"Unidad: [C:]"};
	unsigned w_size;

	x=110,y=60,ancho=300,alto=320,altura_titulo=20;
	k=krptextobject;
	pathname=filename;
	w_size=imagesize(x,y,x+ancho,y+alto);
	if ((zona=(void far *)calloc(1,w_size))==NULL)
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	oculta_cursor();
	getimage(x,y,x+ancho,y+alto,zona);
	dibujar_cuadro();
	print_title(titulo);
	boton aceptar(x+ancho-90,y+50,50,20,"Aceptar",k);
	get_letra_unidad(unidad);
	listbox uni(&unidad,100,x+ancho-120,y+alto-90,k);
	boton chuni(x+ancho-120,y+alto-70,100,20,"Cambiar...",k);
	k->kprintf("Directorios",x+40,y+30,BLACK);
	listbox dir(0,archivos,100,x+20,y+50,14,k);
	listbox ruta(NULL,ancho-20,x+10,y+alto-30,k);
	muestra_cursor();
	leer_unidades();
	leer_directorios();
	dir.actualizar(n_dirs,directorios);
	get_pathname();
	ruta.actualizar(1,&pathname);
	while (1)
	{
		unsigned dirn;
		if(dir.get_elemento(&dirn)==OK)
		{
			chdir(directorios[dirn]);
			liberar_directorios();
			leer_directorios();
			dir.actualizar(n_dirs,directorios);
			get_pathname();
			ruta.actualizar(1,&pathname);
		}

		if(aceptar.activado())
		{
			if (filename[strlen(filename)-1]=='\\')
				filename[0]=0;
			break;
		}
		if(chuni.activado())
		{
			int x,y,n_unidad;
			char **lista;
			lista=new char *[n_unidades];
			if (lista==NULL) return;
			for (int cont=0;cont<n_unidades;cont ++)
			{
				lista[cont]=new char[10];
				if (lista[cont]==NULL) break;
				sprintf(lista[cont],"Unidad %c:",unidades[cont]+'A');
			}
			leeraton(&x,&y,&n_unidad);	//n_unidad es dummy
			menu (n_unidades,lista,x,y,k,&n_unidad);
			for (cont=0;cont<n_unidades;cont ++)
				delete (lista[cont]);
			delete (lista);
			if(testdrive(n_unidad))
			{
				if (n_unidad!=-1)
				if(!_chdrive(unidades[n_unidad]+1))
				{
					liberar_directorios();
					leer_directorios();
					unidad[9]='A'+n_unidad;
					uni.actualizar(1,&unidad);
					dir.actualizar(n_dirs,directorios);
					get_pathname();
					ruta.actualizar(1,&pathname);
				}
			}
			else
			{
				CuadroUnidadNoLista(k);
			}
		}
	}
	oculta_cursor();
	putimage(x,y,zona,0);
	muestra_cursor();
	liberar_directorios();
	last_error=OK;
}

filedialog::~filedialog()
{
	if (zona!=NULL)
		free(zona);
}


void filedialog::dibujar_cuadro(void)
{
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(x,y,x+ancho,y+alto);
	setfillstyle(SOLID_FILL,RED);
	bar(x+2,y+2,x+ancho-2,y+altura_titulo-2);
	setcolor(BLACK);
	moveto(x,y);
	linerel(ancho,0);
	linerel(0,alto);
	linerel(-ancho,0);
	linerel(0,-alto);
	setcolor(DARKGRAY);
	moveto(x+ancho-1,y+alto-1);
	linerel(0,2-alto);
	moveto(x+ancho-1,y+alto-1);
	linerel(2-ancho,0);
	setcolor(WHITE);
	moveto(x+1,y+1);
	linerel(ancho-2,0);
	moveto(x+1,y+1);
	linerel(0,alto-2);
}

void filedialog::print_title(char *texto)
{
	int cont;
	k->kprintf(texto,x+((ancho-k->longtexto(texto))/2),y+2,WHITE);
}

void filedialog::leer_archivos(void)
{
	ffblk archivo;
	n_archs=0;
	if (findfirst("*.bmp",&archivo,FA_ARCH)==0)
		n_archs++;
	else
		return;
	while (findnext (&archivo)==0)
		n_archs++;
	archivos=new char far *[n_archs];
//	archivos=(char far * far *)calloc(n_archs,sizeof(char far * far *));
	if (archivos==NULL) exit(0);
	for (int cont=0; cont<n_archs; cont++)
	{
		archivos[cont]=new char far [13];
//		archivos[cont]=(char far *)calloc(1,13);
		if (archivos[cont]==NULL) exit(0);
	}
	findfirst("*.bmp",&archivo,FA_ARCH);
	strcpy(archivos[0],archivo.ff_name);
	for (cont=1;cont<n_archs;cont++)
	{
		findnext(&archivo);
		strcpy(archivos[cont],archivo.ff_name);
	}
	ordenar(n_archs,archivos);
}

void filedialog::leer_directorios(void)
{
	ffblk archivo;
	n_dirs=0;
	if (findfirst("*.*",&archivo,FA_DIREC)==0)
		n_dirs++;
	else
		return;
	while (findnext (&archivo)==0)
	{
		if (archivo.ff_attrib==FA_DIREC)
			n_dirs++;
	}
	directorios=new char far * [n_dirs];
//	directorios=(char far * far *)calloc(n_dirs,sizeof(char far * far *));
	if (directorios==NULL) exit(0);
	for(int cont=0;cont<n_dirs;cont++)
	{
		directorios[cont]=new char far [13];
//		directorios[cont]=(char far *)calloc(1,13);
		if (directorios[cont]==NULL) exit(0);
	}
	findfirst("*.*",&archivo,FA_DIREC);
	strcpy(directorios[0],archivo.ff_name);
	for (cont=1;cont<n_dirs;cont++)
	{
		findnext(&archivo);
		while (archivo.ff_attrib!=FA_DIREC)
			findnext(&archivo);
		strcpy(directorios[cont],archivo.ff_name);
	}
	ordenar(n_dirs,directorios);
}

void filedialog::liberar_archivos(void)
{
	if (n_archs==0) return;
	for(int cont=0; cont<n_archs; cont++)
		delete archivos[cont];
//		free(archivos[cont]);
	delete archivos;
//	free(archivos);
}

void filedialog::liberar_directorios(void)
{
	if (n_dirs==0) return;
	for(int cont=0; cont<n_dirs; cont++)
		delete directorios[cont];
//		free(directorios[cont]);
	delete directorios;
//	free(directorios);
}

void filedialog::leer_unidades(void)
{
	unsigned char cont;
	unsigned char indice=0;
	unsigned char save;

	n_unidades=0;
	save=getdisk();
	for (cont=0;cont<26;cont++)
	{
		setdisk(cont);
		if (cont==getdisk())
		{
			unidades[indice]=cont;
			n_unidades++;
			indice++;
		}
	}
	setdisk(save);
}


void filedialog::get_pathname(void)
{
	char curdir[MAXDIR+1];
	char drive[]="?:\\";
	int drive_n;
	drive_n=getdisk();
	drive_n+=65;			//para obtener el car cter ASCII 'A'
	drive[0]=(char) drive_n;
	getcurdir (0,curdir);
	strcpy(pathname,drive);
	strcat(pathname,curdir);
	if (curdir[0]!=0)
		strcat(pathname,"\\");
}

void filedialog::ordenar(int num, char**lista)
{
	int indice,cambios=1;
	char aux[13];
	while ((cambios>0)&&(num>1))
	{
		cambios=0;
		for (indice=0;indice<num-1;indice++)
		{
			if (strcmp(lista[indice],lista[indice+1])>0)
			{
				strcpy(aux,lista[indice+1]);
				strcpy(lista[indice+1],lista[indice]);
				strcpy(lista[indice],aux);
				cambios++;
			}
		}
	}
}
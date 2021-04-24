/*Fichero SBVPMAIN.CPP*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <alloc.h>
#include <dir.h>

#include "xms_ctrl.h"
#include "sbvperr.h"
#include "interfaz.h"
#include "sbvpmain.h"

char ver[]="1.01";
char InitialDir[MAXDIR];
int InitialDrive;

int DebugMode=0;

void main(int argc,char *argv[])
{
	interfaz *gui;
	clrscr();
	printf("\nIniciando SBVP... por favor, espere");
	SaveInitialPath();
	atexit(RestoreInitialPath);
	if ((argc>1)&&(!strcmpi("/DEBUG",argv[1])))
	{
		DebugMode=1;
		printf("\nDebug Mode - Opciones de depuraci¢n activadas\n");
		printf("\nCoreleft():%ld",coreleft());
		printf("\nFarcoreleft():%ld\n",farcoreleft());
		printf("\nPulse una tecla para continuar...");
		getch();
	}
//	return;
	gui=new interfaz (ver);
	if (gui==NULL)
	{
		printf("\nError grave de asignaci¢n de memoria. Saliendo de SBVP...");
		return;
	}
	clrscr();
	if (gui->last_error==OK)
		gui->start();
	else
	{
		char *error_name;
		clrscr();
		printf("\nHa ocurrido un error en esta aplicaci¢n");
		printf("\nC¢digo del error: %x",gui->last_error);
		get_error(gui->last_error,error_name);
		printf("\nDescripci¢n: %s",error_name);
		printf("\nPor favor, comun¡queselo al autor del programa");
		printf("\n    junto con este n£mero de versi¢n: %s",ver);
		return;
	}
	printf("\nGracias por usar la versi¢n %s de SBVP en proyecto...",ver);
	if (gui!=NULL) delete gui;
}

void SaveInitialPath(void)
{
	InitialDrive=getdisk();
	if (getcurdir(0,InitialDir))	//getcurdir devuelve 0 cuando hay
		*InitialDir=0;				//un error
}

void RestoreInitialPath(void)
{
	setdisk(InitialDrive);
	chdir("\\");
	chdir(InitialDir);
}
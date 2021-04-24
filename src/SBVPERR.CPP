/*Fichero SBVPERR.CPP - Errores de programa*/

#include "sbvperr.h"
#include <string.h>
#include <stdio.h>

void get_error(int cod_error,char *(&error))
{
	switch(cod_error)
	{
		case OK:
			error="OK";
			break;
		case MEM_ALLOC_ERROR:
			error="Fallo en asignaci¢n de memoria";
			break;
		case FILE_OPEN_ERROR:
			error="Fallo en apertura de fichero";
			break;
		case FILE_NOT_SPECIFIED:
			error="No se ha especificado ning£n fichero";
			break;
		case BMP_NOT_BMP:
			error="El fichero no es un BMP v lido";
			break;
		case BMP_FILE_ERROR:
			error="Fichero BMP da¤ado";
			break;
		case BMP_RLE_NOT_SUPPORTED:
			error="Compresi¢n RLE no soportada";
			break;
		case VESA_NOT_SUPPORTED:
			error="La tarjeta gr fica no soporta VESA";
			break;
		case VESA_VBE12_NOT_SUPP:
			error="La tarjeta gr fica no soporta VESA 1.2";
			break;
		case VESA_STATUS_ERROR:
			error="VESA: Error en la operaci¢n";
			break;
		case VESA_REQ_MODE_NOT_SUPPORTED:
			error="VESA: Modo requerido no soportado";
			break;
		case XMS_NOT_INSTALLED:
			error="No hay ning£n driver XMS instalado";
			break;
		case XMS_FUNC_NOT_IMPLEMENTED:
			error="XMS: Funci¢n no implementada";
			break;
		case 0x0081:
			error="XMS: Detectado dispositivo VDISK";
			break;
		case 0x00A0:
			error="Insuficiente memoria XMS libre";
			break;
		case 0x00A1:
			error="No hay suficientes 'handles' para XMS";
			break;
		case 0x00A2:
			error="Handle XMS no v lido";
			break;
		case 0x00A3:
			error="XMS: SourceHandle no v lido";
			break;
		case 0x00A4:
			error="XMS: SourceOffset no v lido";
			break;
		case 0x00A5:
			error="XMS: DestHandle no v lido";
			break;
		case 0x00A6:
			error="XMS: DestOffset no v lido";
			break;
		case 0x00A7:
			error="XMS: Length no v lida";
			break;
		case 0x00A8:
			error="XMS: Overlap no v lido";
			break;
		case 0x00A9:
			error="XMS: Error de paridad";
			break;
		case FONT_FILE_NOT_FOUND:
			error="Tipo de letra (Kresp32.fon) no hallado";
			break;
		case MOUSE_NOT_INSTALLED:
			error="Rat¢n no instalado";
			break;
		case BGI_DRIVER_ERROR:
			error="Error en Driver BGI";
			break;
		case ALL_ICONS_OPEN:
			error="No se pueden abrir m s im genes";
			break;
		case FUNC_NOT_IMPL:
			error="Funci¢n no implementada";
			break;
		case ILLEGAL_BITS_PER_PIXEL:
			error="N£mero no permitido de bits/pixel en la operaci¢n";
			break;
		case ILLEGAL_IMAGE_CONVERSION:
			error="Conversi¢n de imagen necesaria para la operaci¢n";
			break;
		case USER_MASK_SIZE_MISMATCH:
			error="No coincide tama¤o de m scara de usuario";
			break;
		case DIVIDE_BY_ZERO:
			error="Divisi¢n por cero";
			break;
		case CANCELLED_OPERATION:
			error="Operaci¢n cancelada";
			break;
		case HEAP_CORRUPT:
			error="­HEAP CORRUPTO! ­SALGA DEL PROGRAMA!";
			break;
		case NOT_8BITS_GRAYSCALE:
			error="Es necesaria una imagen de 8 bits (escala de grises)";
			break;
		default:
		{
			char *nerror="XXXX ";
			char *aux;
			sprintf(nerror,"%4X",cod_error);
			error="C¢digo de error n§ XXXX ";
			aux=error+19;
			while(*nerror) *aux++ = *nerror++;
			break;
		}
	}
}
/*
		case :
		{
			strcpy(error,"");
			break;
		}
*/

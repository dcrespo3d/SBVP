class xmscontrol;
class imagen;
class vesa_video_subsystem;
class boton;
class menu;
class krpfont;

class interfaz
{
	#define N_ICONOS 20
	#define N_MAX_MENUS 10
	private:
	struct icono_imagen{
		char nombre[13];
		int valido;
		int x,y;
		boton *bot;
		imagen *img;
	}pic[N_ICONOS];
	boton *BarraBoton[6];
	menu *menuptr[N_MAX_MENUS];
	int m_ac;
	char aux_filename[20];
	krpfont *k;
	xmscontrol *xms;
	vesa_video_subsystem *vesa;
	char *ver;
	void far *logo_fondo;
	void far *logo_icono;
//	cdialogo *cdlg;

	void delcdlg(void);
	int inigraf(void);
	void fingraf(void);
	int cargar_logo(void far *(&),char *);
	void errores_iniciales(void);
	void ventana_general(void);
	void getpiccoords(int,int *,int *);
//	int tecla_pulsada(void);
    int getfreepics(void);
	int getpicnum(void);
	int newpic(imagen *,char *);
	void mostrar_error(int);
	void mostrar_memoria_libre(void);
	void mostrar_info_vesa(void);
	void mostrar_heap(void);
	int convertir(imagen*, char *);
	int filtros(imagen *,char *);
	int BitPlanes(imagen *origen,char *);
	int Quant(imagen *origen,char *);
	void get_nombre(char *nombre,char *nombre_dos_8_3);
	void get_extens(char *extens,char *nombre_dos_8_3);
	void anadir_tag(char tag,char *nombre);
	void cerrar_menus(void);
	void procesar(char tag,imagen *,char *,
		int (*func)(imagen *,imagen *(&)));
	void ActualizarBotones(void);
	int HeapOk(void);

	public:
	int last_error;
	interfaz(char *);
	int start(void);
};

long ahtol(char *);	//convierte cadena hex en long;

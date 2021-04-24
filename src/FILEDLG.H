class boton;
class listbox;

class filedialog
{
	private:
	int x,y,ancho,alto,altura_titulo;
	void far *zona;
	char far * far *archivos,far * far *directorios;
	char far *pathname;
	char unidades[30];
	unsigned n_archs,n_dirs,n_unidades;
	krpfont *k;
	void dibujar_cuadro(void);
	void print_title(char *texto);
	void leer_archivos(void);
	void leer_directorios(void);
	void liberar_archivos(void);
	void liberar_directorios(void);
	void leer_unidades(void);
	void ordenar(int num,char **lista);
	void get_pathname(void);

	public:
	int last_error;
	filedialog(char *titulo,char *filename,char *name,
		krpfont *krptextobject);
	filedialog(char *titulo,char *filename,
		krpfont *krptextobject);
	~filedialog();
};


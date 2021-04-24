class listbox
{
	private:
	int alto_de_fila;	//alto de cada fila
	int x1,y1,x2,y2;	//coordenadas de la listbox
	int elementos_totales,elementos_mostrados;
	int n_filas;
	int prim_elem_mostrado;
	int scroll_permitido;
	int ult_op,op;

	char far **texto;
	krpfont *k;
	boton *up,*down;
	void dibujar(void);
	void init_botones(void);
	void dibujar_marco();
	void mostrar_texto(void);
	void marcar (int opcion);
	int scrollup(void);
	int scrolldown(void);

	public:
	listbox(int argc,char *argv[],int ancho,int x,int y,
		int elem_a_mostrar,krpfont *krptextobject);
	listbox(char *argv[],int ancho,int x,int y,krpfont *krptextobject);
	void actualizar(int argc,char *argv[]);
	int get_elemento(unsigned *argout);
	int get_AltoDeFila(void);
	~listbox(void);
};

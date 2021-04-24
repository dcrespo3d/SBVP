class menu{
	private:
	krpfont *k;
	int x,y;
	void far *zona;			//zona a preservar
	unsigned altura_opcion;
	unsigned calcular_long_max(int argc,char *argv[]);
	void ajustar_posicion(unsigned ancho,unsigned alto);
	void dibujar_recuadro(unsigned dx,unsigned dy);
	int opcion_sobre(unsigned ancho,int *opcion);
	void print_text(int argc,char *argv[],unsigned ancho);
	void marcar(int opcion,unsigned ancho);

	public:
	int last_error;
	menu(int argc,char *argv[],unsigned x_in,unsigned y_in,
		krpfont *krptextobject,int *argout);
	~menu();
};

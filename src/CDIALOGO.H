struct mascara;
class krpfont;

class cdialogo
{
	private:
	unsigned altura_linea;
	krpfont *k;
	void far *zona;
	unsigned x,y;
	void dibujar_cuadro(unsigned dx,unsigned dy);
	unsigned calcular_long_max(int argc,char *argv[]);
	void ajustar_posicion(unsigned ancho,unsigned alto);
	void print_text(int argc,char *argv[],unsigned ancho);
	void centrar(unsigned ancho,unsigned alto);
	int existe(char *);

	public:
	int last_error;
	cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
		krpfont *krptextobject);	//cuadro "aceptar"
	cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
		krpfont *krptextobject,int *respuesta);  //cuadro "acept/cancel"
	cdialogo(unsigned x_in,unsigned y_in,krpfont *krptextobject);
		// cuadro "espere, por favor"
	cdialogo(unsigned x_in,unsigned y_in,krpfont *krptextobject,
		char *respuesta,char *extension);
			//cuadro "introduzca nombre"
	cdialogo(unsigned x_in,unsigned y_in,krpfont *krptextobject,
		char cAnchoMaskIn,mascara *pMaskIn);
	cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
					krpfont *krptextobject,long *respuesta);
	cdialogo(int argc,char *argv[],unsigned x_in,unsigned y_in,
					krpfont *krptextobject,char *respuesta);

	cdialogo::~cdialogo();
};

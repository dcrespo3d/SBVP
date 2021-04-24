class xmsbuffer;

class imagen	//declaraci¢n de la clase "imagen"
{
	public:
	struct regpal{			//registro de paleta
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char y;
	};
	regpal *paleta;			//puntero (array) de registros de paleta


	private:
	unsigned xsize,ysize;	//tama¤o de la imagen
	unsigned char bitsperpixel; //8 o 24

	xmsbuffer *cuerpo;		//cuerpo (datos) de la imagen

	public:
	int last_error;
	int escala_de_grises;	//1 si la imagen es en escala de grises,
							//0 en caso contrario.
	char nombre[13];
	unsigned getxsize(void);	//obtener ancho
	unsigned getysize(void);	//obtener alto
	unsigned char getbitsperpixel(void);	//obtener bpp
	int escribir(unsigned char *origen,unsigned long pos,
		unsigned long nbytes);
		//escribir a la imagen una serie de datos

	int leer(unsigned char *destino,unsigned long pos,
		unsigned long nbytes);
		//leer de la imagen una serie de datos

	int leer_paleta(regpal *destino);
		//leer paleta completa sobre paleta destino

	int escribir_paleta(regpal *origen);
		//escribir paleta completa desde paleta origen

	int leer_registro_paleta(unsigned char nreg,unsigned char *r,
		unsigned char *g,unsigned char *b,unsigned char *y);
		//leer un registro desde paleta sobre *r, *g, *b e *y

	int escribir_registro_paleta(unsigned char nreg,unsigned char *r,
		unsigned char *g,unsigned char *b,unsigned char *y);
		//leer un registro desde *r, *g, *b, e *y hasta la paleta.

	void crear_paleta_gris(void);		//crear una paleta gris, nueva
	void calcular_paleta_gris(void);	//calcular sobre (paleta.y) el
		//valor de luminancia a partir de y = 0.3 r + 0.59 g + 0.11 b

	void TestGrayScale(void);
		//comprobar la paleta de la imagen para ver si es escala de grises.

	imagen(unsigned x,unsigned y,unsigned char bpp);
	~imagen();
};

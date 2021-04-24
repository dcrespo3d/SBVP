class xmsbuffer;
class imagen;

class bmp
{
	private:
	FILE *bmpfile;
	unsigned long real_x;		//no. de pixels horizontales en fichero
	unsigned long real_y;		//no. de pixels verticales en fichero
	unsigned long real_size;	//tama¤o real del cuerpo de la imagen
	unsigned long total_size;	//tama¤o real del total de la imagen
	xmsbuffer *bmpbuffer;
	struct{
		unsigned bftype;			//signatura:"BM"
		unsigned long bfsize;		//tama¤o de la imagen (poco fiable)
		unsigned bfreserved1,bfreserved2;	//reservados
		unsigned long bfoffbits;	//offset de la cabecera al cuerpo
		unsigned long bisize;		//tama¤o de la cabecera (40)
		unsigned long biwidth;		//alto de la imagen
		unsigned long biheight;		//alto de la imagen
		unsigned biplanes;			//no. de planos de bits
		unsigned bibitcount;		//no. de bits por pixel
		unsigned long bicompres;	//flag de compresi¢n (0=no)
		unsigned long bisizimage;	//tama¤o del cuerpo de la imagen
		unsigned long bixpelsxmet,biypelsxmet;	//no usados por SBVP
		unsigned long biclrused,biclrimport;	//no usados por SBVP
	}bmpheader;

	struct regpal{
		unsigned char r;	//rojo
		unsigned char g;	//verde
		unsigned char b;	//azul
		unsigned char y;	//luminancia
	};

	unsigned paleta_ncolores;	//no. de colores en la paleta
	void getrealdata(void);
	unsigned imgbpp;			//solo creamos imagenes de 8 o 24 bits
	int decode24(imagen *);
	int decode8(imagen *);
	int decode4(imagen *);
	int decode1(imagen *);
	int encode24(imagen *);
	int encode8(imagen *);
	int leer_paleta(imagen *);
	int escribir_paleta(imagen *);

	public:
	int last_error;
	int leer_de_fichero(char *,imagen *(&));
	int escribir_a_fichero(char *,imagen *);
	bmp();
	~bmp();
};

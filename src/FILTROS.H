struct mascara
{
	int ancho;    //3,5 o 7
	int *coef;	//coeficientes de la mascara
	int factor;   //factor entre el que se divide el total
	int offset;			//offset que se le suma al total (255/-255)
};

class imagen;

class filtro
{
	private:
	imagen *imgin,*imgout;
	mascara *pMask;	//puntero a estructura m scara
	int nAnchoMask;		//3x3,5x5,7x7
	int nOffsetMask;	//1,  2,  3
	int cont;
	int BorrarDestino;
	unsigned char *il[7];
	unsigned char *il_aux,*ol;
	unsigned char *t_mn,*t_mnr,*t_mng,*t_mnb;	//tabla para calculo de mediana
	int error;
	int ancho,alto,bpp;
	int media8(void);
	int media24(void);
	int mediana8(void);
	int mediana24(void);
	int moda8(void);
	int moda24(void);
	int usuario8(void);
	int usuario24(void);
	unsigned char mediana(unsigned char *,unsigned char);
	void rellenar_bordes8(unsigned char *);
	void rellenar_bordes24(unsigned char *);
	long limx(long);

	public:
	int last_error;
	filtro(int,imagen *,imagen *(&));
	int media(void);
	int mediana(void);
	int moda(void);
	int usuario(mascara *);
	~filtro();
};

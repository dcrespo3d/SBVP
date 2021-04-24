class media
{
	private:
	imagen *imgin,*imgout;
	int ancho_mask;		//3x3,5x5,7x7
	int offset_mask;	//1,  2,  3
	int cont;
	unsigned char *il[7];
	unsigned char *il_aux,*ol;
	int error;
	int ancho,alto,bpp;
	int media8(void);
	int media24(void);
	void rellenar_bordes8(unsigned char *);
	void rellenar_bordes24(unsigned char *);
	long limx(long);

	public:
	int last_error;
	media(int,imagen *,imagen *(&));
	~media();
};

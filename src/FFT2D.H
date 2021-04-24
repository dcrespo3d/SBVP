class fft2d
{
	private:
	int nAlto,nAncho;
	int nAltoComp,nAnchoComp;
	int Nx,Ny;
	int BorrarDestino;	//flag que est  a 1 si ha ocurrido un error en
						//la generaci¢n de la imagen. Su prop¢sito es
						//borrar la imagen de salida cuando no es v lida.
	xmsbuffer *cuerpo1,*cuerpo2;
	fft *fft1d_X,*fft1d_Y;
	imagen *origen,*destino;
	complejo *CompLinea;
	complejo *CompLineaAux;
	unsigned char *UCLinea;
	void RoundP2(int &,int &);	//redondear a potencia de 2
	void LineaUCharToComplejo(int,int);
	void LineaComplejoToUChar(int);
	float Modulo (complejo);

	public:
	int last_error;
	fft2d(imagen *,imagen *(&));
	int CalcFFT(void);
	~fft2d();
};


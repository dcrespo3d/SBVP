class imagen;
class krpfont;

class histogram
{
	private:

	unsigned x,y;
	unsigned dx,dy;
	unsigned altura_linea;
	krpfont *k;
	long *histo_data;
	long maximo;
	long TotalPixels;
	void dibujar_cuadro(void);
	void print_text (char *);
	void centrar(void);
	int CalcHistoGray(imagen *);
	int CalcHistoPal(imagen *);
	int CalcHistoRGB(imagen *);
	int AdjustHistoGray(imagen *,imagen *,unsigned char *);
	void ActualizarHisto(int);

	public:
	int last_error;
	histogram();
	int ShowHistogram(imagen *,krpfont *);
	int EqualizeHisto(imagen *,imagen *(&));
	int EqualizeHistoCorr(imagen *,imagen *(&));
	histogram(imagen *,krpfont *);
	histogram(imagen *,imagen *(&));
	~histogram();
};


class krpfont
{
	private:
	unsigned *buffer_fuente;
	unsigned maxx,maxy;
	void inicializar_fuente();
	void liberar_fuente();

	public:
	unsigned last_error;
	void kprintf(const unsigned char *texto,unsigned x_inicial,
		unsigned y_inicial,unsigned char color);
	unsigned longtexto(const unsigned char *texto);
	unsigned getmaxx(void);
	void setmaxx(unsigned);
	krpfont();
	~krpfont();
};

class fft
{
	private:
	unsigned *orden;	//tabla precalculada de ordenaci¢n
	complejo *pre_expj;	//tabla precal. de expon. complejas
	unsigned char N;
	unsigned n_elem;
	void calc_orden(void);	//precalcular tabla de ordenaci¢n
	void calc_expj(void);	//precalcular expon. complejas
	void ordenar(complejo *vect_in,complejo *vect_out);
	void aj_amplitud(complejo *vect);
	public:
	int last_error;
	fft(unsigned char N_in);
    ~fft();
	void calc_fft(complejo *vect_in,complejo *vect_out);
};
class krpfont;

class boton
{
	private:
	int activacion;
	int existeicono;
	void far *icono;
	krpfont *ktext;
	unsigned char activo,pulsado,permiso_local,permiso_inicial;
	static int permiso_global;
	char *texto;
	unsigned x1,x2,y1,y2,centrox,centroy;
	void dibujar_suelto(void);
	void dibujar_pulsado(void);
	unsigned char sobre(unsigned x,unsigned y);
	void boton::procesar_raton_pulsado(unsigned x,unsigned y);
	void boton::procesar_raton_suelto(unsigned x,unsigned y);

	public:
	void actualizar(void);
	int activado(void);
	boton(unsigned posx,unsigned posy,unsigned tamx,unsigned tamy,
			char *texto_boton,krpfont *ktextobject);
	boton(unsigned posx,unsigned posy,char *texto_boton,
			krpfont *ktextobject,void far *icono_in);
};


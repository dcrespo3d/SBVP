class xmsbuffer
{
	public:
	xmsbuffer(unsigned nkbytes);
	~xmsbuffer();
	int last_error;
	int leer(unsigned char far *destino,unsigned long pos,
		unsigned long nbytes);
		//leer desde memoria extendida hasta un buffer (destino)

	int escribir(unsigned char *origen,unsigned long pos,
		unsigned long nbytes);
		//leer hacia memoria extendida desde un buffer (origen)


	private:

	int reserva(unsigned nkbytes);		//reservar memoria
	int libera(void);					//liberar memoria
	unsigned int GetBiggestBlock(void);
	unsigned int GetFreeMem(void);
	unsigned handle;	//identificador del buffer XMS
	struct _movestruct{					//datos para la
		unsigned long length;		//transferencia
		unsigned sourcehandle;		//de bloques de
		unsigned long sourceoffset;	//memoria XMS a
		unsigned desthandle;		//convencional
		unsigned long destoffset;
	}far *movestruct;
};

extern void far *xmsfunc;
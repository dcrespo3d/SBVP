class xmscontrol
{
	public:
	int last_error;
	xmscontrol(void);			//constructor, inicializa
								//gesti¢n de memoria XMS
	unsigned getfree(void);		//obtener memoria libre
	unsigned getversion(void);	//obtener versi¢n XMS
};

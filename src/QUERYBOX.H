class listbox;

class querybox
{
	private:
	listbox *lb;
	char *texto;
	char *textout;
	int l_text;
	int x,y,dx,dy;

	public:
	int last_error;
	querybox(int x,int y,int dx,int nmaxch,char *respuesta,krpfont *krp);
	~querybox(void);
	int start(void);
	int sobre(void);
	void actualizar(char *);
};

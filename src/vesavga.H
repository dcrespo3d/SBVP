class vesa_video_subsystem
{
	public:
	int last_error;
	int show(imagen *);
	vesa_video_subsystem();

	private:

	int start(void);
	int initmode(unsigned mode);
	int setmode(unsigned mode);
	int getmode(unsigned *mode);
	int vesa_video_subsystem::ponpaleta(imagen *palimg);
};

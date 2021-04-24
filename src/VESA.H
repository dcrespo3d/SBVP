class imagen;

class vesa_video_subsystem
{
	public:
	int last_error;
	int show(imagen *);
	vesa_video_subsystem();
	~vesa_video_subsystem();
	char *GetVerNumber(void);
	char *GetVendorName(void);
	unsigned *GetModeList(void);
	int GetModeInfo(unsigned,char *[6]);

	private:
	struct vesainfoblock{
		unsigned char vesa_signature[4];
		unsigned vesa_version;
		char far *oemstringptr;
		unsigned char capabilities[4];
		unsigned far *videomodeptr;
		unsigned totalmemory;
		unsigned char reserved[236];
		}*vib;
	struct modeinfoblock {
		unsigned modeattributes;
		unsigned char winaattributes;
		unsigned char winbattributes;
		unsigned wingranularity;
		unsigned winsize;
		unsigned winasegment;
		unsigned winbsegment;
		unsigned char far *winfuncptr;
		unsigned bytesperscanline;

		unsigned xresolution;
		unsigned yresolution;
		unsigned char xcharsize;
		unsigned char ycharsize;
		unsigned char numberofplanes;
		unsigned char bitsperpixel;
		unsigned char numberofbanks;
		unsigned char memorymodel;
		unsigned char banksize;
		unsigned char numberofimagepages;
		unsigned char reserved1;

		unsigned char redmasksize;
		unsigned char redfieldposition;
		unsigned char greenmasksize;
		unsigned char greenfieldposition;
		unsigned char bluemasksize;
		unsigned char bluefieldposition;
		unsigned char rsvdmasksize;
		unsigned char directcolormodeinfo;
		unsigned char reserved[216];
	}*mib;

	unsigned initial_mode;
	int getvesainfoblock(vesainfoblock far *ib);
	int getmodeinfoblock(unsigned modo,modeinfoblock far *ib);
	int start(void);
	int initmode(unsigned mode);
	int setmode(unsigned mode);
	int getmode(unsigned *mode);
	int ShowVGA13h(imagen *);
	int ponpaleta(imagen *palimg);
	void setbank(int );
	unsigned setreqmode(unsigned x,unsigned y,unsigned char bpp);
	char far *GetSVGABaseDir(void);

	public:

};

#include <_null.h>
#include "complejo.h"
#include "sbvperr.h"
#include "fftclass.h"

fft::fft(unsigned char N_in)
{
	orden=NULL;
	pre_expj=NULL;
	N=N_in;
	n_elem=1;
	n_elem<<=N;
	orden=new unsigned[n_elem];
	pre_expj=new complejo[n_elem>>1];

	if ((orden==NULL)||(pre_expj==NULL))
	{
		last_error=MEM_ALLOC_ERROR;
		return;
	}
	calc_orden();
	calc_expj();
	last_error=0;
}

void fft::calc_orden()
{
	unsigned nbit;
	unsigned ndato;
	unsigned dato;
	unsigned valOr;
	unsigned *v_write=orden;

	for(ndato=0;ndato<n_elem;ndato++)
	{
		dato=0;
		for(nbit=0;nbit<N;nbit++)
		{
			valOr=1<<nbit;
			if (ndato&valOr)
				valOr=1<<(N-nbit-1);
			else
				valOr=0;
			dato|=valOr;
		}
		*v_write++=dato;
	}
}

/*
void fft::calc_orden()
{
	unsigned espaciado;
	unsigned comienzo;
	unsigned n_datos;
	unsigned cont;
	unsigned interm;
	unsigned *v_read,*v_write;
	espaciado=n_elem;
	comienzo=espaciado;
	comienzo>>=1;
	n_datos=1;
	*orden=0;

	while(comienzo)
	{
		v_read=orden;
		v_write=orden;
		v_write+=comienzo;
		for(cont=0;cont<n_datos;cont++)
		{
			*v_write=*v_read+n_datos;
			v_read+=espaciado;
			v_write+=espaciado;
		}
		espaciado>>=1;
		comienzo>>=1;
		n_datos<<=1;
	}
}
*/

void fft::calc_expj(void)
{
	unsigned cont,contmax=n_elem>>1;
	float arg;
	complejo *tabla;
	tabla=pre_expj;
	for (cont=0;cont<contmax;cont++)
	{
		arg=(PI*cont)/contmax;	//(2*pi*cont/N)
		*(tabla++)=expj(arg);
	}
}

void fft::ordenar(complejo *vect_in,complejo *vect_out)
{
	unsigned cont;
	unsigned n_elementos=n_elem;
	unsigned *v_aux;
	v_aux=orden;
	long *f_read0,*f_read,*f_write;
	f_write=(long *)vect_out;
	f_read0=(long *)vect_in;
	for(cont=0;cont<n_elementos;cont++)
	{
		f_read=f_read0+(*(v_aux++)<<1);
		*(f_write++)=*(f_read++);
		*(f_write++)=*(f_read);
	}
}

void fft::calc_fft(complejo *vect_in,complejo *vect_out)
{
	unsigned char n_etapa;
	unsigned partes;	//numero de partes
	unsigned epp;		//elementos por parte;
	unsigned cp;		//contador de partes;
	unsigned ce;		//contador de elementos;
	unsigned a;			//indice del elemento a de la celda;
	unsigned b;			//indice del elemento b de la celda;
	complejo interma,intermb;	//elementos intermedios a y b;
	unsigned wn;		//exponencial WsubN;
	complejo *v_aux;

	ordenar(vect_in,vect_out);
	v_aux=vect_out;
	partes=n_elem>>1;
	epp=1;
	for (n_etapa=0;n_etapa<N;n_etapa++)
	{
		for (cp=0;cp<partes;cp++)
		{
			a=cp*epp*2;
			b=a+epp;
			wn=0;
			for (ce=0;ce<epp;ce++)
			{
				{
					intermb=v_aux[b] * pre_expj[wn];
					interma=v_aux[a];
					v_aux[a]=interma+intermb;
					v_aux[b]=interma-intermb;
				}
				a++;
				b++;
				wn+=partes;
			}
		}
		partes>>=1;
		epp<<=1;
	}
	aj_amplitud(vect_out);
}

void fft::aj_amplitud(complejo *vect)
{
	float f_nelem=n_elem;
	unsigned cont,cmax=n_elem;
	complejo *v_aux;
	v_aux=vect;
	for (cont=0;cont<cmax;cont++)
	{
		v_aux->re /= f_nelem;
		v_aux->im /= f_nelem;
		v_aux++;
	}
}

fft::~fft()
{
	if (pre_expj!=NULL)
		delete pre_expj;
	if (orden!=NULL)
		delete orden;
}

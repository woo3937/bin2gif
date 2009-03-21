#include <cstdio>
#include <cstdlib>
#include <complex>
#include <algorithm>
using namespace std;
//---------------------------------------------------------------------------
#include "util_visualize.h"
#include "util_fs.h"
//---------------------------------------------------------------------------
namespace sns
{
    namespace visual
    {
		double real(const complex<double> &c)
		{
			return c.real();
		}
		
		double imag(const complex<double> &c)
		{
			return c.imag();
		}
		
		enum binary_file_type {
			t_double,
			t_complex_double
		};
	
		gdImagePtr get_image_from_binary_file(char* filename, bin2gif_parameters p_parameters)
		{
			gdImagePtr im;
			FILE* fp = NULL;

			int i = 0, j = 0, ii = 0, jj = 0, kk = 0;
			int n = 0, factor = 1;
			
			double (*func)(const complex<double>&) = norm;
			
			switch(p_parameters.to_type)
			{
				case 'n':
					func = norm; break;
				case 'a':
					func = abs; break;
				case 'r':
					func = real; break;
				case 'i':
					func = imag; break;
				case 'p':
					func = arg; break;
			}
			
			// Determine file type and image size {{{
			
			binary_file_type file_type = t_complex_double;
			off_t file_size = util::file_size(filename) - p_parameters.header - p_parameters.footer, elements_in_file;
			
			if( file_size <= 0 ) {
				printf("Error: Cannot determine file size.\n");
				return 0;
			}
			
			elements_in_file = file_size / sizeof(complex<double>);
			n = (off_t)(sqrt((double)elements_in_file));
			if ( elements_in_file != n*n ) {
				file_type = t_double;
				elements_in_file = file_size / sizeof(double);
				n = (off_t)(sqrt((double)elements_in_file));
			}
			
			if ( n < p_parameters.to_size ) {
				p_parameters.to_size = n;
			}
			
			factor = n/p_parameters.to_size;
			
			void *data;

			if ( file_type == t_complex_double ) {
				data = new complex<double>[n*n];
			} else {
				data = new double[n*n];
			}
			
			if ( !data ) {
				printf("Error: Cannot allocate memory for data.");
				return 0;
			}

			fp = fopen(filename, "r");
			
			if ( !fp ) {
				printf("Error: Cannot open input file %s  for reading.", filename);
				delete[] data;
				return 0;
			}
			
			fseek(fp, p_parameters.header, SEEK_SET);
			
			if ( file_type == t_complex_double ) {
				elements_in_file = fread(data, sizeof(complex<double>), n*n, fp);
			} else {
				elements_in_file = fread(data, sizeof(double), n*n, fp);
			}
			
			if ( elements_in_file != n*n ) {
				printf("Error: Bad file format or corrupted file.");
				delete[] data;
				fclose(fp);
				return 0;
			}
			
			fclose(fp);

			double *ddata = new double[p_parameters.to_size*p_parameters.to_size];
			
			if ( !ddata ) {
				printf("Error: Cannot allocate memory for data.");
				delete[] data;
				return 0;
			}

			im = gdImageCreateTrueColor(p_parameters.to_size, p_parameters.to_size);

			if ( !im ) {
				printf("Error: Cannot create image.");
				delete[] data;
				delete[] ddata;
				return 0;
			}

			double d_value = 0;

			#pragma omp parallel for private(d_value,j,ii,jj) shared(ddata)
			for ( i = 0; i < p_parameters.to_size; i++ ) {
				for ( j = 0; j < p_parameters.to_size; j++ ) {

					d_value = 0;

					for ( ii = 0; ii < factor; ii++ ) {
						for ( jj = 0; jj < factor; jj++ ) {
							kk = n*(factor*i+ii)+(factor*j+jj);
							if ( file_type == t_complex_double ) {
								d_value += func(static_cast<complex<double>*>(data)[kk]);
							} else {
								d_value += static_cast<double*>(data)[kk];
							}
						}
					}

					ddata[p_parameters.to_size*i+j] = d_value/factor/factor;

				}
			}

			delete[] data;

			int c_black = gdImageColorAllocate(im, 0, 0, 0);
			int c_white = gdImageColorAllocate(im, 255, 255, 255);
			int c_color;

			double d_max, d_min;
			
			if ( p_parameters.to_type == 'p' ) {
				d_min = -M_PI;
				d_max = M_PI;
			} else if ( p_parameters.to_amp < 0 ) {
				d_max = *max_element(ddata, ddata + p_parameters.to_size*p_parameters.to_size);
				d_min = *min_element(ddata, ddata + p_parameters.to_size*p_parameters.to_size);
			} else {
				d_max = p_parameters.to_amp;
				d_min = ( p_parameters.to_type == 'n' || p_parameters.to_type == 'a' ) ? 0 : -p_parameters.to_amp;
			}

			for ( i = 0; i < p_parameters.to_size; i++ ) {
				for ( j = 0; j < p_parameters.to_size; j++ ) {

					c_color = (int)(255*(ddata[p_parameters.to_size*i+j]-d_min)/(d_max-d_min));
					c_color = ( c_color > 255 ) ? 255 : ( (c_color < 0) ? 0 : c_color  );
					c_color = gdImageColorAllocate(im, c_color, c_color, c_color);
					gdImageSetPixel(im, i, j, c_color );

				}
			}

			delete[] ddata;

			return im;
		}

		int convert_binary_file_to_gif(char* filename_bin, char* filename_gif, bin2gif_parameters p_parameters)
		{
			gdImagePtr im = get_image_from_binary_file(filename_bin, p_parameters);
			
			if ( !im ) {
				return 1;
			}
			
			FILE *fp = fopen(filename_gif, "wb");
			
			if ( !fp ) {
				printf("Error: Cannot open output file %s for writing.", filename_gif);
				gdImageDestroy(im);
				return 1;
			}
			
			gdImageGif(im, fp);

			fclose(fp);
			gdImageDestroy(im);
			
			return 0;
		}
    }
}

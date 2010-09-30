/**
* @file util_visualize.cpp
*/
//---------------------------------------------------------------------------
#include "util_visualize.h"
#include "util_fs.h"
//---------------------------------------------------------------------------
namespace sns {
    namespace visual {
        /**
        * Returns real part of complex number
        * @param complex<double> Complex number
        * @return double Real part
        */
        double real(const complex<double> &c) {
            return c.real();
        }

        /**
        * Returns imagine part of complex number
        * @param complex<double> Complex number
        * @return double Imagine part
        */
        double imag(const complex<double> &c) {
            return c.imag();
        }

        /**
        * Enumerate for binary file types
        */
        enum binary_file_type {
            t_double,         /// Double binary data
            t_complex_double  /// Complex<double> binary data
        };

        /**
        * Array with palette colors
        */
        int palette[256][3];

        /**
        * Struct for palette point, readed from file
        */
        struct palette_point {
            int i; // point index
            
            int r; // red channel
            int g; // green channel
            int b; // blue channel
        };

        /**
        * Comparison function for palette points
        */
        bool compare_palette_points(palette_point a, palette_point b) {
            if( a.i < b.i ) {
                return true;
            } else {
                return false;
            }
        }

        /**
        * Initialize library color palette from file
        * @return void
        */
        void init_color_palette(char* filename) {
            int i;

            palette_point pp;
            list<palette_point> plist;
            list<palette_point>::iterator it, it_min, it_max;
            int min, max;
            palette_point p1, p2;

            // Read palette from file
            if (filename){
                FILE *fp = fopen(filename, "r");

                while (fscanf(fp, "%d %d %d %d", &pp.i, &pp.r, &pp.g, &pp.b) == 4) {
                    plist.push_back(pp);
                }

                // Generate palette from points
                if (plist.size() >= 2) {
                    // Sort point from file, for newbies
                    plist.sort(compare_palette_points);

                    // Scale indexes for 0..255
                    it_min = min_element(plist.begin(), plist.end(), compare_palette_points);
                    it_max = max_element(plist.begin(), plist.end(), compare_palette_points);
                    min = (*it_min).i;
                    max = (*it_max).i;

                    for( it = plist.begin(); it != plist.end(); ++it) {
                        (*it).i = ((*it).i - min)*255/(max-min);
                    }

                    // Set palette colors
                    it = plist.begin();
                    p1 = *it;
                    ++it;
                    p2 = *it;

                    for ( i = 0; i < 256; i++ ) {
                        if ( i > p2.i ) {
                            p1 = p2;
                            ++it;
                            p2 = *it;
                        }

                        palette[i][0] = ((i - p1.i)*p2.r + (p2.i - i)*p1.r)/(p2.i-p1.i);
                        palette[i][1] = ((i - p1.i)*p2.g + (p2.i - i)*p1.g)/(p2.i-p1.i);
                        palette[i][2] = ((i - p1.i)*p2.b + (p2.i - i)*p1.b)/(p2.i-p1.i);
                    }

                    // All OK, return
                    return;
                }
            }

            // OTHERWISE
            // Generate simple grayscale palette
            for ( i = 0; i < 256; i++ ) {
                palette[i][0] = i;
                palette[i][1] = i;
                palette[i][2] = i;
            }
        }

        /**
        *
        * @return gdImagePtr Image (GD class)
        */
        gdImagePtr get_image_from_binary_file(char* filename, bin2gif_parameters p_parameters) {
            gdImagePtr im;
            FILE* fp = NULL;

            int i = 0, j = 0, ii = 0, jj = 0, kk = 0;
            int n = 0, factor_x = 1, factor_y = 1;

            double (*func)(const complex<double>&) = abs;

            if(        strcmp(p_parameters.to_func, "amp") == 0 ) {
                func = abs;
            } else if( strcmp(p_parameters.to_func, "norm") == 0 ) {
                func = norm;
            } else if( strcmp(p_parameters.to_func, "real") == 0 ) {
                func = real;
            } else if( strcmp(p_parameters.to_func, "imag") == 0 ) {
                func = imag;
            } else if( strcmp(p_parameters.to_func, "arg") == 0 ) {
                func = arg;
            }

            binary_file_type file_type = t_complex_double;
            off_t file_size, elements_in_file;
            void *data;

            if (p_parameters.bin_axial) {
                int nr = 0, nt = 0;
                double *grid_r = NULL, *grid_t = NULL;

                fp = fopen(filename, "r");

                if ( !fp ) {
                    printf("Cannot open input file %s  for reading.\n", filename);
                    return NULL;
                }

                if (fread(&nr, sizeof(int), 1, fp) != 1) {
                    printf("Cannot read from file %s.\n", filename);
                    fclose(fp);
                    return NULL;
                }

                grid_r = new double[nr];
                if (!grid_r) {
                    printf("Cannot allocate memory for data.\n");
                    fclose(fp);
                    return NULL;
                }

                if (fread(grid_r, sizeof(double), nr, fp) != nr) {
                    printf("Cannot read from file %s.\n", filename);
                    delete[] grid_r;
                    fclose(fp);
                    return NULL;
                }

                if (fread(&nt, sizeof(int), 1, fp) != 1) {
                    printf("Cannot read from file %s.\n", filename);
                    delete[] grid_r;
                    fclose(fp);
                    return NULL;
                }

                grid_t = new double[nt];
                if (!grid_t) {
                    printf("Cannot allocate memory for data.\n");
                    delete[] grid_r;
                    fclose(fp);
                    return NULL;
                }

                if (fread(grid_t, sizeof(double), nt, fp) != nt) {
                    printf("Cannot read from file %s.\n", filename);
                    delete[] grid_r;
                    delete[] grid_t;
                    fclose(fp);
                    return NULL;
                }

                void *axdata;
                if ( file_type == t_complex_double ) {
                    axdata = new complex<double>[nr*nt];
                } else {
                    axdata = new double[nr*nt];
                }
                if (!axdata) {
                    printf("Cannot allocate memory for data.\n");
                    delete[] grid_r;
                    delete[] grid_t;
                    fclose(fp);
                    return NULL;
                }

                if (file_type == t_complex_double) {
                    elements_in_file = fread(axdata, sizeof(complex<double>), nr*nt, fp);
                } else {
                    elements_in_file = fread(axdata, sizeof(double), nr*nt, fp);
                }
                if (elements_in_file != nr*nt) {
                    printf("Cannot read from file %s.\n", filename);
                    delete[] grid_r;
                    delete[] grid_t;
                    delete[] axdata;
                    fclose(fp);
                    return NULL;
                }

                fclose(fp);

                // Slice central time layer
                if (file_type == t_complex_double) {
                    memmove(axdata,
                            &((static_cast<complex<double>*>(axdata))[nr*((nt-1)/2)]),
                            sizeof(complex<double>)*nr);
                } else {
                    memmove(axdata,
                            &((static_cast<double*>(axdata))[nr*((nt-1)/2)]),
                            sizeof(double)*nr);
                }

                // Convert axial to square
                double radius = (grid_r[nr-1] + grid_r[nr-2])/2, r = 0;
                p_parameters.bin_width = 2*static_cast<int>( radius / ( ((grid_r[nr-1]-grid_r[nr-2]) + (grid_r[1]-grid_r[0]))/2  ) );
                p_parameters.bin_height = p_parameters.bin_width;
                if (file_type == t_complex_double) {
                    data = new complex<double>[p_parameters.bin_width*p_parameters.bin_height];
                } else {
                    data = new double[p_parameters.bin_width*p_parameters.bin_height];
                }

                // Convert axial to square
                int k = 0;
                for (j = 0; j < p_parameters.bin_height; j++) {
                    for (i = 0; i < p_parameters.bin_width; i++) {
                        r = 4*static_cast<double>(
                                (j-p_parameters.bin_height/2)*(j-p_parameters.bin_height/2) +
                                (i-p_parameters.bin_height/2)*(i-p_parameters.bin_height/2)
                            ) / p_parameters.bin_height/p_parameters.bin_height;
                        r = sqrt(r)*radius;
                        
                        if (r < grid_r[k]) {
                            for (; k >= 0; k--) {
                                if (grid_r[k] <= r && r <= grid_r[k + 1]) {
                                    break;
                                }
                            }
                        } else {
                            for (; k < nr-1; k++) {
                                if (grid_r[k] <= r && r <= grid_r[k + 1]) {
                                    break;
                                }
                            }
                        }
                        
                        if (file_type == t_complex_double) {
                            static_cast<complex<double>*>(data)[p_parameters.bin_width*j+i] = static_cast<complex<double>*>(axdata)[k];
                        } else {
                            static_cast<double*>(data)[p_parameters.bin_width*j+i] = static_cast<double*>(axdata)[k];
                        }
                    }
                }

                // Temp hack
                p_parameters.to_width = p_parameters.bin_width;
                p_parameters.to_height = p_parameters.bin_height;

                delete[] axdata;
            } else { // Standart square matrix
                // Determine file type and image size {{{
                if (p_parameters.bin_width == -1 || p_parameters.bin_height == -1) {
                    file_size = fs::file_size(filename) - p_parameters.bin_header - p_parameters.bin_footer;

                    if (file_size <= 0) {
                        printf("Cannot determine file size.\n");
                        return NULL;
                    }

                    elements_in_file = file_size / sizeof(complex<double>);
                    n = (off_t)(sqrt((double)elements_in_file));
                    if (elements_in_file != n*n) {
                        file_type = t_double;
                        elements_in_file = file_size / sizeof(double);
                        n = (off_t)(sqrt((double)elements_in_file));
                    }

                    p_parameters.bin_width = n;
                    p_parameters.bin_height = n;

                    if (n < p_parameters.to_width) {
                        p_parameters.to_width = n;
                    }

                    if (n < p_parameters.to_height) {
                        p_parameters.to_height = n;
                    }
                }
                // }}}

                // No resize if --resize not specified {{{
                if (p_parameters.to_width < 0) {
                    p_parameters.to_width = p_parameters.bin_width;
                }

                if (p_parameters.to_height < 0) {
                    p_parameters.to_height = p_parameters.bin_height;
                }

                factor_x = p_parameters.bin_width/p_parameters.to_width;
                factor_y = p_parameters.bin_height/p_parameters.to_height;

                if (file_type == t_complex_double) {
                    data = new complex<double>[p_parameters.bin_width*p_parameters.bin_height];
                } else {
                    data = new double[p_parameters.bin_width*p_parameters.bin_height];
                }

                if (!data) {
                    printf("Cannot allocate memory for data.\n");
                    return NULL;
                }

                fp = fopen(filename, "r");

                if (!fp) {
                    printf("Cannot open input file %s  for reading.\n", filename);
                    delete[] data;
                    return NULL;
                }

                fseek(fp, p_parameters.bin_header, SEEK_SET);

                if (file_type == t_complex_double) {
                    elements_in_file = fread(data, sizeof(complex<double>), p_parameters.bin_width*p_parameters.bin_height, fp);
                } else {
                    elements_in_file = fread(data, sizeof(double), p_parameters.bin_width*p_parameters.bin_height, fp);
                }

                if (elements_in_file != p_parameters.bin_width*p_parameters.bin_height) {
                    printf("Bad file format or corrupted file, only %ld elements of %d readed.\n", elements_in_file, p_parameters.bin_width*p_parameters.bin_height);
                    delete[] data;
                    fclose(fp);
                    return NULL;
                }

                fclose(fp);
            } // Data loaded

            // Debug {{{
            if (p_parameters.debug) {
                //printf("\033[0;33mDebug {{{\n");

                printf("bin_width: %d\n", p_parameters.bin_width);
                printf("bin_height: %d\n", p_parameters.bin_height);

                printf("to_width: %d\n", p_parameters.to_width);
                printf("to_height: %d\n", p_parameters.to_height);

                printf("factor_x: %d\n", factor_x);
                printf("factor_y: %d\n", factor_y);

                if (file_type == t_complex_double) {
                    printf("file_type: complex<double>\n");
                } else {
                    printf("file_type: double\n");
                }

                if (file_type == t_complex_double) {
                    printf("func: %s\n", p_parameters.to_func);
                }

                //printf("Debug }}}\033[0m\n");
            }
            // Debug }}}

            double *ddata = new double[p_parameters.to_width*p_parameters.to_height];

            if (!ddata) {
                printf("Cannot allocate memory for data.\n");
                delete[] data;
                return NULL;
            }

            if (!p_parameters.to_reflect) {
                im = gdImageCreateTrueColor(p_parameters.to_width, p_parameters.to_height);
            } else {
                im = gdImageCreateTrueColor(p_parameters.to_height, p_parameters.to_width);
            }

            if (!im) {
                printf("Cannot create GD image.\n");
                delete[] data;
                delete[] ddata;
                return NULL;
            }

            double d_value = 0;
            #pragma omp parallel for private(d_value,i,ii,jj) shared(ddata)
            for (j = 0; j < p_parameters.to_height; j++) {
                for (i = 0; i < p_parameters.to_width; i++) {

                    d_value = 0;

                    for (jj = 0; jj < factor_y; jj++) {
                        for (ii = 0; ii < factor_x; ii++) {
                            kk = p_parameters.bin_width*(factor_y*j+jj)+(factor_x*i+ii);
                            if (file_type == t_complex_double) {
                                d_value += func(static_cast<complex<double>*>(data)[kk]);
                            } else {
                                d_value += static_cast<double*>(data)[kk];
                            }
                        }
                    }

                    ddata[p_parameters.to_width*j+i] = d_value/factor_x/factor_y;

                }
            }

            delete[] data;

            /*if ( p_parameters.to_fixphase && ( strcmp(p_parameters.to_func, "arg") == 0 ) ) {
                for ( j = 0; j < p_parameters.to_height; j++ ) {
                    for ( i = 0; i < p_parameters.to_width; i++ ) {

                        kk = p_parameters.to_width*j+i;
                        
                        if ( i > 0 || j > 0 ) {
                            if ( i > 0 ) {
                                ii = p_parameters.to_width*j+i-1;
                                
                                while ( ddata[kk] > ddata[ii] + M_PI ) {
                                    ddata[kk] -= 2*M_PI;
                                }
                                while ( ddata[kk] < ddata[ii] + M_PI ) {
                                    ddata[kk] += 2*M_PI;
                                }
                            
                            }
                        }

                    }
                }
            }*/

            int c_color;

            double d_max, d_min;

            d_min = *min_element(ddata, ddata + p_parameters.to_width*p_parameters.to_height);
            d_max = *max_element(ddata, ddata + p_parameters.to_width*p_parameters.to_height);

            if (strcmp(p_parameters.to_func, "arg") == 0) {
                if (p_parameters.debug) {
                    //printf("\033[0;33mDebug {{{\n");
                    printf("d_min: %lf\n", d_min);
                    printf("d_max: %lf\n", d_max);  
                    //printf("Debug }}}\033[0m\n");
                }

                d_min = -M_PI;
                d_max = M_PI;
            } else if (( p_parameters.to_amp > 0 ) && !(p_parameters.to_use_min || p_parameters.to_use_max )) {
                d_min = ( (strcmp(p_parameters.to_func, "norm") == 0) || (strcmp(p_parameters.to_func, "abs") == 0) ) ? 0 : -p_parameters.to_amp;
                d_max = p_parameters.to_amp;
            }

            if (p_parameters.to_use_min) {
                d_min = p_parameters.to_min;
            }

            if (p_parameters.to_use_max) {
                d_max = p_parameters.to_max;
            }

            // Debug {{{
            if (p_parameters.debug) {
                //printf("\033[0;33mDebug {{{\n");
                printf("d_min: %lf\n", d_min);
                printf("d_max: %lf\n", d_max);
                //printf("Debug }}}\033[0m\n");
            }
            // Debug }}}

            for (j = 0; j < p_parameters.to_height; j++) {
                for (i = 0; i < p_parameters.to_width; i++) {
                    c_color = static_cast<int>(255*(ddata[p_parameters.to_width*j+i]-d_min)/(d_max-d_min));
                    c_color = ( c_color > 255 ) ? 255 : ( (c_color < 0) ? 0 : c_color );
                    c_color = gdImageColorAllocate(im, palette[c_color][0], palette[c_color][1], palette[c_color][2]);

                    if (!p_parameters.to_reflect) {
                        gdImageSetPixel(im, i, j, c_color );
                    } else {
                        gdImageSetPixel(im, j, i, c_color );
                    }
                }
            }

            delete[] ddata;

            return im;
        }

        int convert_binary_file_to_gif(char* filename_bin, char* filename_gif, bin2gif_parameters p_parameters) {
            gdImagePtr im = get_image_from_binary_file(filename_bin, p_parameters);

            if (!im) {
                return 1;
            }

            FILE *fp = fopen(filename_gif, "wb");

            if (!fp) {
                printf("Cannot open output file %s for writing.\n", filename_gif);
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


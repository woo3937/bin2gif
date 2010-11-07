/*
Copyright (C) 2009, Oleg Efimov <efimovov@yandex.ru>

See license text in LICENSE file
*/
//---------------------------------------------------------------------------
#include "./util_visualize.h"
#include "./util_fs.h"
//---------------------------------------------------------------------------
namespace sns {
    namespace visual {
        /**
        * Returns real part of std::complex number
        * @param std::complex<double> std::complex number
        * @return double Real part
        */
        double real(const std::complex<double> &c) {
            return c.real();
        }

        /**
        * Returns imagine part of std::complex number
        * @param std::complex<double> std::complex number
        * @return double Imagine part
        */
        double imag(const std::complex<double> &c) {
            return c.imag();
        }

        /**
        * Interpolate value by one coordinate
        */
        template<typename T>
        T interpolate1D(double r1, double r2, T I1, T I2, double r) {
            return (I1*(r2-r) + I2*(r-r1))/(r2-r1);
        }

        /**
        * Interpolate value by two coordinates
        */
        template<typename T>
        T interpolate2D(double r1, double r2, double t1, double t2,
                      T I11, T I12, T I21, T I22,  double r, double t) {
            T I1121 = interpolate1D<T>(r1, r2, I11, I21, r);
            T I1222 = interpolate1D<T>(r1, r2, I12, I22, r);
            T I1121_1222 = interpolate1D<T>(t1, t2, I1121, I1222, t);

            T I1112 = interpolate1D<T>(t1, t2, I11, I12, t);
            T I2122 = interpolate1D<T>(t1, t2, I21, I22, t);
            T I1112_2122 = interpolate1D<T>(r1, r2, I1112, I2122, r);

            return (I1121_1222 + I1112_2122)/2.;
        }

        /**
        * Array with palette colors
        */
        int palette[256][3];

        /**
        * Struct for palette point, readed from file
        */
        struct palette_point {
            int i;  // point index

            int r;  // red channel
            int g;  // green channel
            int b;  // blue channel
        };

        /**
        * Comparison function for palette points
        */
        bool compare_palette_points(palette_point a, palette_point b) {
            if (a.i < b.i) {
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
            std::list<palette_point> plist;
            std::list<palette_point>::iterator it, it_min, it_max;
            int min, max;
            palette_point p1, p2;

            // Read palette from file
            if (filename) {
                FILE *fp = fopen(filename, "r");

                while (fscanf(fp, "%d %d %d %d",
                               &pp.i, &pp.r, &pp.g, &pp.b) == 4) {
                    plist.push_back(pp);
                }

                // Generate palette from points
                if (plist.size() >= 2) {
                    // Sort point from file, for newbies
                    plist.sort(compare_palette_points);

                    // Scale indexes for 0..255
                    it_min = min_element(plist.begin(), plist.end(),
                                         compare_palette_points);
                    it_max = max_element(plist.begin(), plist.end(),
                                         compare_palette_points);
                    min = (*it_min).i;
                    max = (*it_max).i;

                    for (it = plist.begin(); it != plist.end(); ++it) {
                        (*it).i = ((*it).i - min)*255/(max-min);
                    }

                    // Set palette colors
                    it = plist.begin();
                    p1 = *it;
                    ++it;
                    p2 = *it;

                    for (i = 0; i < 256; i++) {
                        if (i > p2.i) {
                            p1 = p2;
                            ++it;
                            p2 = *it;
                        }

                        palette[i][0] = ((i - p1.i)*p2.r + (p2.i - i)*p1.r) /
                                        (p2.i-p1.i);
                        palette[i][1] = ((i - p1.i)*p2.g + (p2.i - i)*p1.g) /
                                        (p2.i-p1.i);
                        palette[i][2] = ((i - p1.i)*p2.b + (p2.i - i)*p1.b) /
                                        (p2.i-p1.i);
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

        void* get_data_from_binary_file(char* filename,
                                        bin2gif_parameters *p_params) {
            FILE* fp = NULL;

            int i = 0, j = 0;
            int n = 0;

            p_params->file_type = t_complex_double;
            off_t file_size, elements_in_file;
            void *data;
            std::complex<double> *data_cd;
            double *data_d;

            if (p_params->bin_axial || p_params->bin_axial_all) {
                // TODO(Sannis): Add filetype determining {{{
                if (p_params->bin_type == 'c') {
                    p_params->file_type = t_complex_double;
                } else if (p_params->bin_type == 'd') {
                    p_params->file_type = t_double;
                } else {
                    return NULL;
                }
                // }}}

                int nr = 0, nt = 0;
                double *grid_r = NULL, *grid_t = NULL;

                fp = fopen(filename, "r");

                if ( !fp ) {
                    printf("Cannot open input file %s  for reading.\n",
                           filename);
                    return NULL;
                }

                if (fread(&nr, sizeof(nr), 1, fp) != 1) {
                    printf("Cannot read Nr from file %s.\n",
                           filename);
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
                    printf("Cannot read grid_r from file %s.\n",
                           filename);
                    delete[] grid_r;
                    fclose(fp);
                    return NULL;
                }

                if (fread(&nt, sizeof(nt), 1, fp) != 1) {
                    printf("Cannot read Nt from file %s.\n",
                           filename);
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
                    printf("Cannot read grid_t from file %s.\n",
                           filename);
                    delete[] grid_r;
                    delete[] grid_t;
                    fclose(fp);
                    return NULL;
                }

                void *axdata;
                if (p_params->file_type == t_complex_double) {
                    axdata = new std::complex<double>[nr*nt];
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
                std::complex<double> *axdata_cd = static_cast<std::complex<double>*>(axdata); // NOLINT
                double *axdata_d = static_cast<double*>(axdata);

                if (p_params->file_type == t_complex_double) {
                    elements_in_file = fread(axdata, sizeof(std::complex<double>),
                                             nr*nt, fp);
                } else {
                    elements_in_file = fread(axdata, sizeof(double),
                                             nr*nt, fp);
                }
                if (elements_in_file != nr*nt) {
                    printf("Cannot read axial data from file %s. Read %d %s elements, but %d expected.\n", filename, elements_in_file, (p_params->file_type == t_complex_double) ? "double" : "std::complex", nr*nt); // NOLINT
                    delete[] grid_r;
                    delete[] grid_t;
                    delete[] axdata;
                    fclose(fp);
                    return NULL;
                }

                fclose(fp);

                if (p_params->bin_axial) {  // Draw only T=0 cut
                    // Slice central time layer
                    if (p_params->file_type == t_complex_double) {
                        axdata_cd += nr*((nt-1)/2);
                    } else {
                        axdata_d += nr*((nt-1)/2);
                    }

                    // Convert axial to square
                    double radius = (grid_r[nr-1] + grid_r[nr-2])/2, r = 0;

                    if (p_params->to_width < 0) {
                        p_params->to_width = 2*static_cast<int>( radius / sqrt((grid_r[nr-1]-grid_r[nr-2])*(grid_r[1]-grid_r[0])) ) + 1; // NOLINT
                    }
                    p_params->to_height = p_params->to_width;

                    if (p_params->file_type == t_complex_double) {
                        data = new std::complex<double>[p_params->to_width*p_params->to_height]; // NOLINT
                    } else {
                        data = new double[p_params->to_width*p_params->to_height]; // NOLINT
                    }
                    if (!data) {
                        printf("Cannot allocate memory for data.\n");
                        delete[] grid_r;
                        delete[] grid_t;
                        delete[] axdata;
                        return NULL;
                    }
                    data_cd = static_cast<std::complex<double>*>(data);
                    data_d = static_cast<double*>(data);

                    // Convert axial to square
                    int k = 0;
                    for (j = 0; j < p_params->to_height; j++) {
                        for (i = 0; i < p_params->to_width; i++) {
                            r = 4*static_cast<double>(
                                    (j-p_params->to_height/2)*(j-p_params->to_height/2) + // NOLINT
                                    (i-p_params->to_height/2)*(i-p_params->to_height/2)) // NOLINT
                                / p_params->to_height/p_params->to_height;
                            r = sqrt(r)*radius;

                            if (r < radius) {
                                if (r < grid_r[k]) {
                                    for (; k >= 0; k--) {
                                        if (grid_r[k] <= r && r <= grid_r[k + 1]) { // NOLINT
                                            break;
                                        }
                                    }
                                } else {
                                    for (; k < nr-1; k++) {
                                        if (grid_r[k] <= r && r <= grid_r[k + 1]) { // NOLINT
                                            break;
                                        }
                                    }
                                }

                                if (p_params->file_type == t_complex_double) {
                                    data_cd[p_params->to_width*j+i] =
                                        interpolate1D< std::complex<double> >(grid_r[k], grid_r[k+1], axdata_cd[k], axdata_cd[k+1], r); // NOLINT
                                } else {
                                    data_d[p_params->to_width*j+i] =
                                        interpolate1D< double >(grid_r[k], grid_r[k+1], axdata_d[k], axdata_d[k+1], r); // NOLINT
                                }
                            } else {
                                if (p_params->file_type == t_complex_double) {
                                    data_cd[p_params->to_width*j+i] = 0;
                                } else {
                                    data_d[p_params->to_width*j+i] = 0;
                                }
                            }
                        }
                    }
                } else {  // Draw all RT plane
                    if (nt < 3) {
                        delete[] grid_r;
                        delete[] grid_t;
                        delete[] axdata;
                        return NULL;
                    }

                    // Convert axial to square
                    p_params->sr = (grid_r[nr-1] + grid_r[nr-2])/2;
                    p_params->st = (grid_t[nt-1] + grid_r[nt-2])/2;
                    double r = 0, t = 0;

                    if (p_params->to_width < 0) {
                        p_params->to_width = 2*static_cast<int>( p_params->st / sqrt((grid_t[nt-1]-grid_t[nt-2])*(grid_t[static_cast<int>((nt-1)/2)-1]-grid_t[static_cast<int>((nt-1)/2)])) ) + 1; // NOLINT
                    }
                    if (p_params->to_height < 0) {
                        p_params->to_height = 2*static_cast<int>( p_params->sr / sqrt((grid_r[nr-1]-grid_r[nr-2])*(grid_r[1]-grid_r[0])) ) + 1; // NOLINT
                    }

                    if (p_params->file_type == t_complex_double) {
                        data = new std::complex<double>[p_params->to_width*p_params->to_height]; // NOLINT
                    } else {
                        data = new double[p_params->to_width*p_params->to_height]; // NOLINT
                    }
                    if (!data) {
                        printf("Cannot allocate memory for data.\n");
                        delete[] grid_r;
                        delete[] grid_t;
                        delete[] axdata;
                        return NULL;
                    }
                    data_cd = static_cast<std::complex<double>*>(data);
                    data_d = static_cast<double*>(data);

                    // Convert axial to square
                    int k_r = 0, k_t = 0;
                    for (j = 0; j < p_params->to_height/2; j++) {
                        for (i = 0; i < p_params->to_width; i++) {
                            r = p_params->sr * 2 * static_cast<double>(j) / p_params->to_height; // NOLINT
                            t = p_params->st * 4 * static_cast<double>(i - p_params->to_width/2) / p_params->to_width; // NOLINT

                            if (r < grid_r[k_r]) {
                                for (; k_r >= 0; k_r--) {
                                    if (grid_r[k_r] <= r && r <= grid_r[k_r + 1]) { // NOLINT
                                        break;
                                    }
                                }
                            } else {
                                for (; k_r < nr-1; k_r++) {
                                    if (grid_r[k_r] <= r && r <= grid_r[k_r + 1]) { // NOLINT
                                        break;
                                    }
                                }
                            }

                            if (t < grid_t[k_t]) {
                                for (; k_t >= 0; k_t--) {
                                    if (grid_t[k_t] <= t && t <= grid_t[k_t + 1]) { // NOLINT
                                        break;
                                    }
                                }
                            } else {
                                for (; k_t < nt-1; k_t++) {
                                    if (grid_t[k_t] <= t && t <= grid_t[k_t + 1]) { // NOLINT
                                        break;
                                    }
                                }
                            }

                            if (p_params->file_type == t_complex_double) {
                                data_cd[p_params->to_width*(p_params->to_height/2 + j) + (p_params->to_width - 1 - i)] =                                 // NOLINT
                                    interpolate2D< std::complex<double> >(grid_r[k_r], grid_r[k_r+1], grid_t[k_t], grid_t[k_t+1],                       // NOLINT
                                                                      axdata_cd[nr*k_t + k_r], axdata_cd[nr*(k_t+1) + k_r],                              // NOLINT
                                                                      axdata_cd[nr*k_t + (k_r+1)], axdata_cd[nr*(k_t+1) + (k_r+1)], r, t);               // NOLINT
                                data_cd[p_params->to_width*(p_params->to_height/2 - 1 - j) + (p_params->to_width - 1 - i)] = data_cd[p_params->to_width*(p_params->to_height/2 + j) + (p_params->to_width - 1 - i)]; // NOLINT
                            } else {
                                data_d[p_params->to_width*(p_params->to_height/2 + j) + (p_params->to_width - 1 - i)] =                                  // NOLINT
                                    interpolate2D< double >(grid_r[k_r], grid_r[k_r+1], grid_t[k_t], grid_t[k_t+1],                                     // NOLINT
                                                             axdata_d[nr*k_t + k_r], axdata_d[nr*(k_t+1) + k_r],                                         // NOLINT
                                                             axdata_d[nr*k_t + (k_r+1)], axdata_d[nr*(k_t+1) + (k_r+1)], r, t);                          // NOLINT
                                data_d[p_params->to_width*(p_params->to_height/2 - 1 - j) + (p_params->to_width - 1 - i)] = data_d[p_params->to_width*(p_params->to_height/2 + j) + (p_params->to_width - 1 - i)]; // NOLINT
                            }
                        }
                    }
                }

                // Temp hack
                p_params->bin_width = p_params->to_width;
                p_params->bin_height = p_params->to_height;

                delete[] axdata;
            } else {  // Standart square matrix
                // Determine file type and image size {{{
                if (p_params->bin_width == -1 || p_params->bin_height == -1) {
                    file_size = fs::file_size(filename)
                                - p_params->bin_header
                                - p_params->bin_footer;

                    if (file_size <= 0) {
                        printf("Cannot determine file size.\n");
                        return NULL;
                    }

                    elements_in_file = file_size / sizeof(std::complex<double>);
                    n = static_cast<off_t>(sqrt(static_cast<double>(elements_in_file))); // NOLINT
                    if (elements_in_file != n*n) {
                        p_params->file_type = t_double;
                        elements_in_file = file_size / sizeof(double);
                        n = static_cast<off_t>(sqrt(static_cast<double>(elements_in_file))); // NOLINT
                    }

                    p_params->bin_width = n;
                    p_params->bin_height = n;

                    if (n < p_params->to_width) {
                        p_params->to_width = n;
                    }

                    if (n < p_params->to_height) {
                        p_params->to_height = n;
                    }
                }
                // }}}

                off_t bin_count = static_cast<off_t>(p_params->bin_width)*p_params->bin_height; // NOLINT

                // No resize if --resize not specified {{{
                if (p_params->to_width < 0) {
                    p_params->to_width = p_params->bin_width;
                }

                if (p_params->to_height < 0) {
                    p_params->to_height = p_params->bin_height;
                }

                if (p_params->file_type == t_complex_double) {
                    data = new std::complex<double>[bin_count];
                } else {
                    data = new double[bin_count];
                }
                if (!data) {
                    printf("Cannot allocate memory for data.\n");
                    return NULL;
                }
                data_cd = static_cast<std::complex<double>*>(data);
                data_d = static_cast<double*>(data);

                fp = fopen(filename, "r");

                if (!fp) {
                    printf("Cannot open input file %s  for reading.\n",
                           filename);
                    delete[] data;
                    return NULL;
                }

                fseek(fp, p_params->bin_header, SEEK_SET);

                if (p_params->file_type == t_complex_double) {
                    elements_in_file = fread(data,
                                             sizeof(std::complex<double>),
                                             bin_count, fp);
                } else {
                    elements_in_file = fread(data, sizeof(double),
                                             bin_count, fp);
                }

                if (elements_in_file != bin_count) {
                    printf("Error: Bad file format or corrupted file\n");
                    printf("Only %ld elements of %d readed.\n",
                           elements_in_file, bin_count);
                    delete[] data;
                    fclose(fp);
                    return NULL;
                }

                fclose(fp);
            }

            return data;
        }

        int convert_binary_file_to_gif(char* filename_bin, char* filename_gif,
                                       bin2gif_parameters *p_params) {
            gdImagePtr im;

            int i = 0, j = 0, ii = 0, jj = 0, kk = 0;
            int factor_x = 1, factor_y = 1;

            double (*func)(const std::complex<double>&) = std::abs;

            if (       strcmp(p_params->to_func, "amp") == 0) { // NOLINT
                func = std::abs;
            } else if (strcmp(p_params->to_func, "norm") == 0) {
                func = std::norm;
            } else if (strcmp(p_params->to_func, "real") == 0) {
                func = sns::visual::real;
            } else if (strcmp(p_params->to_func, "imag") == 0) {
                func = sns::visual::imag;
            } else if (strcmp(p_params->to_func, "arg") == 0) {
                func = std::arg;
            }

            double d_value = 0;

            // Read data from file and convert to square matrix
            void *data = get_data_from_binary_file(filename_bin, p_params);
            std::complex<double>* data_cd = static_cast<std::complex<double>*>(data); // NOLINT
            double* data_d = static_cast<double*>(data);

            factor_x = p_params->bin_width/p_params->to_width;
            factor_y = p_params->bin_height/p_params->to_height;

            // Debug {{{
            if (p_params->debug) {
                // printf("\033[0;33mDebug {{{\n");

                printf("bin_width: %d\n", p_params->bin_width);
                printf("bin_height: %d\n", p_params->bin_height);

                printf("to_width: %d\n", p_params->to_width);
                printf("to_height: %d\n", p_params->to_height);

                printf("factor_x: %d\n", factor_x);
                printf("factor_y: %d\n", factor_y);

                if (p_params->file_type == t_complex_double) {
                    printf("p_params->file_type: std::complex<double>\n");
                } else {
                    printf("p_params->file_type: double\n");
                }

                if (p_params->file_type == t_complex_double) {
                    printf("func: %s\n", p_params->to_func);
                }

                // printf("Debug }}}\033[0m\n");
            }
            // Debug }}}

            double *ddata = new double[p_params->to_width*p_params->to_height]; // NOLINT

            if (!ddata) {
                printf("Cannot allocate memory for data.\n");
                delete[] data;
                return 1;
            }

            if (!p_params->to_reflect) {
                im = gdImageCreateTrueColor(p_params->to_width,
                                            p_params->to_height);
            } else {
                im = gdImageCreateTrueColor(p_params->to_height,
                                            p_params->to_width);
            }

            if (!im) {
                printf("Cannot create GD image.\n");
                delete[] data;
                delete[] ddata;
                return 1;
            }

            for (j = 0; j < p_params->to_height; j++) {
                for (i = 0; i < p_params->to_width; i++) {
                    d_value = 0;

                    for (jj = 0; jj < factor_y; jj++) {
                        for (ii = 0; ii < factor_x; ii++) {
                            kk = p_params->bin_width*(factor_y*j + jj) +
                                                    (factor_x*i + ii);
                            if (p_params->file_type == t_complex_double) {
                                d_value += func(data_cd[kk]);
                            } else {
                                d_value += data_d[kk];
                            }
                        }
                    }

                    ddata[p_params->to_width*j+i] = d_value/factor_x/factor_y;
                }
            }

            delete[] data;

            if (p_params->export_text) {
                for (j = 0; j < p_params->to_height; j++) {
                    for (i = 0; i < p_params->to_width; i++) {
                        printf("%d  %d  %lf\n", i, j, ddata[p_params->to_width*j+i]);
                    }
                }
            }

            double d_min, d_max;

            d_min = *std::min_element(ddata,
                                ddata + p_params->to_width*p_params->to_height);
            d_max = *std::max_element(ddata,
                                ddata + p_params->to_width*p_params->to_height);

            if (strcmp(p_params->to_func, "arg") == 0) {
                if (p_params->debug) {
                    // printf("\033[0;33mDebug {{{\n");
                    printf("d_min: %lf\n", d_min);
                    printf("d_max: %lf\n", d_max);
                    // printf("Debug }}}\033[0m\n");
                }

                d_min = -M_PI;
                d_max = M_PI;
            } else if ((p_params->to_amp > 0) &&
                        !(p_params->to_use_min ||
                          p_params->to_use_max ||
                          p_params->to_amp_e)
                       ) {
                d_min = ( (strcmp(p_params->to_func, "norm") == 0) ||
                          (strcmp(p_params->to_func, "abs") == 0) ) ?
                          0 : -p_params->to_amp;
                d_max = p_params->to_amp;
            } else if (p_params->to_amp_e) {
                d_min = d_min/M_El;
                d_max = d_max/M_El;
            }

            if (p_params->to_use_min) {
                d_min = p_params->to_min;
            }

            if (p_params->to_use_max) {
                d_max = p_params->to_max;
            }

            // Debug {{{
            if (p_params->debug) {
                // printf("\033[0;33mDebug {{{\n");
                printf("d_min: %lf\n", d_min);
                printf("d_max: %lf\n", d_max);
                // printf("Debug }}}\033[0m\n");
            }
            // Debug }}}

            if (p_params->use_mathgl) {
                mglData md;

                // if (p_params->bin_axial || p_params->bin_axial_all) {
                if (p_params->bin_axial_all) {
                    md.Create(p_params->to_width, p_params->to_height);
                    for (j = 0; j < p_params->to_height; j++) {
                        for (i = 0; i < p_params->to_width; i++) {
                            md.a[p_params->to_width*j+i] =
                                                  ddata[p_params->to_width*j+i];
                        }
                    }
                }

                mglGraphZB mgr(800, 600);
                mgr.Rotate(60, -20);
                if (p_params->bin_axial_all || p_params->bin_axial_all) {
                    mgr.SetRanges (-p_params->sr, p_params->sr,
                                   -p_params->st, p_params->st,
                                   d_min, d_max);
                }
                mgr.AdjustTicks();
                mgr.Label('x', "t", 0);
                mgr.Label('y', "r", 0);
                mgr.Surf(md);
                //mgr.Cont(md, "y");
                mgr.Axis();
                //mgr.Box();
                mgr.WriteGIF(filename_gif);
            } else { // Use GD for render plain image
                int c_color;

                for (j = 0; j < p_params->to_height; j++) {
                    for (i = 0; i < p_params->to_width; i++) {
                        c_color = static_cast<int>(255*(ddata[p_params->to_width*j+i]-d_min)/(d_max-d_min)); // NOLINT
                        c_color = (c_color > 255) ? 255
                                                  : ((c_color < 0) ? 0
                                                                   : c_color);
                        c_color = gdImageColorAllocate(im, palette[c_color][0],
                                                           palette[c_color][1],
                                                           palette[c_color][2]);

                        if (!p_params->to_reflect) {
                            gdImageSetPixel(im, i, j, c_color);
                        } else {
                            gdImageSetPixel(im, j, i, c_color);
                        }
                    }
                }

                delete[] ddata;

                FILE *fp = fopen(filename_gif, "wb");

                if (!fp) {
                    printf("Cannot open output file %s for writing.\n",
                           filename_gif);
                    gdImageDestroy(im);
                    return 1;
                }

                gdImageGif(im, fp);

                fclose(fp);
                gdImageDestroy(im);
            }

            return 0;
        }
    }
}


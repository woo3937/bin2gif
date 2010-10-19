#include <complex>
#include <cstdio>
using namespace std;
//---------------------------------------------------------------------------
template<typename T>
void create_file_const(char* filename, int w, int h, T d) {
    int i = 0;

    T *data = new T[w*h];

    for (i = 0; i < w*h; i++) {
        data[i] = d;
    }

    FILE *fp = fopen(filename, "wb");
    
    if (!fp) {
        printf("Test file %s doesn't created.\n", filename);
        return;
    }

    fwrite(data, sizeof(T), w*h, fp);

    fclose(fp);

    delete[] data;

    printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
template<typename T>
void create_file_gradient(char* filename, int w, int h) {
    int i = 0, j = 0;

    T *data = new T[w*h];

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            data[j*w+i] = 300*i + j*j;
        }
    }

    FILE *fp = fopen(filename, "wb");
    
    if (!fp) {
        printf("Test file %s doesn't created.\n", filename);
        return;
    }

    fwrite(data, sizeof(T), w*h, fp);

    fclose(fp);

    delete[] data;

    printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
template<typename T>
void create_file_gauss(char* filename, int w, int h) {
    int i = 0, j = 0;

    double w2 = static_cast<double>(w)/2;
    double h2 = static_cast<double>(h)/2;

    double r = static_cast<double>(w+h)/13;

    T *data = new T[w*h];

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            data[j*w+i] = exp(-((i-w2)*(i-w2) + (j-h2)*(j-h2))/r/r);
        }
    }

    FILE *fp = fopen(filename, "wb");

    if (!fp) {
        printf("Test file %s doesn't created.\n", filename);
        return;
    }

    fwrite(data, sizeof(T), w*h, fp);

    fclose(fp);

    delete[] data;

    printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
template<typename T>
void create_file_const_axial(char* filename, int nr, T d) {
    int i = 0;

    T *data = new T[nr];
    double grid_t[] = {0};
    int nt = 1;
    double grid_r[nr];

    for (i = 0; i < nr; i++) {
        grid_r[i] = i;
        data[i] = d;
    }

    FILE *fp = fopen(filename, "wb");

    if (!fp) {
        printf("Test file %s doesn't created.\n", filename);
        return;
    }

    // Write Nr
    fwrite(&nr, sizeof(int), 1, fp);

    // Write grid_r
    fwrite(grid_r, sizeof(double), nr, fp);

    // Write Nt
    fwrite(&nt, sizeof(int), 1, fp);

    // Write grid_t
    fwrite(grid_t, sizeof(double), nt, fp);

    fwrite(data, sizeof(T), nr, fp);

    fclose(fp);

    delete[] data;

    printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
template<typename T>
void create_file_gauss_axial(char* filename, int nr) {
    int i = 0;

    double r = static_cast<double>(nr)/5;

    T *data = new T[nr];
    double grid_t[] = {0};
    int nt = 1;
    double grid_r[nr];

    for (i = 0; i < nr; i++) {
        grid_r[i] = i;
        data[i] = exp(-(grid_r[i]*grid_r[i])/r/r);
    }

    FILE *fp = fopen(filename, "wb");

    if (!fp) {
        printf("Test file %s doesn't created.\n", filename);
        return;
    }

    // Write Nr
    fwrite(&nr, sizeof(int), 1, fp);

    // Write grid_r
    fwrite(grid_r, sizeof(double), nr, fp);

    // Write Nt
    fwrite(&nt, sizeof(int), 1, fp);

    // Write grid_t
    fwrite(grid_t, sizeof(double), nt, fp);

    fwrite(data, sizeof(T), nr, fp);

    fclose(fp);

    delete[] data;

    printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    create_file_const<double>("./tests/one512x512.dbl", 512, 512, 1.);
    create_file_const<double>("./tests/zero512x512.dbl", 512, 512, 0.);
    create_file_const< complex<double> >("./tests/one512x512.cpl", 512, 512, 1.);
    create_file_const< complex<double> >("./tests/zero512x512.cpl", 512, 512, 0.);

    create_file_gradient<double>("./tests/gradient256x256.dbl", 256, 256);
    create_file_gradient<double>("./tests/gradient512x512.dbl", 512, 512);
    create_file_gradient<double>("./tests/gradient1024x1024.dbl", 1024, 1024);
    create_file_gradient< complex<double> >("./tests/gradient256x256.cpl", 256, 256);
    create_file_gradient< complex<double> >("./tests/gradient512x512.cpl", 512, 512);
    create_file_gradient< complex<double> >("./tests/gradient1024x1024.cpl", 1024, 1024);

    create_file_gauss<double>("./tests/gauss1024x1024.dbl", 1024, 1024);
    create_file_gauss< complex<double> >("./tests/gauss1024x1024.cpl", 1024, 1024);

    create_file_const_axial<double>("./tests/one512_axial.adbl", 512, 1.);
    create_file_const_axial<double>("./tests/zero512_axial.adbl", 512, 0.);
    create_file_const_axial< complex<double> >("./tests/one512_axial.acpl", 512, 1.);
    create_file_const_axial< complex<double> >("./tests/zero512_axial.acpl", 512, 0.);

    create_file_gauss_axial<double>("./tests/gauss512_axial.adbl", 512);
    create_file_gauss_axial< complex<double> >("./tests/gauss512_axial.acpl", 512);

    create_file_const_axial<double>("./tests/one5_axial.adbl", 5, 1.);
    create_file_const_axial<double>("./tests/one50_axial.adbl", 50, 1.);
    create_file_const_axial<double>("./tests/one777_axial.adbl", 777, 1.);

    return 0;
}

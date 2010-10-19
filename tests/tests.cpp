#include <complex>
#include <cstdio>
using namespace std;
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
void create_file_gauss_axial(char* filename, int nr) {
    int i = 0;

    double r = static_cast<double>(nr)/3;

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
    fwrite(grid_t, sizeof(double), 1, fp);

    fwrite(data, sizeof(T), nr, fp);

    fclose(fp);

    delete[] data;

    printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    create_file_gradient<double>("./tests/gradient512x512.dbl", 512, 512);
    create_file_gradient<double>("./tests/gradient512x256.dbl", 512, 256);
    create_file_gradient<double>("./tests/gradient1024x1024.dbl", 1024, 1024);

    create_file_gradient< complex<double> >("./tests/gradient256x512.cpl", 256, 512);
    create_file_gradient< complex<double> >("./tests/gradient256x256.cpl", 256, 256);
    create_file_gradient< complex<double> >("./tests/gradient512x512.cpl", 512, 512);
    create_file_gradient< complex<double> >("./tests/gradient1024x1024.cpl", 1024, 1024);

    create_file_gauss<double>("./tests/gauss1024x1024.dbl", 1024, 1024);
    create_file_gauss< complex<double> >("./tests/gauss1024x1024.cpl", 1024, 1024);

    create_file_gauss_axial<double>("./tests/gauss512_axial.adbl", 512);
    create_file_gauss_axial< complex<double> >("./tests/gauss512_axial.acpl", 512);

    return 0;
}

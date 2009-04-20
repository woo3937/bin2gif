#include <complex>
#include <iostream>
using namespace std;
//---------------------------------------------------------------------------
template<typename T>
void create_file(char* filename, int w, int h)
{
	int i = 0, j = 0;
	
	T *data = new T[w*h];
	
	#pragma omp parallel for private(i) shared(data)
	for ( j = 0; j < h; j++ ) {
		for ( i = 0; i < w; i++ ) {
			data[j*w+i] = 300*i + j*j;
		}
	}
	
	FILE *fp = fopen(filename, "wb");
	
	if ( !fp ) {
		printf("Test file %s doesn't created.\n", filename);
		return;
	}
	
	fwrite(data, sizeof(T), w*h, fp);
	
	fclose(fp);
	
	delete[] data;

	printf("Test file %s created.\n", filename);
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	create_file<double>("./tests/double512x512.bin", 512, 512);
	create_file<double>("./tests/double512x256.bin", 512, 256);
	create_file<double>("./tests/double1024x1024.bin", 1024, 1024);
	
	create_file< complex<double> >("./tests/complex256x512.bin", 256, 512);
	create_file< complex<double> >("./tests/complex256x256.bin", 256, 256);
	create_file< complex<double> >("./tests/complex512x512.bin", 512, 512);
	create_file< complex<double> >("./tests/complex1024x1024.bin", 1024, 1024);
	
	return 0;
}

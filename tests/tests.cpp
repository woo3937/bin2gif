#include <complex>
#include <fstream>
#include <iostream>
using namespace std;
//---------------------------------------------------------------------------
template<typename T>
void create_file(char* filename, int n)
{
	int i = 0, j = 0;
	
	T *data = new T[n*n];
	
	for ( i = 0; i < n; i++ ) {
		for ( j = 0; j < n; j++ ) {
			data[i+n*j] = sin(8*M_PI*i/n)*exp((j+sqrt(i))/n);
		}
	}
	
	ofstream outfile(filename, ios::out | ios::binary);
	
	if ( !outfile.is_open() ) {
		cout << "Test file " << filename << "doesn't created." << endl;
		return;
	}
	
	outfile.write(reinterpret_cast<char *>(data), n*n*sizeof(T));
	
	outfile.close();
	
	delete[] data;

	cout << "Test file " << filename << " created." << endl;
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	create_file<double>("./tests/double512.dbl", 512);
	create_file<double>("./tests/double256.bin", 256);
	create_file< complex<double> >("./tests/complex512.bin", 512);
	create_file< complex<double> >("./tests/complex256.cpl", 256);
	
	return 0;
}

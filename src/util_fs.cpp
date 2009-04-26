#include <sys/stat.h>
//---------------------------------------------------------------------------
#include "util_fs.h"
//---------------------------------------------------------------------------
namespace sns
{
	namespace util
	{
		bool is_dir(char* filename)
		{
			struct stat st;
		
			if ( stat(filename, &st) != 0 ) {
				return false;
			}
		
			return S_ISDIR(st.st_mode);
		}
		
		off_t file_size(char* filename)
		{
			struct stat st;
		
			if ( stat(filename, &st) != 0 ) {
				return 0;
			}
		
			return st.st_size;
		}
		
		bool create_dir(char* dirname)
		{
			return mkdir(dirname, S_IRWXU | S_IRGRP | S_IROTH | S_IXOTH);
		}
    }
}

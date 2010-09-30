#ifndef SNS_BIN2GIF_UTIL_FS
#define SNS_BIN2GIF_UTIL_FS
//---------------------------------------------------------------------------
#include <sys/stat.h>
//---------------------------------------------------------------------------
namespace sns {
    namespace fs {
        bool file_exists(char* filename);
        bool is_dir(char* filename);
        off_t file_size(char* filename);
    }
}
//---------------------------------------------------------------------------
#endif


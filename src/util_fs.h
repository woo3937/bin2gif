/*
Copyright (C) 2009, Oleg Efimov <efimovov@yandex.ru>

See license text in LICENSE file
*/
//---------------------------------------------------------------------------
#ifndef SRC_UTIL_FS_H_
#define SRC_UTIL_FS_H_
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
#endif  // SRC_UTIL_FS_H_


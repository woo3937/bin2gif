/*
Copyright (C) 2009, Oleg Efimov <efimovov@yandex.ru>

See license text in LICENSE file
*/
//---------------------------------------------------------------------------
#ifndef SRC_UTIL_VISUALIZE_H_
#define SRC_UTIL_VISUALIZE_H_
//---------------------------------------------------------------------------
#include <gd.h>
#include <mgl/mgl_zb.h>
//---------------------------------------------------------------------------
#include <complex>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <list>
//---------------------------------------------------------------------------
#include "./parameters.h"
//---------------------------------------------------------------------------
namespace sns {
    namespace visual {
        void init_color_palette(char* filename);
        int convert_binary_file_to_gif(char* filename_bin, char* filename_image,
                                       bin2gif_parameters *p_params);
    }
}
//---------------------------------------------------------------------------
#endif  // SRC_UTIL_VISUALIZE_H_


/*
Copyright (C) 2009, Oleg Efimov <efimovov@yandex.ru>

See license text in LICENSE file
*/
//---------------------------------------------------------------------------
#ifndef SRC_PARAMETERS_H_
#define SRC_PARAMETERS_H_
//---------------------------------------------------------------------------
namespace sns {
    struct bin2gif_parameters {
        int file_patterns_count;
        char** file_patterns;

        char* palette_file;

        bool delete_original;

        bool debug;
        bool verbose;
        bool force;

        int bin_width;
        int bin_height;
        char bin_type;
        bool bin_axial;
        bool bin_axial_all;

        int bin_header;
        int bin_footer;

        int to_width;
        int to_height;
        bool to_reflect;
        bool to_fixphase;

        char* to_func;
        double to_amp;
        bool to_amp_e;
        double to_min;
        double to_max;
        bool to_use_min;
        bool to_use_max;
    };
}
//---------------------------------------------------------------------------
#endif  // SRC_PARAMETERS_H_


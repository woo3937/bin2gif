#ifndef SNS_BIN2GIF_UTIL_VISUALIZE
#define SNS_BIN2GIF_UTIL_VISUALIZE
//---------------------------------------------------------------------------
#include "gd.h"
//---------------------------------------------------------------------------
#include "parameters.h"
//---------------------------------------------------------------------------
namespace sns
{
    namespace visual
    {
		gdImagePtr get_image_from_binary_file(char* filename, bin2gif_parameters p_parameters);
		int convert_binary_file_to_gif(char* filename_bin, char* filename_gif, bin2gif_parameters p_parameters);
    }
}
//---------------------------------------------------------------------------
#endif

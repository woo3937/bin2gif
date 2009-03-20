#include "gd.h"
//---------------------------------------------------------------------------
#include "parameters.h"
//---------------------------------------------------------------------------
namespace sns
{
    namespace visual
    {
		gdImagePtr get_image_from_cpl_file(char* filename, bin2gif_parameters p_parameters);
		int convert_cpl_file_to_gif(char* filename_cpl, char* filename_gif, bin2gif_parameters p_parameters);
    }
}

#ifndef SNS_BIN2GIF_PARAMETERS
#define SNS_BIN2GIF_PARAMETERS
//---------------------------------------------------------------------------
namespace sns
{
	struct bin2gif_parameters {
		int file_patterns_count;
		char** file_patterns;
		
		bool delete_original;
		
		bool debug;
		
		int bin_width;
		int bin_height;
		char bin_type;
		
		int bin_header;
		int bin_footer;
		
		int to_width;
		int to_height;
		bool to_reflect;
		bool to_fixphase;
		
		char* to_func;
		double to_amp;
	};
}
//---------------------------------------------------------------------------
#endif

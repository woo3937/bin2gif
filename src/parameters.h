#ifndef SNS_BIN2GIF_PARAMETERS
#define SNS_BIN2GIF_PARAMETERS
//---------------------------------------------------------------------------
namespace sns
{
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
		
		int bin_header;
		int bin_footer;
		
		int to_width;
		int to_height;
		bool to_reflect;
		bool to_fixphase;
		
		char* to_func;
		double to_amp;
		double to_min;
		double to_max;
		bool to_use_min;
		bool to_use_max;
	};
}
//---------------------------------------------------------------------------
#endif


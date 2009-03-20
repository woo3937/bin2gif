#ifndef SNS_BIN2GIF_PARAMETERS
#define SNS_BIN2GIF_PARAMETERS

namespace sns
{
    struct bin2gif_parameters {
		int file_patterns_count;
		char** file_patterns;
		
		int header;
		int footer;
		
		int to_size;
		char to_type;
		int to_amp;
	};
}

#endif
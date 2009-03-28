#include <omp.h>
//---------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
//---------------------------------------------------------------------------
#include <getopt.h>
#include <dirent.h>
#include <glob.h>
#include <string.h>
//---------------------------------------------------------------------------
#include "parameters.h"
#include "util_fs.h"
#include "util_visualize.h"
using namespace sns;
//---------------------------------------------------------------------------
const char *program_version = "1.1 RC1";
//---------------------------------------------------------------------------
char* get_program_name(char *argv0)
{
	char* ch = strrchr(argv0, '/');
	if ( ch != NULL ) {
		return ch + 1;
	} else {
	    return argv0;
	}
}
//---------------------------------------------------------------------------
void display_help(char *argv0)
{
	printf("Usage: %s [options] [--] <filename|dirname|pattern>\n", get_program_name(argv0));
	printf("Utility to convert binary 2D data file into GIF images.\n");
	printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");
	
	printf("\nOptions:\n");
	printf("  -s, --size <num>[x<num>]		dimensions of data in binary file\n");
	printf("  -r, --resize <num>[x<num>]			dimensions of produced image\n");
	printf("  -t, --type (double|d|complex|c)	type of binary data\n");
	printf("  -f, --func (abs|norm|re|im|arg)	function for complex->real conversion\n");
	printf("  -a, --amp <double>			value of image color scale maximum\n");
	printf("  --header <num>			size of file header in bytes\n");
	printf("  --footer <num>			size of file footer in bytes\n");
	printf("  --verbose				do verbose output\n");
	printf("  -v, --version				display program vesion\n");
	printf("  -h, --help				display this help page\n");
	
	printf("\nExamples:\n");
	printf("  %s ~/results/today/*.cpl\n", get_program_name(argv0));
	printf("  %s --header 8 ~/results/today/ ~/results/tomorrow/file.ext\n", get_program_name(argv0));
    
	printf("\nAuthor: Oleg Efimov.\n");
	printf("Report bugs to <efimovov@yandex.ru>.\n");
}
//---------------------------------------------------------------------------
void display_version(char *argv0)
{
	printf("%s version %s\n", get_program_name(argv0), program_version);
	printf("Utility to convert binary 2D data files into GIF images.\n");
	printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");
	
	printf("\nAuthor: Oleg Efimov.\n");
}
//---------------------------------------------------------------------------
void show_debug(bin2gif_parameters *p_parameters)
{
	int i = 0, j = 0;
	
	glob_t globbuf;
	globbuf.gl_offs = 0;
	
	printf("\033[0;33mDebug {{{\n");
	printf("Width: %d\n", p_parameters->bin_width);
	printf("Height: %d\n", p_parameters->bin_height);
	printf("Header: %d\n", p_parameters->bin_header);
	printf("Footer: %d\n", p_parameters->bin_footer);
	printf("Files/directories to process:\n");
	for ( i = 0; i < p_parameters->file_patterns_count; i++ ) {
		printf("\t%s\n", p_parameters->file_patterns[i]);
	}

	for ( i = 0; i < p_parameters->file_patterns_count; i++ ) {
		glob(p_parameters->file_patterns[i], GLOB_DOOFFS, NULL, &globbuf);
		printf("%s:\n", p_parameters->file_patterns[i]);
		for ( j = 0; j < globbuf.gl_pathc; j++ ) {
			printf("\t%s\n", globbuf.gl_pathv[j]);
		}
	}
	printf("Debug }}}\033[0m\n");
}
//---------------------------------------------------------------------------
void get_program_options(int argc, char *argv[], bin2gif_parameters *p_parameters)
{
	int c = 0;
	opterr = 0;
	
	struct option long_options[] = {
		{"size", required_argument, NULL, 's'},
		{"resize", required_argument, NULL, 'r'},
		{"type", required_argument, NULL, 't'},
		{"func", required_argument, NULL, 'f'},
		{"amp", required_argument, NULL, 'a'},
		{"header", required_argument, NULL, NULL}, //+
		{"footer", required_argument, NULL, NULL}, //+
		{"verbose", no_argument, NULL, NULL}, //+
		{"version", no_argument, NULL, 'v'}, //+
		{"help", no_argument, NULL, 'h'}, //+
	};
	int option_index = 0;

	char *ch = NULL;
    
	while ( (c = getopt_long_only(argc, argv, "hvd", long_options, &option_index)) != -1 ) {
		switch(c) {
        	    case 0:
        		if( strcmp(long_options[option_index].name, "resize") == 0 ) {
			    //sscanf(optarg, "%d", &p_parameters->footer);
			} else if( strcmp(long_options[option_index].name, "type") == 0 ) {
			    //sscanf(optarg, "%d", &p_parameters->footer);
			} else if( strcmp(long_options[option_index].name, "func") == 0 ) {
			    //sscanf(optarg, "%d", &p_parameters->footer);
			} else if( strcmp(long_options[option_index].name, "amp") == 0 ) {
			    //sscanf(optarg, "%lf", &p_parameters->amp);
			} else if( strcmp(long_options[option_index].name, "header") == 0 ) {
			    sscanf(optarg, "%d", &p_parameters->bin_header);
			} else if( strcmp(long_options[option_index].name, "footer") == 0 ) {
			    sscanf(optarg, "%d", &p_parameters->bin_footer);
			} else if( strcmp(long_options[option_index].name, "verbose") == 0 ) {
			    p_parameters->verbose = true;
			}
	                break;
	            case 's':
	        	if ( ( ch = strchr(optarg, 'x') ) != NULL  ) {
        			*ch = '\0';
        			sscanf(optarg, "%d", &p_parameters->bin_width);
        			sscanf(ch+1, "%d", &p_parameters->bin_height);
        			
        		    } else {
        			sscanf(optarg, "%d", &p_parameters->bin_width);
        			p_parameters->bin_height = p_parameters->bin_width;
        		    }
	        	break;
        	    case 'h':
            		display_help(argv[0]);
            		exit(0);
        	    case 'v':
            		display_version(argv[0]);
            		exit(0);
            	    case '?':
			printf("Unknown option %d.\n", optopt);
			display_help(argv[0]);
            		exit(0);
        	    default:
            		break;
		}
	}
	
	if ( optind < argc ) {
		p_parameters->file_patterns_count = argc - optind;
		p_parameters->file_patterns = new char*[p_parameters->file_patterns_count];
		
		if ( !p_parameters->file_patterns ) {
		    printf("Error: Мало памяти.");
		    exit(1);
		}
		
		while ( optind < argc ) {
			p_parameters->file_patterns[argc-optind-1] = argv[optind];
			optind++;
		}
	} else {
		if ( p_parameters->verbose ) {
		    show_debug(p_parameters);
		}	
		display_help(argv[0]);
    		exit(0);
	}
}
//---------------------------------------------------------------------------
void process_file(char *filename_bin, bin2gif_parameters p_parameters)
{
	if ( util::is_dir(filename_bin) ) {
		if ( p_parameters.verbose ) {
			printf("Directory %s: \033[70G\033[1;33m[Skipped]\033[0m\n", filename_bin);
		}
		return;
	} else if ( strstr(filename_bin, ".gif") != NULL ) {
		if ( p_parameters.verbose ) {
			printf("File %s: \033[70G\033[1;33m[Skipped]\033[0m\n", filename_bin);
		}
		return;
	}
	char filename_gif[1024];
	char filename_type_fix[] = "_n.gif\0";
	filename_type_fix[1] = p_parameters.to_type;
	char* ch;
	
	filename_gif[0] = '\0';
	ch = strrchr(filename_bin, '.');
	strncat(filename_gif, filename_bin, ch - filename_bin );
	
	strcat(filename_gif, filename_type_fix);
	
	printf("File %s:\t", filename_bin);
	
	if ( visual::convert_binary_file_to_gif(filename_bin, filename_gif, p_parameters) == 0 ) {
		if ( p_parameters.verbose ) {
			printf("\n  -> %s", filename_gif);
		}
		printf("\033[70G\033[0;32m[Done]\033[0m\n");
	} else {
		printf("\033[70G\033[0;31m[Failed]\033[0m\n");
	}
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int i = 0, j = 0;
	
	char *filename_bin = new char[1024];
	if ( !filename_bin ) {
		return 1;
	}
	
	bin2gif_parameters p_parameters;
	
	p_parameters.verbose = false;
	
	p_parameters.file_patterns_count = 0;
	p_parameters.bin_header = 0;
	p_parameters.bin_footer = 0;
	p_parameters.to_size = 512;
	p_parameters.to_type = 'r';
	p_parameters.to_amp = -1;
	
	get_program_options(argc, argv, &p_parameters);
	
	if ( p_parameters.verbose ) {
	    show_debug(&p_parameters);
	}
	
	glob_t globbuf;
	globbuf.gl_offs = 0;
	
	for ( i = 0; i < p_parameters.file_patterns_count; i++ ) {
		glob(p_parameters.file_patterns[i], GLOB_DOOFFS, NULL, &globbuf);
		for ( j = 0; j < globbuf.gl_pathc; j++ ) {
			
			if ( util::is_dir(globbuf.gl_pathv[j]) ) {
			
				dirent *de;
				DIR *dp;
			
				if ( !(dp = opendir(globbuf.gl_pathv[j])) ) {
					printf("\033[0;31m[Error:\033[0m Cannot read directory %s.\n", globbuf.gl_pathv[j]);
					return 1;
				}

				printf("Processing %s:\n", globbuf.gl_pathv[j]);
				
				while ( de = readdir(dp) ) {
					strcpy(filename_bin, globbuf.gl_pathv[j]);
					if( filename_bin[strlen(filename_bin)-1] != '/' ) {
						strcpy(filename_bin + strlen(filename_bin), "/");
					}
					strcpy(filename_bin + strlen(filename_bin), de->d_name);
					
					process_file(filename_bin, p_parameters);
				}
				
				closedir(dp);
			
			} else { // Maybe file?
				process_file(globbuf.gl_pathv[j], p_parameters);
			}
		}
	}

	return 0;
}

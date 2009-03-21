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
#include "./src/parameters.h"
#include "./src/util_fs.h"
#include "./src/util_visualize.h"
using namespace sns;
//---------------------------------------------------------------------------
void display_help(char *program_name)
{
	program_name = strrchr(program_name, '/') + 1;
	
    printf("Usage: %s [options] [--] <filename|dirname|pattern>\n", program_name);
	printf("Utility to convert binary 2D data file into GIF images.\n");
	printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");
	printf("Examples:\n");
	printf("  %s ~/results/today/*.cpl\n", program_name);
	printf("  %s --header 8 ~/results/today/ ~/results/tomorrow/file.ext\n", program_name);
	
	printf("\nOptions:\n");
    printf("  --header    size of file header in bytes, skipped when reading data and determining type\n");
	printf("  --footer    size of file footer in bytes, skipped when determining type\n");
    
	printf("\nAuthor: Oleg Efimov.\n");
	printf("Report bugs to <efimovov@yandex.ru>.\n");
}
//---------------------------------------------------------------------------
void display_version(char *program_name)
{
	program_name = strrchr(program_name, '/') + 1;
	
	printf("%s version 1.0\n", program_name);
	printf("Utility to convert binary 2D data file into GIF images.\n");
	printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");
	
	printf("\nAuthor: Oleg Efimov.\n");
}
//---------------------------------------------------------------------------
void get_program_options(int argc, char *argv[], bin2gif_parameters *p_parameters)
{
    int c = 0;
	opterr = 0;
	
	static struct option long_options[] = {
		{"header", required_argument, NULL, NULL},
		{"footer", required_argument, NULL, NULL},
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"verbose", no_argument, NULL, NULL}
	};
	int option_index = 0;

    
    while ( (c = getopt_long/*_only*/(argc, argv, "hvd", long_options, &option_index)) != -1 ) {
        switch(c) {
            case 0:
				if( strcmp(long_options[option_index].name, "header") == 0 ) {
					sscanf(optarg, "%d", &p_parameters->header);
				} else if( strcmp(long_options[option_index].name, "footer") == 0 ) {
					sscanf(optarg, "%d", &p_parameters->footer);
				}
            break;

			case '?':
				printf("Unknown option %d.\n", optopt);
            case 'h':
                display_help(argv[0]);
                exit(0);
            case 'v':
                display_version(argv[0]);
                exit(0);
			case 'd':
				p_parameters->debug = true;
				break;
            default:
                break;
        }
    }
	
    if ( optind < argc ) {
        p_parameters->file_patterns_count = argc - optind;
		p_parameters->file_patterns = new char*[p_parameters->file_patterns_count];
		while ( optind < argc ) {
			p_parameters->file_patterns[argc-optind-1] = argv[optind];
			optind++;
		}
    } else {
		display_help(argv[0]);
        exit(0);
	}
}
//---------------------------------------------------------------------------
void process_file(char *filename_bin, bin2gif_parameters p_parameters)
{
	if ( util::is_dir(filename_bin) ) {
		printf("Directory %s: \033[70G\033[1;33m[Skipped]\033[0m\n", filename_bin);
		return;
	} else if ( strstr(filename_bin, ".gif") != NULL ) {
		printf("File %s: \033[70G\033[1;33m[Skipped]\033[0m\n", filename_bin);
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
	
	printf("File %s:\n", filename_bin);
	
	if ( visual::convert_binary_file_to_gif(filename_bin, filename_gif, p_parameters) == 0 ) {
		printf("  -> %s", filename_gif);
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
	
	p_parameters.debug = false;
	
	p_parameters.file_patterns_count = 0;
	p_parameters.header = 0;
	p_parameters.footer = 0;
	p_parameters.to_size = 512;
	p_parameters.to_type = 'r';
	p_parameters.to_amp = -1;
	
	get_program_options(argc, argv, &p_parameters);
	
	glob_t globbuf;
	globbuf.gl_offs = 0;
	
	// Debug {{{
	if ( p_parameters.debug ) {
		printf("\033[0;33mDebug {{{\n");
		printf("Header: %d\n", p_parameters.header);
		printf("Footer: %d\n", p_parameters.footer);
		printf("Files/directories to process:\n");
		for ( i = 0; i < p_parameters.file_patterns_count; i++ ) {
			printf("\t%s\n", p_parameters.file_patterns[i]);
		}

		for ( i = 0; i < p_parameters.file_patterns_count; i++ ) {
			glob(p_parameters.file_patterns[i], GLOB_DOOFFS, NULL, &globbuf);
			printf("%s:\n", p_parameters.file_patterns[i]);
			for ( j = 0; j < globbuf.gl_pathc; j++ ) {
				printf("\t%s\n", globbuf.gl_pathv[j]);
			}
		}
		printf("Debug }}}\033[0m\n");
	}
	// Debug }}}
	
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

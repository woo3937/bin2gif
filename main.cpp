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
    printf("Usage: %s [options] [--] <filename|dirname|pattern>\n", program_name);
	printf("Utility to convert binary 2D data file into GIF image.\n");
	printf("Example: %s --type real --max 1 ~/results/today/\n\n", program_name);
	printf("Options:\n");
    printf("  --header    number of point per impulse\n");
	printf("  --footer    number of point per impulse\n");
    
	printf("\nAuthor: Oleg Efimov.\n");
	printf("Report bugs to <efimovov@yandex.ru>.\n");
}
//---------------------------------------------------------------------------
void get_program_options(int argc, char *argv[], bin2gif_parameters *p_parameters)
{
    int c = 0;
	opterr = 0;
	
	static struct option long_options[] = {
		{"header", required_argument, 0, 0},
		{"footer", required_argument, 0, 0}
	};
	int option_index = 0;

    
    while ( (c = getopt_long/*_only*/(argc, argv, "h", long_options, &option_index)) != -1 ) {
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
int main(int argc, char *argv[])
{
	bin2gif_parameters p_parameters;
	
	p_parameters.file_patterns_count = 0;
	p_parameters.header = 0;
	p_parameters.footer = 0;
	p_parameters.to_size = 512;
	p_parameters.to_type = 'n';
	p_parameters.to_amp = -1;
	
	get_program_options(argc, argv, &p_parameters);
	
	printf("Header: %d\n", p_parameters.header);
	printf("Footer: %d\n", p_parameters.footer);
	for ( int i = 0; i < p_parameters.file_patterns_count; i++ ) {
		printf("Pattern: %s\n", p_parameters.file_patterns[i]);
	}
	
	if ( util::is_dir(argv[1]) ) {
	
	    dirent *de;
	    DIR *dp;
	
	    if ( !(dp = opendir(argv[1])) ) {
			printf("Error: Cannot read directory.\n");
			return 1;
	    }
	    
	    char filename_cpl[1024];
	    char filename_gif[1024];
	    char filename_type_fix[] = "_n.gif\0";
	    filename_type_fix[1] = p_parameters.to_type;
	    char* ch;
	    
	    printf("Processing files in directory %s...\n", argv[1]);
	    
	    while ( de = readdir(dp) ) {
			strcpy(filename_cpl, argv[1]);
			strcpy(filename_cpl + strlen(filename_cpl), de->d_name);
			
			if ( strstr(de->d_name, "cpl") == 0 || util::is_dir(filename_cpl) ) {
				printf("File %s: \033[1;31[Skipped]\033[0m\n", de->d_name);
				continue;
			} else {
				printf("File %s: \033[1;32 [Done]\033[0m\n", de->d_name);
				continue;
			}
			
			strcpy(filename_gif, filename_cpl);
			ch = strstr(filename_gif, ".cpl");
			strcpy(ch, filename_type_fix);
			
			if ( visual::convert_cpl_file_to_gif(filename_cpl, filename_gif, p_parameters) == 0 ) {
				printf("File %s: (%s) \033[1;32 [Done]\033[0m\n", de->d_name, filename_gif);
			}
	    }
	    
	    closedir(dp);
	
	}/* else {
	
	    if ( visual::convert_cpl_file_to_gif(argv[1], argv[2], p_parameters) == 0 ) {
		printf("Single file %s: \033[32 [Done]\033[0m\n", argv[1]);
	    }
	
	}*/
	
	return 0;
}

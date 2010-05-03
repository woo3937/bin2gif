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
#define VERSION "0.4"
#define AUTHOR "Oleg Efimov"
#define BUGREPORT_EMAIL "efimovov@gmail.com"
//---------------------------------------------------------------------------
const char* get_program_name(const char *argv0)
{
	char* ch = strrchr(argv0, '/');
	if ( ch != NULL ) {
		return ch + 1;
	} else {
	    return argv0;
	}
}
//---------------------------------------------------------------------------
void display_version(const char *argv0)
{
	printf("%s version %s\n", get_program_name(argv0), VERSION);
	printf("Utility to convert binary 2D data file into GIF images.\n");
	printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");
	
	printf("\nAuthor: %s.\n", AUTHOR);
	printf("Report bugs to <%s>.\n", BUGREPORT_EMAIL);
}
//---------------------------------------------------------------------------
void display_help(const char *argv0)
{
	const char* program_name = get_program_name(argv0);
	
	printf("Usage: %s [options] [--] <filename|dirname|pattern>\n", program_name);
	printf("Utility to convert binary 2D data file into GIF images.\n");
	printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");
	
	printf("\nOptions:\n");
	printf("	-s, --size <num>				dimensions of data in binary file\n");
	printf("	-r, --resize <num>				dimensions of produced image\n");
	printf("	-t, --type (double|d|complex|c)			type of binary data\n");
	printf("	-f, --func (abs|norm|real|imag|arg)		function for complex to real conversion\n");
	printf("	-a, --amp <double>				value of image color scale amplitude\n");
	printf("	--min <double>				value of image color scale minimum\n");
	printf("	--max <double>				value of image color scale maximum\n");
	printf("	--reflect					reflect image, swaps x and y coords\n");
	printf("	--palette <filename>				color palette filename\n\n");
	
	//printf("	--fixphase					fix 2π jumps in calculated complex arguments \n");
	
	printf("	--header <num>					size of file header in bytes\n");
	printf("	--footer <num>					size of file footer in bytes\n");
	printf("	--delete-original				delete original file after convert\n");
	printf("	--debug						do debug output\n");
	printf("	--force						force rewrite existing GIF files\n");
	printf("	--verbose					verbosely output\n\n");
	printf("	-v, --version					display program vesion\n");
	printf("	-h, --help					display this help page\n");
    
	printf("\nExamples:\n");
	printf("	%s ~/results/today/*.cpl\n", program_name);
	printf("	%s --header 8 ~/results/today/ ~/results/tomorrow/file.ext\n", program_name);
	
	printf("\nAuthor: %s.\n", AUTHOR);
	printf("Report bugs to <%s>.\n", BUGREPORT_EMAIL);
}
//---------------------------------------------------------------------------
void get_program_options(int argc, char *argv[], bin2gif_parameters *p_parameters)
{
	int c = 0;
	opterr = 0;
	
	static struct option long_options[] = {
		{"size", required_argument, NULL, 's'},
		{"resize", required_argument, NULL, 'r'},
		{"type", required_argument, NULL, 't'},
		{"func", required_argument, NULL, 'f'},
		{"amp", required_argument, NULL, 'a'},
		{"min", required_argument, NULL, 0},
		{"max", required_argument, NULL, 0},
		{"reflect", no_argument, NULL, 0},
		{"palette", required_argument, NULL, 0},
		
		//{"fixphase", no_argument, NULL, 0},
		
		{"header", required_argument, NULL, 0},
		{"footer", required_argument, NULL, 0},
		
		{"delete-original", no_argument, NULL, 0},
		
		{"debug", no_argument, NULL, 0},
		{"verbose", no_argument, NULL, 0},
		{"force", no_argument, NULL, 0},
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'}
	};
	int option_index = 0;

	while ( (c = getopt_long_only(argc, argv, "s:r:t:f:a:hvd", long_options, &option_index)) != -1 ) {
		switch(c) {
        	    case 0:
			if(        strcmp(long_options[option_index].name, "reflect") == 0 ) {
				p_parameters->to_reflect = true;
			} else if( strcmp(long_options[option_index].name, "palette") == 0 ) {
				p_parameters->palette_file = optarg;
			//} else if( strcmp(long_options[option_index].name, "fixphase") == 0 ) {
			//	p_parameters->to_fixphase = true;
			} else if( strcmp(long_options[option_index].name, "delete-original") == 0 ) {
				p_parameters->delete_original = true;
			} else if( strcmp(long_options[option_index].name, "debug") == 0 ) {
				p_parameters->debug = true;
			} else if( strcmp(long_options[option_index].name, "verbose") == 0 ) {
				p_parameters->verbose = true;
			} else if( strcmp(long_options[option_index].name, "force") == 0 ) {
				p_parameters->force = true;
			} else if( strcmp(long_options[option_index].name, "header") == 0 ) {
				sscanf(optarg, "%d", &p_parameters->bin_header);
			} else if( strcmp(long_options[option_index].name, "footer") == 0 ) {
				sscanf(optarg, "%d", &p_parameters->bin_footer);
			} else if( strcmp(long_options[option_index].name, "min") == 0 ) {
				sscanf(optarg, "%lf", &p_parameters->to_min);
				p_parameters->to_use_min = true;
			} else if( strcmp(long_options[option_index].name, "max") == 0 ) {
				sscanf(optarg, "%lf", &p_parameters->to_max);
				p_parameters->to_use_max = true;
			}
			break;
			
		    case 's':
			sscanf(optarg, "%d", &p_parameters->bin_width);
			p_parameters->bin_height = p_parameters->bin_width;
			break;
		    case 'r':
			sscanf(optarg, "%d", &p_parameters->to_width);
			p_parameters->to_height = p_parameters->to_width;
			break;
		    case 't':
			p_parameters->bin_type = optarg[0];
			break;
		    case 'f':
			p_parameters->to_func = optarg;
			break;
		    case 'a':
			sscanf(optarg, "%lf", &p_parameters->to_amp);
			break;
		    case '?':
			printf("Unknown option %d.\n", optopt);
        	    case 'h':
			display_help(argv[0]);
			exit(0);
        	    case 'v':
			display_version(argv[0]);
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
void process_file(char *filename_bin, bin2gif_parameters p_parameters)
{
	if ( fs::is_dir(filename_bin) ) {
		if ( p_parameters.verbose ) {
			//printf("Directory %s: \033[90G\033[1;33m[Skipped]\033[0m\n", filename_bin);
		}
		return;
	} else if ( strstr(filename_bin, ".gif") != NULL ) {
		if ( p_parameters.verbose ) {
			//printf("File %s: \033[90G\033[1;33m[Skipped]\033[0m\n", filename_bin);
		}
		return;
	}
	char filename_gif[1024];
	char filename_type_fix[32];
	sprintf(filename_type_fix, "_%s%s.gif", p_parameters.to_func, ( p_parameters.to_fixphase && ( strcmp(p_parameters.to_func, "arg") == 0 ) ) ? "_fp" : "");
	char* ch;
	
	filename_gif[0] = '\0';
	ch = strrchr(filename_bin, '.');
	strncat(filename_gif, filename_bin, ch - filename_bin );
	
	strcat(filename_gif, filename_type_fix);
	
	printf("File %s:\n", filename_bin);
	
	if ( fs::file_exists(filename_gif) && !p_parameters.force ) {
		//printf("\033[90G\033[0;33m[GIF file already exists]\033[0m\n");
	} else if ( visual::convert_binary_file_to_gif(filename_bin, filename_gif, p_parameters) == 0 ) {
		printf("  -> %s\n", filename_gif);
		//printf("\033[90G\033[0;32m[Done]\033[0m\n");
		
		if ( p_parameters.delete_original ) {
			char* rm_cmd = new char[1024];
			sprintf(rm_cmd, "rm -f %s", filename_bin);
			system(rm_cmd);
		}
		
	} else {
		//printf("\033[90G\033[0;31m[Failed]\033[0m\n");
	}
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int i = 0, j = 0;
	
	char *filename_bin = new char[1024];
	if ( !filename_bin ) {
		printf("Error: Not enough memory for temporary string o_0. \033[70G\033[0;31m[Failed]\033[0m\n");
		return 1;
	}
	
	glob_t globbuf;
	globbuf.gl_offs = 0;
	
	bin2gif_parameters p_parameters;
	
	p_parameters.file_patterns_count = 0;
	
	p_parameters.delete_original = false;
	
	// No print debug output by default
	p_parameters.debug = false;
	// No show verbosely output
	p_parameters.verbose = false;
	// Don't rewrite existing gif's
	p_parameters.force = false;
	
	p_parameters.bin_width = -1;  // Autodetect
	p_parameters.bin_height = -1; // Autodetect
	p_parameters.bin_type = ' ';   // Autodetect

	p_parameters.bin_header = 0;
	p_parameters.bin_footer = 0;
	
	p_parameters.to_width = -1;  // No resize
	p_parameters.to_height = -1; // No resize
	p_parameters.to_reflect = false;
	p_parameters.to_fixphase = false;
	
	p_parameters.to_func = "real";
	p_parameters.to_amp = -1;
	p_parameters.to_use_min = false;
	p_parameters.to_use_max = false;
	
	p_parameters.palette_file = 0;
	
	// Parse program command line options
	get_program_options(argc, argv, &p_parameters);
	
	// Confirm originals deletion
	if ( p_parameters.delete_original ) {
		printf("Are you sure to delete original binary files after convertion[y/N]: ");
		char deletion_confirm[256] = {'\0'};
		scanf("%s", deletion_confirm);
		if ( deletion_confirm[0] == 'y' || deletion_confirm[0] == 'Y' ) {
			p_parameters.delete_original = true;
			printf(" => Yes\n");
		} else {
			p_parameters.delete_original = false;
			printf(" => No\n");
		}
	}
	
	// Init color palette
	visual::init_color_palette(p_parameters.palette_file);

	// Debug {{{
	if ( p_parameters.debug ) {
	
		//printf("\033[0;33mDebug {{{\n");
		
		printf("Header: %d\n", p_parameters.bin_header);
		printf("Footer: %d\n", p_parameters.bin_footer);
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
		
		//printf("Debug }}}\033[0m\n");
		
	}
	// Debug }}}

	for ( i = 0; i < p_parameters.file_patterns_count; i++ ) {
	
		glob(p_parameters.file_patterns[i], GLOB_DOOFFS, NULL, &globbuf);
		
		for ( j = 0; j < globbuf.gl_pathc; j++ ) {
			
			if ( fs::is_dir(globbuf.gl_pathv[j]) ) { // Directory
			
				dirent *de;
				DIR *dp;
			
				if ( !(dp = opendir(globbuf.gl_pathv[j])) ) {
					//printf("\033[0;31m[Error:\033[0m Cannot read directory %s.\n", globbuf.gl_pathv[j]);
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

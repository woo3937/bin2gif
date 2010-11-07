/*
Copyright (C) 2009, Oleg Efimov <efimovov@yandex.ru>

See license text in LICENSE file
*/
//---------------------------------------------------------------------------
#include <omp.h>
//---------------------------------------------------------------------------
#include <getopt.h>
#include <dirent.h>
#include <glob.h>
#include <string.h>
//---------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
//---------------------------------------------------------------------------
#include "./parameters.h"
#include "./util_fs.h"
#include "./util_visualize.h"
//---------------------------------------------------------------------------
#define VERSION "0.5"
#define AUTHOR "Oleg Efimov"
#define BUGREPORT_EMAIL "efimovov@yandex.ru"
//---------------------------------------------------------------------------
const char* get_program_name(const char *argv0) {
    char* ch = strrchr(const_cast<char*>(argv0), '/');
    if (ch != NULL) {
        return ch + 1;
    } else {
        return argv0;
    }
}
//---------------------------------------------------------------------------
void display_version(const char *argv0) {
    printf("%s version %s\n", get_program_name(argv0), VERSION);
    printf("Utility to convert binary 2D data file into GIF images.\n");
    printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");

    printf("\nAuthor: %s.\n", AUTHOR);
    printf("Report bugs to <%s>.\n", BUGREPORT_EMAIL);
}
//---------------------------------------------------------------------------
void display_help(const char *argv0) {
    const char* program_name = get_program_name(argv0);

    printf("Usage: %s [options] [--] <filename|dirname|pattern>\n", program_name); // NOLINT
    printf("Utility to convert binary 2D data file into GIF images.\n");
    printf("Supports 'double' and 'complex<double>' C/C++ data types.\n");

    printf("\nOptions:\n");
    printf("    -s, --size <num>                     dimensions of data in binary file\n"); // NOLINT
    printf("    -r, --resize <num>                   dimensions of produced image\n"); // NOLINT
    printf("    -t, --type (double|d|complex|c)      type of binary data\n");
    printf("    -f, --func (abs|norm|real|imag|arg)  function for complex to real conversion\n"); // NOLINT
    printf("    -a, --amp <double>                   value of image color scale amplitude\n"); // NOLINT
    printf("    -e                                   set amplitude to e^-1\n");
    printf("    --min <double>                       value of image color scale minimum\n"); // NOLINT
    printf("    --max <double>                       value of image color scale maximum\n"); // NOLINT
    printf("    --reflect                            reflect image, swaps x and y coords\n"); // NOLINT
    printf("    --palette <filename>                 color palette filename\n");
    printf("    --axial                              color palette filename\n\n"); // NOLINT

    printf("    --header <num>                       size of file header in bytes\n"); // NOLINT
    printf("    --footer <num>                       size of file footer in bytes\n"); // NOLINT
    printf("    --delete-original                    delete original file after convert\n"); // NOLINT
    printf("    --debug                              do debug output\n");
    printf("    --force                              force rewrite existing GIF files\n"); // NOLINT
    printf("    --verbose                            verbosely output\n\n");
    printf("    -v, --version                        display program vesion\n");
    printf("    -h, --help                           display this help page\n");

    printf("\nExamples:\n");
    printf("    %s ~/results/today/*.cpl\n",
           program_name);
    printf("    %s --header 8 ~/results/today/ ~/results/tomorrow/file.ext\n",
           program_name);
}
//---------------------------------------------------------------------------
void get_program_options(int argc, char *argv[],
                         sns::bin2gif_parameters *p_params) {
    int c = 0;
    opterr = 0;

    static struct option loptions[] = {
        {"size", required_argument, NULL, 's'},
        {"resize", required_argument, NULL, 'r'},
        {"type", required_argument, NULL, 't'},
        {"func", required_argument, NULL, 'f'},
        {"amp", required_argument, NULL, 'a'},
        {"min", required_argument, NULL, 0},
        {"max", required_argument, NULL, 0},
        {"reflect", no_argument, NULL, 0},
        {"palette", required_argument, NULL, 0},
        {"axial", no_argument, NULL, 0},
        {"axial-all", no_argument, NULL, 0},

        {"header", required_argument, NULL, 0},
        {"footer", required_argument, NULL, 0},

        {"delete-original", no_argument, NULL, 0},

        {"debug", no_argument, NULL, 0},
        {"verbose", no_argument, NULL, 0},
        {"force", no_argument, NULL, 0},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'}
    };
    int oindex = 0;

    while ((c = getopt_long_only(argc, argv, "s:r:t:f:a:ehvd",
                                 loptions, &oindex)) != -1) {
        switch (c) {
            case 0:
                if (        strcmp(loptions[oindex].name, "reflect") == 0) {
                    p_params->to_reflect = true;
                } else if (strcmp(loptions[oindex].name, "axial") == 0) {
                    p_params->bin_axial = true;
                } else if (strcmp(loptions[oindex].name, "axial-all") == 0) {
                    p_params->bin_axial_all = true;
                } else if (strcmp(loptions[oindex].name, "palette") == 0) {
                    p_params->palette_file = optarg;
                } else if (strcmp(loptions[oindex].name, "delete-original") == 0) { // NOLINT
                    p_params->delete_original = true;
                } else if (strcmp(loptions[oindex].name, "debug") == 0) {
                    p_params->debug = true;
                } else if (strcmp(loptions[oindex].name, "verbose") == 0) {
                    p_params->verbose = true;
                } else if (strcmp(loptions[oindex].name, "force") == 0) {
                    p_params->force = true;
                } else if (strcmp(loptions[oindex].name, "header") == 0) {
                    sscanf(optarg, "%d", &p_params->bin_header);
                } else if (strcmp(loptions[oindex].name, "footer") == 0) {
                    sscanf(optarg, "%d", &p_params->bin_footer);
                } else if (strcmp(loptions[oindex].name, "min") == 0) {
                    sscanf(optarg, "%lf", &p_params->to_min);
                    p_params->to_use_min = true;
                } else if (strcmp(loptions[oindex].name, "max") == 0) {
                    sscanf(optarg, "%lf", &p_params->to_max);
                    p_params->to_use_max = true;
                }
                break;
            case 's':
                sscanf(optarg, "%dx%d",
                       &p_params->bin_width, &p_params->bin_height);
                break;
            case 'r':
                sscanf(optarg, "%dx%d",
                       &p_params->to_width, &p_params->to_height);
                break;
            case 't':
                p_params->bin_type = optarg[0];
                break;
            case 'f':
                p_params->to_func = optarg;
                break;
            case 'a':
                sscanf(optarg, "%lf", &p_params->to_amp);
                break;
            case 'e':
                p_params->to_amp_e = true;
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

    if (optind < argc) {
        p_params->file_patterns_count = argc - optind;
        p_params->file_patterns = new char*[p_params->file_patterns_count];
        while (optind < argc) {
            p_params->file_patterns[argc-optind-1] = argv[optind];
            optind++;
        }
    } else {
        display_help(argv[0]);
            exit(0);
    }
}
//---------------------------------------------------------------------------
void process_file(char *filename_bin, sns::bin2gif_parameters *p_params) {
    if (sns::fs::is_dir(filename_bin)) {
        if (p_params->verbose) {
            // printf("Directory %s: \033[90G\033[1;33m[Skipped]\033[0m\n", filename_bin); // NOLINT
        }
        return;
    } else if (strstr(filename_bin, ".gif") != NULL) {
        if (p_params->verbose) {
            // printf("File %s: \033[90G\033[1;33m[Skipped]\033[0m\n", filename_bin); // NOLINT
        }
        return;
    }
    char filename_gif[1024];
    char filename_type_fix[32];
    snprintf(filename_type_fix, sizeof(filename_type_fix),
             "_%s.gif", p_params->to_func);
    char* ch;

    filename_gif[0] = '\0';
    ch = strrchr(filename_bin, '.');
    strncat(filename_gif, filename_bin, ch - filename_bin);

    strncat(filename_gif, filename_type_fix, sizeof(filename_type_fix));

    printf("File %s:\n", filename_bin);

    if (sns::fs::file_exists(filename_gif) && !p_params->force) {
        // printf("\033[90G\033[0;33m[GIF file already exists]\033[0m\n");
    } else if (sns::visual::convert_binary_file_to_gif(filename_bin, filename_gif,
                                                   p_params) == 0) {
        printf("  -> %s\n", filename_gif);
        // printf("\033[90G\033[0;32m[Done]\033[0m\n");

        if (p_params->delete_original) {
            char* rm_cmd = new char[1024];
            snprintf(rm_cmd, sizeof(rm_cmd), "rm -f %s", filename_bin);
            system(rm_cmd);
        }
    } else {
        // printf("\033[90G\033[0;31m[Failed]\033[0m\n");
    }
}
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    unsigned int i = 0, j = 0;

    char *filename_bin = new char[1024];
    if (!filename_bin) {
        printf("Error: Not enough memory for temporary string o_0. \033[70G\033[0;31m[Failed]\033[0m\n"); // NOLINT
        return 1;
    }

    glob_t globbuf;
    globbuf.gl_offs = 0;

    sns::bin2gif_parameters p_params;

    p_params.file_patterns_count = 0;

    p_params.delete_original = false;

    // No print debug output by default
    p_params.debug = false;
    // No show verbosely output
    p_params.verbose = false;
    // Don't rewrite existing gif's
    p_params.force = false;

    p_params.bin_width = -1;   // Autodetect
    p_params.bin_height = -1;  // Autodetect
    p_params.bin_type = ' ';   // Autodetect
    p_params.bin_axial = false;      // Standart square matrix
    p_params.bin_axial_all = false;  // Standart square matrix

    p_params.bin_header = 0;
    p_params.bin_footer = 0;

    p_params.to_width = -1;   // No resize
    p_params.to_height = -1;  // No resize
    p_params.to_reflect = false;

    p_params.to_func = const_cast<char*>("real");
    p_params.to_amp = -1;
    p_params.to_amp_e = false;
    p_params.to_use_min = false;
    p_params.to_use_max = false;

    p_params.palette_file = 0;

    // Parse program command line options
    get_program_options(argc, argv, &p_params);

    if (p_params.bin_axial && p_params.bin_axial_all) {
        printf("You should use only one option at same time: --axial OR --axial-all"); // NOLINT
        return 1;
    }

    // Confirm originals deletion
    if (p_params.delete_original) {
        printf("Are you sure to delete original binary files after convertion[y/N]: "); // NOLINT
        char deletion_confirm[256] = {'\0'};
        scanf("%s", deletion_confirm);
        if (deletion_confirm[0] == 'y' || deletion_confirm[0] == 'Y') {
            p_params.delete_original = true;
            printf(" => Yes\n");
        } else {
            p_params.delete_original = false;
            printf(" => No\n");
        }
    }

    // Init color palette
    sns::visual::init_color_palette(p_params.palette_file);

    // Debug {{{
    if (p_params.debug) {
        // printf("\033[0;33mDebug {{{\n");

        printf("Header: %d\n", p_params.bin_header);
        printf("Footer: %d\n", p_params.bin_footer);
        printf("Files/directories to process:\n");

        for (i = 0; i < p_params.file_patterns_count; i++) {
            printf("\t%s\n", p_params.file_patterns[i]);
        }

        for (i = 0; i < p_params.file_patterns_count; i++) {
            glob(p_params.file_patterns[i], GLOB_DOOFFS, NULL, &globbuf);
            printf("%s:\n", p_params.file_patterns[i]);
            for (j = 0; j < globbuf.gl_pathc; j++) {
                printf("\t%s\n", globbuf.gl_pathv[j]);
            }
        }
        // printf("Debug }}}\033[0m\n");
    }
    // Debug }}}

    for (i = 0; i < p_params.file_patterns_count; i++) {
        glob(p_params.file_patterns[i], GLOB_DOOFFS, NULL, &globbuf);

        for (j = 0; j < globbuf.gl_pathc; j++) {
            if (sns::fs::is_dir(globbuf.gl_pathv[j])) {  // Directory
                dirent *de;
                DIR *dp;

                if (!(dp = opendir(globbuf.gl_pathv[j]))) {
                    // printf("\033[0;31m[Error:\033[0m Cannot read directory %s.\n", globbuf.gl_pathv[j]); // NOLINT
                    return 1;
                }

                printf("Processing %s:\n", globbuf.gl_pathv[j]);

                while ((de = readdir(dp))) {
                    snprintf(filename_bin, sizeof(filename_bin),
                             "%s", globbuf.gl_pathv[j]);
                    if (filename_bin[strlen(filename_bin)-1] != '/') {
                        strcpy(filename_bin + strlen(filename_bin), "/");
                    }
                    strcpy(filename_bin + strlen(filename_bin), de->d_name);

                    process_file(filename_bin, &p_params);
                }

                closedir(dp);
            } else {  // Maybe file?
                process_file(globbuf.gl_pathv[j], &p_params);
            }
        }
    }

    return 0;
}

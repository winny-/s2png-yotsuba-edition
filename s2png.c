/*
 *   s2png - "something to png" copyright (c) 2006 k0wax, winny
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *   (at your option) any later version.                                 
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include <err.h>
#include <sysexits.h>
#include <stdlib.h>

#include "gd.h"
#include "gdfontt.h"


#define VERSION_STR ("0.02")
#define VERSION_DATE ("20130317")
#define BANNER_HEIGHT 8
#define MODE_ENCODE 1
#define MODE_DECODE 2

static int verbose = 0;

int pngtofile(FILE *fin, FILE *fout)
{
    gdImagePtr im = gdImageCreateFromPng(fin);

    int c = gdImageGetPixel(im, gdImageSX(im) - 1, gdImageSY(im) - 1);
    int data_size = (gdImageRed(im, c) << 8*2) + (gdImageGreen(im, c) << 8*1) + (gdImageBlue(im, c));

    unsigned char buf[3];
    long written_bytes = 0;
    int x, y;
    int nb = 0;
    for(y = 0; y < gdImageSY(im); y++) {
        for(x = 0; x < gdImageSX(im); x++) {
            c = gdImageGetPixel(im, x, y);
            buf[0] = gdImageRed(im, c);
            buf[1] = gdImageGreen(im, c);
            buf[2] = gdImageBlue(im, c);
            if(written_bytes >= data_size) {
                break; /* FIXME */
            } else {
                nb = written_bytes + 3 > data_size ?
                    data_size - written_bytes : 3;
                written_bytes += fwrite(buf, 1, nb, fout);
            }
        }
    }

    gdImageDestroy(im);
    return 1;
}

int filetopng(FILE *fin, FILE *fout, int im_w, char *banner)
{
    struct stat fin_stat;
    gdImagePtr im = NULL; 

    fstat(fileno(fin), &fin_stat);
    long data_size = fin_stat.st_size;

    int im_w_bytes = im_w * 3;
    int im_h = ((data_size + im_w_bytes - 1) / im_w_bytes) + BANNER_HEIGHT;  /* ceil((float)data_size / im_w_bytes) + BANNER_HEIGHT */
    im = gdImageCreateTrueColor(im_w, im_h);

    unsigned char buf[3];
    long bytes_read = 0;
    long total_bytes = 0;
    int x = 0;
    int y = 0;
    while((bytes_read = fread(buf, 1, 3, fin)) > 0) {
        total_bytes += bytes_read;
        gdImageSetPixel(im, x, y, gdImageColorAllocate(im, buf[0], buf[1], buf[2]));

        if(x + 1 < im_w) {
            x++;
        } else {
            x = 0;
            y++;
        }
    }

    gdImageFilledRectangle(im, 0, gdImageSY(im) - BANNER_HEIGHT,
        im_w - 1, gdImageSY(im) + BANNER_HEIGHT, gdImageColorAllocate(im, 255, 255, 255));
    gdImageString(im, (gdFontPtr) gdFontGetTiny(), 5, gdImageSY(im) - BANNER_HEIGHT,
        (unsigned char *)banner, gdImageColorAllocate(im, 0, 0, 0));
    /* store data_size at last pixel */
    gdImageSetPixel(im, gdImageSX(im) - 1, gdImageSY(im) - 1,
        gdImageColorAllocate(im, (data_size & 0xff0000) >> 8*2, (data_size & 0xff00) >> 8*1, data_size & 0xff));
    
    if(verbose)
        fprintf(stderr, "Width:  %d\nHeight: %d\n", im_w, im_h);

    gdImagePng(im, fout);

/*
    int c = gdImageGetPixel(im, gdImageSX(im) - 1, gdImageSY(im) - 1);
    int ds = (gdImageRed(im, c) << 8*2) + (gdImageGreen(im, c) << 8*1) + (gdImageBlue(im, c));
    printf("debug: ds %d, data_size %d\n", ds, data_size);
    //printf("c: %d %d %d\n", (data_size & 0xff0000) >> 8*2, (data_size & 0xff00) >> 8*1, data_size & 0xff);
    //printf("d: %d %d %d\n", (gdImageRed(im, c) << 8*2), (gdImageGreen(im, c) << 8*1), (gdImageBlue(im, c)));
*/

    gdImageDestroy(im);
    return 1;
}

int is_png_file(char *filename)
{
    char png_sign[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    char buf[8];

    FILE *fp = fopen(filename, "rb"); 
    fread(buf, 8, 1, fp);
    fclose(fp);

    return(memcmp(buf, png_sign, 8) == 0 ? 1 : 0);
}

void usage()
{
    printf("s2png \"something to png\" version %s\n", VERSION_STR);
    printf("usage: s2png [-h] [-v] [-o filename] [-w width (600)] [-e (default) | -d] [-b banner] file\n");
    exit(EX_USAGE);
}

int main(int argc, char **argv)
{
    char *in_fn = NULL;
    char *out_fn = NULL;
    char *banner = NULL;
    int im_w = 600;
    /* verbose is declared statically at the top of this file. */
    int mode;
    int modeset = 0;

    int ch;
    while((ch = getopt(argc, argv, "w:o:hvedb:")) != -1) {
        switch(ch) {
            case 'w':
                im_w = atoi(optarg);
                if (im_w < 1) {
                    fprintf(stderr, "Image width is set to %d, but it must be a positive integer.\n", im_w);
                    exit(1);
                }
                break;
            case 'o':
                out_fn = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'e':
                if(modeset)
                    usage();
                mode = MODE_ENCODE;
                modeset = 1;
                break;
            case 'd':
                if(modeset)
                    usage();
                mode = MODE_DECODE;
                modeset = 1;
                break;
            case 'b':
                banner = optarg;
                break;
            case 'h':
            default:
                usage();
                /* Not reached */
        }
    }

    if (optind+1 == argc) {
        in_fn = argv[argc-1];
    } else {
        usage();
    }
    
    int use_stdin = strcmp(in_fn, "-") == 0 ? 1 : 0;
    int use_stdout = (out_fn != NULL && strcmp(out_fn, "-") == 0) ? 1 : 0;
    
    if(!use_stdin && access(in_fn, R_OK) < 0)
        err(EX_NOINPUT, "%s", in_fn);

    if(!modeset && use_stdin) {
        fprintf(stderr, "To use stdin, you MUST specify -e or -d.\n");
        exit(EX_USAGE);
    }
    if(!modeset)
        mode = is_png_file(in_fn) ? MODE_DECODE : MODE_ENCODE;

    if(mode == MODE_DECODE && out_fn == NULL) {
        out_fn = calloc(1, sizeof(char) * 255);
        if(strcasecmp(in_fn + strlen(in_fn) - 4, ".png") == 0) {
            strncpy(out_fn, in_fn, strlen(in_fn) - 4);
        } else {
            strcpy(out_fn, in_fn);
            strcat(out_fn, ".orig");
            fprintf(stderr, "warn: file `%s' will be saved as `%s'\n", in_fn, out_fn);
        }
    } else if(mode == MODE_ENCODE && out_fn == NULL) {
        out_fn = calloc(1, sizeof(char) * strlen(in_fn) + sizeof(".png"));
        strcpy(out_fn, in_fn);
        strcat(out_fn, ".png");
    }

    if(verbose) {
        char modestring[20] = "";
        if(mode == MODE_ENCODE) {
            strcat(modestring, "Encode");
        } else if(mode == MODE_DECODE) {
            strcat(modestring, "Decode");
        }
        fprintf(stderr, "%s -> %s\nMode:   %s\n", in_fn, out_fn, modestring);
    }

    FILE *fin;
    FILE *fout;
    if (use_stdin) {
        fin = stdin;
    } else {
        fin = fopen(in_fn, "rb");
    }
    if (use_stdout) {
        fout = stdout;
    } else {
        fout = fopen(out_fn, "wb");
    }
    if(mode == MODE_ENCODE) {
        if(banner == NULL) {
            banner = calloc(1, sizeof(char) * 255);
            strcat(banner, "Created using s2png http://s2png.sf.net || ");
            strcat(banner, in_fn);
        }
        filetopng(fin, fout, im_w, banner);
    } else if (mode == MODE_DECODE) {
        pngtofile(fin, fout);
    }

    fclose(fin);
    fclose(fout);
    return(EXIT_SUCCESS);
}

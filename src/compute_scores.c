#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <png.h>

png_bytep read_png(char *png_filename, int *width, int *height) {
    FILE *fp = fopen(png_filename, "rb");
    if (!fp) {
        perror("Failed to open file for input");
        return false;
    }
    
    // Use the simplified API introduced in libpng 1.6
    png_image im;
    png_bytep image_data = NULL;
    im.version = PNG_IMAGE_VERSION;
    im.opaque = NULL;

    png_image_begin_read_from_stdio(&im, fp);
    if (im.warning_or_error) {
        fprintf(stderr, "%s\n", im.message);
        if (im.warning_or_error & 2) goto end;
    }

    *width = im.width;
    *height = im.height;
    
    image_data = malloc(PNG_IMAGE_SIZE(im));
    im.format = PNG_FORMAT_RGB;
    png_image_finish_read(&im, NULL, image_data, PNG_IMAGE_ROW_STRIDE(im), NULL);

    if (im.warning_or_error) {
        fprintf(stderr, "%s\n", im.message);
        if (im.warning_or_error & 2) goto end;
    }

    end:
    fclose(fp);
    if (im.warning_or_error & 2) { free(image_data); return NULL; }
    return image_data;
}

int pixel_score(png_bytep p1, png_bytep p2) {
    // return (p1[0] - p2[0])*(p1[0] - p2[0]) + (p1[1] - p2[1])*(p1[1] - p2[1]) + (p1[2] - p2[2])*(p1[2] - p2[2]);
    return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]) + abs(p1[2] - p2[2]);
}

int col_score(png_bytep image, int width, int height, int col1, int col2) {
    int score = 0;
    for (int row = 0; row < height; ++row)
        score += pixel_score(image + 3*width*row + 3*col1, image + 3*width*row + 3*col2);
    return score;
}

int row_score(png_bytep image, int width, int height, int row1, int row2) {
    int score = 0;
    for (int col = 0; col < width; ++col)
        score += pixel_score(image + 3*width*row1 + 3*col, image + 3*width*row2 + 3*col);
    return score;
}

void write_tsplib_header(char *filename, int dimension) {
    printf("NAME : column similarity for %s\n", filename);
    printf("TYPE : TSP\n");
    printf("DIMENSION : %d\n", dimension);
    printf("EDGE_WEIGHT_TYPE : EXPLICIT\n");
    printf("EDGE_WEIGHT_FORMAT : UPPER_ROW\n");
    printf("NODE_COORD_TYPE : NO_COORDS\n");
    printf("DISPLAY_DATA_TYPE : NO_DISPLAY\n");
    printf("\n");
    printf("EDGE_WEIGHT_SECTION :\n");
}

void write_tsplib_cols(png_bytep image, char *filename, int width, int height) {
    write_tsplib_header(filename, width + 1);

    for (int i = 0; i < width; ++i) printf("0 ");
    printf("\n");

    for (int col1 = 0; col1 < width; ++col1)
    {
        for (int col2 = col1 + 1; col2 < width; ++col2)
            printf("%d ", col_score(image, width, height, col1, col2));
        printf("\n");
    }
}

void write_tsplib_rows(png_bytep image, char *filename, int width, int height) {
    write_tsplib_header(filename, height + 1);

    for (int i = 0; i < height; ++i) printf("0 ");
    printf("\n");

    for (int row1 = 0; row1 < height; ++row1)
    {
        for (int row2 = row1 + 1; row2 < height; ++row2)
            printf("%d ", row_score(image, width, height, row1, row2));
        printf("\n");
    }
}

void die_usage(char *prog_name) {
    fprintf(stderr,"Usage: %s [--rows | --cols] input.png\n", prog_name);
    exit(1);
}

int main(int argc, char **argv)
{
    static struct option longopts[] = {
         { "rows", no_argument, NULL, 'r' }, /* Use rows */
         { "cols", no_argument, NULL, 'c' }, /* Use columns */
         { NULL, 0, NULL, 0 }
    };
    char ch;

    bool use_rows = false,
         use_cols = false;
    while ((ch = getopt_long(argc, argv, "rc", longopts, NULL)) != -1) {
        switch (ch) {
            case 'r':
                if (use_cols) die_usage(argv[0]);
                use_rows = true;
                break;
            case 'c':
                if (use_rows) die_usage(argv[0]);
                use_cols = true;
                break;
            default:
                die_usage(argv[0]);
        }
    }
    if (!use_rows && !use_cols) use_cols = true;
    
    char **input_filenames = argv + optind;
    int num_input_filenames = argc - optind;

    if (num_input_filenames != 1) die_usage(argv[0]);
    char *image_filename = input_filenames[0];

    int width, height;
    png_bytep image = read_png(image_filename, &width, &height);
    if (image == NULL) {
        fprintf(stderr, "%s: Failed to read image\n", argv[0]);
        exit(1);
    }

    if (use_cols)
        write_tsplib_cols(image, image_filename, width, height);
    else
        write_tsplib_rows(image, image_filename, width, height);
    
    return 0;
}

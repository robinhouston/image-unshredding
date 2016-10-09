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

int pixelScore(png_bytep p1, png_bytep p2) {
    // return (p1[0] - p2[0])*(p1[0] - p2[0]) + (p1[1] - p2[1])*(p1[1] - p2[1]) + (p1[2] - p2[2])*(p1[2] - p2[2]);
    return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]) + abs(p1[2] - p2[2]);
}

int score(png_bytep image, int width, int height, int col1, int col2) {
    int score = 0;
    for (int row = 0; row < height; ++row)
        score += pixelScore(image + 3*width*row + 3*col1, image + 3*width*row + 3*col2);
    return score;
}

void write_tsplib(png_bytep image, char *filename, int width, int height) {
    printf("NAME : column similarity for %s\n", filename);
    printf("TYPE : TSP\n");
    printf("DIMENSION : %d\n", width + 1);
    printf("EDGE_WEIGHT_TYPE : EXPLICIT\n");
    printf("EDGE_WEIGHT_FORMAT : UPPER_ROW\n");
    printf("NODE_COORD_TYPE : NO_COORDS\n");
    printf("DISPLAY_DATA_TYPE : NO_DISPLAY\n");
    printf("\n");
    printf("EDGE_WEIGHT_SECTION :\n");

    for (int i = 0; i < width; ++i) printf("0 ");
    printf("\n");

    for (int col1 = 0; col1 < width; ++col1)
    {
        for (int col2 = col1 + 1; col2 < width; ++col2)
            printf("%d ", score(image, width, height, col1, col2));
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr,"Usage: %s input.png\n", argv[0]);
        exit(1);
    }
    char *image_filename = argv[1];
    int width, height;

    png_bytep image = read_png(image_filename, &width, &height);
    if (image == NULL) {
        fprintf(stderr, "%s: Failed to read image\n", argv[0]);
        exit(1);
    }

    write_tsplib(image, image_filename, width, height);    
    
    return 0;
}

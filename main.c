#include "bmp.h"
#include "transformations.h"

int main(int argc, char *argv[]){
    char *input_file = NULL;
    char *output_file = NULL;

    if( argc < 6) {
        fprintf(stderr, "The minimal number of arguments is 6.\nUsage: $ ./bmp -i <input_file.bmp> -o <output_file.bmp> -[option]\n");
        fprintf(stderr, "list of options: -r -l -v -h -c -s -e\n");
        return 1;
    }
    
    // LOAD INPUT AND OUTPUT FILES
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input_file = argv[i + 1];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[i + 1];
        }
    }

    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr,"Input and output filenames are required.\n");
        fprintf(stderr,"$ ./bmp -i <input_file.bmp> -o <output_file.bmp> -[option]\n");
        return 1;
    }

    // READ BMP IMAGE
    FILE *stream = fopen(input_file, "rb");
    struct bmp_image *image = read_bmp(stream);
    fclose(stream);

    for (int i = 1; i < argc; i++) {
            //  ROTATE RIGHT
            if (strcmp(argv[i], "-r") == 0) {
                image = rotate_right(image);
            }
            // ROTATE LEFT
            if (strcmp(argv[i], "-l") == 0) {
                image = rotate_left(image);
            }
            // FLIP HORIZONTALLY
            if (strcmp(argv[i], "-h") == 0) {
                image = flip_horizontally(image);
            }
            // FLIP VERTICALLY  
            if (strcmp(argv[i], "-v") == 0) {
                image = flip_vertically(image);
            }
            // CROP
            if (strcmp(argv[i], "-c") == 0) {
                if(i+4 < argc){
                    uint32_t start_y = (uint32_t)atoi(argv[i+1]);
                    uint32_t start_x = (uint32_t)atoi(argv[i+2]);
                    uint32_t height = (uint32_t)atoi(argv[i+3]);
                    uint32_t width = (uint32_t)atoi(argv[i+4]);
                    image = crop(image, start_y, start_x, height, width);
                }
                else{
                    fprintf(stderr, "Usage: $ ./bmp -i <input_file.bmp> -o <output_file.bmp> -c start_y start_y height width\n");
                }
            }
            // EXTRACT
            if (strcmp(argv[i], "-e") == 0) {
                if(i+1 < argc){
                    image = extract(image, argv[i+1]);
                }
                else{
                    fprintf(stderr, "Usage: $ ./bmp -i <input_file.bmp> -o <output_file.bmp> -e rgb\n");
                }
            }
            // SCALE
            if (strcmp(argv[i], "-s") == 0) {
                if(i+1 < argc){
                    float factor = (float)atof(argv[i+1]);
                    image = scale(image, factor);
                }
                else{
                    fprintf(stderr, "Usage: $ ./bmp -i <input_file.bmp> -o <output_file.bmp> -s 1.5\n");
                }
            }
        }

    // SAVE IMAGE
    FILE *stream_write = fopen(output_file, "wb");
    bool save = write_bmp(stream_write, image);
    if (!save) {
        fprintf(stderr, "Error writing BMP image\n");
    }
    fclose(stream_write);
    free(image);

    return 0;
}
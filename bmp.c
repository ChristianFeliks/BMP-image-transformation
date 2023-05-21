#include "bmp.h"

struct bmp_header* read_bmp_header(FILE* stream){

    if(stream == NULL){
        return NULL;
    }
    //! SET to the beginning of the file
    fseek(stream, 0, SEEK_SET);
    struct bmp_header *header = calloc(1 , sizeof(struct bmp_header));
    // this will load whole header 
    //* fread(header, sizeof(struct bmp_header), 1, stream);

    //? Read the BMP header fields individually
    fread(&header->type, sizeof(uint16_t), 1, stream);

    //? Check if the image is a valid BMP file
    if (header->type != 0x4D42) {
        free(header);
        return NULL; //! not a BMP file
    }
    
    // this loads header separately
    fread(&header->size, sizeof(uint32_t), 1, stream);
    fread(&header->reserved1, sizeof(uint16_t), 1, stream);
    fread(&header->reserved2, sizeof(uint16_t), 1, stream);
    fread(&header->offset, sizeof(uint32_t), 1, stream);
    fread(&header->dib_size, sizeof(uint32_t), 1, stream);
    fread(&header->width, sizeof(uint32_t), 1, stream);
    fread(&header->height, sizeof(uint32_t), 1, stream);
    fread(&header->planes, sizeof(uint16_t), 1, stream);
    fread(&header->bpp, sizeof(uint16_t), 1, stream);
    fread(&header->compression, sizeof(uint32_t), 1, stream);
    fread(&header->image_size, sizeof(uint32_t), 1, stream);
    fread(&header->x_ppm, sizeof(uint32_t), 1, stream);
    fread(&header->y_ppm, sizeof(uint32_t), 1, stream);
    fread(&header->num_colors, sizeof(uint32_t), 1, stream);
    fread(&header->important_colors, sizeof(uint32_t), 1, stream);

    return header;
}

struct pixel* read_data(FILE* stream, const struct bmp_header* header){

    if(stream == NULL){
        return NULL;
    }
    if(header == NULL){
        return NULL;
    }
    //? Move the file pointer to the starting position of the pixel array
    //fseek(stream, header->offset, SEEK_SET);

    size_t num_pixels = (size_t)header->width * (size_t)header->height;

    struct pixel *pixels = calloc(num_pixels, sizeof(struct pixel));

    //? Calculate row padding
    int row_padding = (4 - ((header->width * sizeof(struct pixel)) % 4)) % 4;

    //? Read pixel data
    for (int y = 0; y < (int)header->height; y++) {
        for (int x = 0; x < (int)header->width; x++) {
            struct pixel *pixel = &pixels[y * (int)header->width + x];
            fread(&pixel->blue, sizeof(uint8_t), 1, stream);
            fread(&pixel->green, sizeof(uint8_t), 1, stream);
            fread(&pixel->red, sizeof(uint8_t), 1, stream);
        }
        fseek(stream, row_padding, SEEK_CUR);
    }

    return pixels;
}


struct bmp_image* read_bmp(FILE* stream){

    if(stream == NULL){
        return NULL;
    }
    //? Allocate memory for new image and set seek to beginning of the file
    struct bmp_image *new_image = calloc(1 ,sizeof(struct bmp_image));
    fseek(stream, 0, SEEK_SET);
    //? Load the header
    struct bmp_header *stream_header = read_bmp_header(stream);

    //? Check for NULL header
    if(stream_header == NULL){
        fprintf(stderr, "Error: This is not a BMP file.\n");
        free(new_image);
        free(stream_header);
        return NULL; //! Invalid stream
    }
    //? Set seek for reading data
    fseek(stream, stream_header->offset, SEEK_SET);
    struct pixel *stream_pixels = read_data(stream, stream_header);

    //? Append what was loaded to new_image
    new_image->header = stream_header;
    new_image->data = stream_pixels;
    
    return new_image;
}

bool write_bmp(FILE* stream, const struct bmp_image* image){

    if(stream == NULL){
        return false; 
    }
    if(image == NULL){
        return false;
    }

    //? Write BMP header
    fwrite(image->header, sizeof(struct bmp_header), 1, stream);

    int height = (int)image->header->height;
    int width = (int)image->header->width;
    int bpp = (int)image->header->bpp;

    //? Write BMP pixel data
    int row_padding = (4 - ((width * bpp / 8) % 4)) % 4;
    for (int i = 0; i < height; i++) { // Start from top row
        for (int j = 0; j < width; j++) {
            struct pixel p = image->data[i * width + j];
            fwrite(&p.blue, sizeof(uint8_t), 1, stream);
            fwrite(&p.green, sizeof(uint8_t), 1, stream);
            fwrite(&p.red, sizeof(uint8_t), 1, stream);
        }

        //? Write padding bytes
        for (int k = 0; k < row_padding; k++) {
            fputc(0x00, stream);
        }
    }

    return true;

}

void free_bmp_image(struct bmp_image* image){
    if(image != NULL){
        if(image->data != NULL){
            free(image->data);
        }
        if(image->header != NULL){
            free(image->header);
        }        
        free(image);
    }
}
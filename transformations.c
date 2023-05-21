#include "bmp.h"
#include "transformations.h"

struct bmp_image* flip_horizontally(const struct bmp_image* image){

    if (image == NULL) {
        return NULL;
    }
    
    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));
    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));
    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));
    //? Allocate memory for new image pixels
    copy->data = calloc(image->header->height * image->header->width, sizeof(struct pixel));
    //? Copy the BMP pixel data from the original image
    
    //? copy the pixel data, while flipping horizontally
    for (uint32_t i = 0; i < image->header->height; i++) {
        for (uint32_t j = 0; j < image->header->width; j++) {
            uint32_t index = i * image->header->width + j;
            uint32_t reverse_index = i * image->header->width + (image->header->width - 1 - j);
            copy->data[reverse_index] = image->data[index];
        }
    }
    return copy;
}

struct bmp_image* flip_vertically(const struct bmp_image* image){

    if (image == NULL) {
            return NULL;
        }
        
    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));
    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));
    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));
    //? Allocate memory for new image pixels
    copy->data = calloc(image->header->height * image->header->width, sizeof(struct pixel));
    
    int height = (int)image->header->height;
    int width = (int)image->header->width;
    
    //? Flip the image vertically
    uint32_t row_size_bytes = image->header->width * sizeof(struct pixel);
    uint8_t* row_buffer = (uint8_t*) calloc(1 ,row_size_bytes);

    for (int y = 0; y < height / 2; y++) {
        int top_row_index = y * width;
        int bottom_row_index = (height - y - 1) * width;

        //? Copy the top row to the buffer
        memcpy(row_buffer, &image->data[top_row_index], row_size_bytes);

        //? Copy the bottom row to the top row
        memcpy(&copy->data[top_row_index], &image->data[bottom_row_index], row_size_bytes);

        //? Copy the buffer (containing the original top row) to the bottom row
        memcpy(&copy->data[bottom_row_index], row_buffer, row_size_bytes);
    }

    free(row_buffer);

    return copy;

}

struct bmp_image* rotate_right(const struct bmp_image* image){

    if (image == NULL) {
            return NULL;
        }
        
    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));

    //? Save width and height of the original image
    uint32_t original_width = image->header->width;
    uint32_t original_height = image->header->height;

    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));

    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));

    //? Switch width and height values inside the header of the new copy
    copy->header->width = image->header->height;
    copy->header->height = image->header->width;

    //? Save width and height of thcopy->header->size e new imagenew_height
    int height = (int)copy->header->height;
    int width = (int)copy->header->width;

    //? adjust Size and Image_size
    double row_size = round(((24 * width + 31) / 32)) * 4;  // Calculate row size with padding
    double image_size = row_size * height;  // Calculate image size with padding
    copy->header->image_size = (uint32_t)image_size;
    double bmp_size = 54 + image_size;  // Add BMP header size (54 bytes)
    copy->header->size = (uint32_t)bmp_size;


    //? Allocate memory for new image pixels
    copy->data = calloc((size_t)(height * width), sizeof(struct pixel));

    //? Rotate the pixels
    for (uint32_t row = 0; row < original_height; row++) {
        for (uint32_t col = 0; col < original_width; col++) {
            uint32_t original_index = row * original_width + col;
            uint32_t rotated_index = (original_width - col - 1) * original_height + row;
            copy->data[rotated_index] = image->data[original_index];
        }
    }   

    return copy;
}

struct bmp_image* rotate_left(const struct bmp_image* image){

    if (image == NULL) {
            return NULL;
    }
    
    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));

    //? Save width and height of the original image
    uint32_t original_width = image->header->width;
    uint32_t original_height = image->header->height;

    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));

    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));

    //? Switch width and height values inside the header of the new copy
    copy->header->width = image->header->height;
    copy->header->height = image->header->width;

    //? Save width and height of the new image
    int height = (int)copy->header->height;
    int width = (int)copy->header->width;

    //? adjust Size and Image_size
    double row_size = round(((24 * width + 31) / 32)) * 4;  // Calculate row size with padding
    double image_size = row_size * height;  // Calculate image size with padding
    copy->header->image_size = (uint32_t)image_size;
    double bmp_size = 54 + image_size;  // Add BMP header size (54 bytes)
    copy->header->size = (uint32_t)bmp_size;



    //? Allocate memory for new image pixels
    copy->data = calloc((size_t)(height * width), sizeof(struct pixel));

    //? Rotate the pixels
    for (uint32_t row = 0; row < original_height; row++) {
        for (uint32_t col = 0; col < original_width; col++) {
            uint32_t original_index = row * original_width + col;
            uint32_t rotated_index = (original_height - row - 1) + col * original_height;
            copy->data[rotated_index] = image->data[original_index];
        }
    }

    return copy;
}

struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep){

    bool cervena = false;
    bool zelena = false;
    bool modra = false;

    if (image == NULL || colors_to_keep == NULL) {
            return NULL;
    }
    if(strlen(colors_to_keep) > 3){
        return NULL;
    }
    //? Check for wrong letters inside colors_to_keep
    for(int i = 0; i < strlen(colors_to_keep); i++){
        char c = colors_to_keep[i];

        if(c != 'r' && c != 'g' && c != 'b'){
            return NULL;
        }

        if(c == 'r'){
            cervena = true;
        }
        if(c == 'g'){
            zelena = true;
        }
        if(c == 'b'){
            modra = true;
        }
    }

    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));

    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));

    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));

    //? Save width and height of the new image
    int height = (int)copy->header->height;
    int width = (int)copy->header->width;

    //? Allocate memory for new image pixels
    copy->data = calloc((size_t)(height * width), sizeof(struct pixel));

    //? Calculate total pixels
    int num_pixels = height*width;
    
    
    /* 
    The `if` statements check if the corresponding
    color (red, green, or blue) should be kept,color is copied from theoriginal pixel to the new pixel.
    If a color is not specified in `colors_to_keep`, its value in the new pixel is set to 0. 
    */
    for(int i = 0; i < num_pixels; i++){

        if(cervena){
            copy->data[i].red = image->data[i].red;
        }
        if(zelena){
            copy->data[i].green = image->data[i].green;
        }
        if(modra){
            copy->data[i].blue = image->data[i].blue;
        }
    }

    return copy;
}


void reverse_rows(struct bmp_image* image) {
    if (image == NULL) {
        return;
    }
    
    uint32_t width = image->header->width;
    uint32_t height = image->header->height;
    
    struct pixel* new_data = calloc(width * height, sizeof(struct pixel));
    uint32_t new_index = 0;
    
    for (int row = (int)height - 1; row >= 0; row--) {
        for (int col = 0; col < width; col++) {
            new_data[new_index++] = image->data[row * (int)width + col];
        }
    }
    
    free(image->data);
    image->data = new_data;
}


struct bmp_image* crop(const struct bmp_image* image, const uint32_t start_y, const uint32_t start_x, const uint32_t height, const uint32_t width){
    
    if (image == NULL ) {
        return NULL;
    }

    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));

    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));

    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));

    //? Check if the start coordinates belongs inside the image
    if (start_x > image->header->width || start_y > image->header->height){
        free(copy->header);
        free(copy);
        return NULL;
    }
    //? Check if the cropped part belongs fully inside the original image
    if (start_x + width > image->header->width || start_y + height > image->header->height){
        free(copy->header);
        free(copy);
        return NULL;
    }

    //? Update header values for the cropped image
    copy->header->width = width;
    copy->header->height = height;
    double row_size = round(((24 * width + 31) / 32)) * 4;  // Calculate row size with padding
    double image_size = row_size * height;  // Calculate image size with padding
    copy->header->image_size = (uint32_t)image_size;
    double bmp_size = 54 + image_size;  // Add BMP header size (54 bytes)
    copy->header->size = (uint32_t)bmp_size;

    //? Allocate memory for new image pixels
    copy->data = calloc(height * width, sizeof(struct pixel));
    
    uint32_t original_width = image->header->width;

    //? Copy the pixels from the original image to the cropped image
    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t col = 0; col < width; col++) {
            uint32_t original_index = (image->header->height - start_y - row - 1) * original_width + (start_x + col);
            uint32_t cropped_index = row * width + col;
            copy->data[cropped_index] = image->data[original_index];
        }
    }
    reverse_rows(copy);
    
    return copy;
}

int find_closest_greater_multiple_of_four(int num) {
    if (num % 4 == 0) {
        return num;
    } else {
        return num + (4 - (num % 4));
    }
}

struct bmp_image* scale(const struct bmp_image* image, float factor){
    
    if (image == NULL || factor <= 0.0) {
        return NULL;
    }

    //? Allocate memory for new image struct
    struct bmp_image *copy = calloc(1 ,sizeof(struct bmp_image));

    //? Allocate memory for new image header
    copy->header = calloc(1, sizeof(struct bmp_header));

    //? Copy header
    memcpy(copy->header, image->header, sizeof(struct bmp_header));

    //? Declare old size parametres
    double old_height = (double)image->header->height;
    double old_width = (double)image->header->width;

    //? Calculate new_width and new_height
    double new_width = round(old_width * factor);
    double new_height = round(old_height * factor);

   //? Append new values into copy header
    copy->header->width = (uint32_t)new_width;
    copy->header->height = (uint32_t)new_height;
    
    int bytes_for_one_row = (int)new_width * 3;
    int row_bytes_with_padding = find_closest_greater_multiple_of_four(bytes_for_one_row);
    int total_bytes = row_bytes_with_padding * (int)new_height;
    copy->header->image_size = (uint32_t)total_bytes;
    int total_size = total_bytes + 54;
    copy->header->size = (uint32_t)total_size;

    //? Allocate memory for new image pixels
    copy->data = calloc((size_t)(new_width * new_height), sizeof(struct pixel));

    //? Iterate through new pixels and find corresponding pixel in original image
    for (uint32_t new_row = 0; new_row < new_height; new_row++) {
        for (uint32_t new_col = 0; new_col < new_width; new_col++) {
            //? Calculate corresponding pixel in original image
            uint32_t original_row = (uint32_t)((float)new_row / new_height * (float)image->header->height);
            uint32_t original_col = (uint32_t)((float)new_col / new_width * (float)image->header->width);

            original_row = (uint32_t)floor(original_row);
            original_col = (uint32_t)floor(original_col);

            //? Copy color from original pixel to new pixel
            copy->data[new_row * copy->header->width + new_col] = image->data[original_row * image->header->width + original_col];
        }
    }

    return copy;
}


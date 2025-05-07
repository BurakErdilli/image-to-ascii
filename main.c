#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- Image Loading using stb_image.h ---
// To load actual JPEG, PNG, etc., we will use the stb_image.h library.
// You need to download this single header file and place it in your project directory.
// Download from: https://github.com/nothings/stb/blob/master/stb_image.h

// Define this before including stb_image.h in *one* of your .c files
// to include the implementation.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Function to load image data using stb_image.h
// Loads the image, forces it to 1 channel (grayscale), and returns the pixel data.
// Returns NULL on failure.
unsigned char* load_image_grayscale(const char* filename, int* width, int* height) {
    int channels;
    // Load image, force to 1 channel (grayscale)
    // The last argument '1' requests 1 channel output.
    unsigned char* img_data = stbi_load(filename, width, height, &channels, 1);
    if (!img_data) {
        fprintf(stderr, "Error loading image %s: %s\n", filename, stbi_failure_reason());
        return NULL;
    }
    printf("Image loaded successfully: %dx%d pixels, %d original channels (forced to 1).\n", *width, *height, channels);
    return img_data;
}

// Function to free image data loaded by stb_image.h
void free_image_data(unsigned char* data) {
    stbi_image_free(data);
}


// --- ASCII Art Conversion Logic ---

// ASCII character palette ordered from darkest to lightest
// You can experiment with different character sets
const char* ascii_palette = "@#S%?*+;:,.- "; // Example palette

// Function to map a grayscale value (0-255) to an ASCII character
char map_grayscale_to_char(int grayscale_value) {
    // Ensure value is within range
    if (grayscale_value < 0) grayscale_value = 0;
    if (grayscale_value > 255) grayscale_value = 255;

    // The palette is ordered from darkest (index 0) to lightest (last index).
    // A low grayscale value (dark) should map to an early character.
    // A high grayscale value (light) should map to a later character.
    // We need to scale the 0-255 range to the index range of the palette.
    int palette_size = strlen(ascii_palette);
    // Calculate index: (value / 256.0) * palette_size
    // Use 255.0 for scaling 0-255 range to 0 to palette_size-1 indices
    int index = (int)(((double)grayscale_value / 255.0) * (palette_size - 1));

    // Ensure index is within valid bounds
    if (index < 0) index = 0;
    if (index >= palette_size) index = palette_size - 1;

    return ascii_palette[index];
}

// Function to convert an image (represented by grayscale pixel data) to ASCII art
// pixel_data: array of grayscale pixel values (0-255), type unsigned char* from stb_image
// img_width, img_height: dimensions of the original image
// output_width: desired width of the ASCII art in characters
// output_file: FILE pointer to write the ASCII art (e.g., stdout or a file)
void convert_to_ascii(const unsigned char* pixel_data, int img_width, int img_height, int output_width, FILE* output_file) {
    if (!pixel_data || img_width <= 0 || img_height <= 0 || output_width <= 0 || !output_file) {
        fprintf(stderr, "Error: Invalid input for ASCII conversion.\n");
        return;
    }

    // Calculate block size for sampling
    // Each block of pixels in the original image will map to one ASCII character.
    int block_width = img_width / output_width;
    // ASCII characters are typically taller than wide (aspect ratio ~ 1:2)
    // To maintain the image's aspect ratio visually, we sample more vertically
    // than horizontally.
    double character_aspect_ratio = 2.0; // Assume character is twice as tall as wide
    int block_height = (int)(block_width * character_aspect_ratio);

    // Ensure block sizes are at least 1
    if (block_width == 0) block_width = 1;
    if (block_height == 0) block_height = 1;

    // Calculate the actual output height based on block height
    int output_height = img_height / block_height;

    printf("Converting image to ASCII art...\n");
    printf("Original: %dx%d, Output (chars): %dx%d (approx)\n", img_width, img_height, output_width, output_height);
    printf("Sampling block size: %dx%d pixels\n", block_width, block_height);


    // Iterate through the output grid of characters
    for (int y_char = 0; y_char < output_height; ++y_char) {
        for (int x_char = 0; x_char < output_width; ++x_char) {

            // Calculate the corresponding block of pixels in the original image
            int start_x = x_char * block_width;
            int start_y = y_char * block_height;

            // Ensure block coordinates are within image bounds
            // Adjust block size for the last row/column if dimensions aren't perfectly divisible
            int current_block_width = block_width;
            if (start_x + current_block_width > img_width) {
                 current_block_width = img_width - start_x;
            }
            int current_block_height = block_height;
             if (start_y + current_block_height > img_height) {
                 current_block_height = img_height - start_y;
            }

            // Calculate the average grayscale value for this block
            long long total_brightness = 0;
            int pixel_count = 0;

            for (int y_pixel = start_y; y_pixel < start_y + current_block_height; ++y_pixel) {
                for (int x_pixel = start_x; x_pixel < start_x + current_block_width; ++x_pixel) {
                    // Ensure pixel coordinates are within bounds (should be due to block size adjustments)
                    if (x_pixel >= 0 && x_pixel < img_width && y_pixel >= 0 && y_pixel < img_height) {
                         // Access pixel data - stb_image returns unsigned char*
                         total_brightness += pixel_data[y_pixel * img_width + x_pixel];
                         pixel_count++;
                    }
                }
            }

            int average_grayscale = 0;
            if (pixel_count > 0) {
                average_grayscale = (int)(total_brightness / pixel_count);
            }

            // Map the average grayscale to an ASCII character and print it
            char ascii_char = map_grayscale_to_char(average_grayscale);
            fputc(ascii_char, output_file);
        }
        // Print a newline at the end of each ASCII row
        fputc('\n', output_file);
    }
    printf("ASCII art conversion complete.\n");
}


int main() {
    char input_filename[256];
    char output_filename[256];
    int output_width;

    printf("------ Image to ASCII Art Converter ------\n");

    // Get input filename
    printf("Enter input image filename (e.g., image.jpg, image.png): ");
    if (scanf("%s", input_filename) != 1) {
        fprintf(stderr, "Error reading input filename.\n");
        return 1;
    }

    // Get output filename
    printf("Enter output ASCII file name (e.g., output.txt): ");
     if (scanf("%s", output_filename) != 1) {
        fprintf(stderr, "Error reading output filename.\n");
        return 1;
    }

    // Get desired output width
    printf("Enter desired output width in characters (e.g., 80, 120): ");
     if (scanf("%d", &output_width) != 1 || output_width <= 0) {
        fprintf(stderr, "Error: Invalid output width.\n");
        return 1;
    }

    // --- Load Image using stb_image.h ---
    int img_width, img_height;
    // stb_image returns unsigned char*
    unsigned char* pixel_data = load_image_grayscale(input_filename, &img_width, &img_height);

    if (!pixel_data) {
        fprintf(stderr, "Failed to load image data. Make sure the file exists and is a supported format (JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC).\n");
        return 1;
    }

    // --- Open Output File ---
    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Error opening output file");
        // Free image data before exiting
        free_image_data(pixel_data);
        return 1;
    }

    // --- Convert and Write ASCII Art ---
    // Pass the unsigned char* data to the conversion function
    convert_to_ascii(pixel_data, img_width, img_height, output_width, output_file);

    // --- Cleanup ---
    fclose(output_file);
    free_image_data(pixel_data); // Free data loaded by stb_image

    printf("ASCII art saved to %s\n", output_filename);

    return 0; 
}

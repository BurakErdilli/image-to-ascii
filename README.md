Image to ASCII Art Converter

This C program converts images (like JPG, PNG) into ASCII art using the stb_image.h library.
How it Works

    Loads an image file (various formats supported).
    Converts it to grayscale.
    Maps pixel brightness to ASCII characters.
    Outputs the ASCII art to a text file.
    Sets the output width to match the original image width by default.

Prerequisites

    A C compiler (GCC, Clang, etc.).
    The stb_image.h header file (download separately).

Setup & Run

    Download stb_image.h and place it in your project folder.
    Save the provided C code as main.c in the same folder.
    Compile using your C compiler (e.g., gcc main.c -o ascii_converter).
    Run the executable (e.g., ./ascii_converter).
    Enter the input image filename and the desired output text filename when prompted. 

For viewing on phone screens/chats, you might need to edit main.c to ask for a smaller output width (e.g., 40-80 characters) instead of using the full image width.
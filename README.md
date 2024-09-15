# BMP Steganography Tool

This program allows for hiding and revealing images or text within BMP image files using steganography techniques. It supports several functionalities like hiding another BMP image, hiding text from the command line, hiding text from a .txt file, and decoding hidden data from BMP files.

## Features

- **Hide and reveal images**: Embed a second BMP image inside the least significant bits of the primary BMP image.
- **Hide and reveal text**: Hide text inside a BMP image, either from the command line or from a text file.
- **Supports 24-bit BMP files**: The program only works with BMP files that have a 24-bit color depth.

## Constraints
- The dimensions of the two BMP files have to match if you are hiding another image

## File Descriptions

- `BMPHeader`: Structure representing the BMP file header (e.g., file size, offset to pixel array).
- `DIBHeader`: Structure representing the DIB header (e.g., image dimensions, color depth).
- `printBMPHeader()`: Prints the BMP header information.
- `printDIBHeader()`: Prints the DIB header information.
- `swapBits()`: Swaps the most significant bits (MSB) and least significant bits (LSB) of a byte.
- `revi()`: Reveals a hidden image by swapping the bits of the BMP file pixels.
- `hidef()`: Hides a BMP image inside another BMP file.
- `hidet()`: Hides a string of text inside a BMP file.
- `hidetf()`: Hides the content of a text file inside a BMP file.
- `revt()`: Reveals hidden text inside a BMP file.
- `revtf()`: Reveals text hidden inside a BMP file into an output file (`Decoded_Text.txt`).

## Usage

Compile the program using a C compiler, e.g. gcc. 


- `Compiling:` gcc -o bmp_steg bmp_steg.c
- `Hiding Images:` ./bmp_steg --hidef <source_image.bmp> <image_to_hide.bmp>
- `Reveal a hidden image from a BMP image:` ./bmp_steg --revi <image_with_hidden_image.bmp>
- `Hide a string of text inside a BMP image:` ./bmp_steg --hidet <image.bmp>
  - This will prompt you for the text to hide
- `Hide a text file inside a BMP image:` ./bmp_steg --hidetf <image.bmp> <textfile.txt>
- `Reveal hidden text from a BMP image:` ./bmp_steg --revt <image_with_hidden_text.bmp>
- `Display BMP header information:` ./bmp_steg --info <image.bmp>

## Author
This program was written by Nico Bartello.



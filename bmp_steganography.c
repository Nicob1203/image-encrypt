//Nico Bartello
#include <stdio.h>
#include <string.h>

#pragma pack(1)

typedef struct {
    char format[2];       //Format identifier should be "BM"
    unsigned int fileSize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;      //Offset to the start of the pixel array
} BMPHeader;

typedef struct{
    unsigned int size;  //Size needs to be 40
    unsigned int width;
    unsigned int height;
    unsigned short colorPlanes;
    unsigned short BPP; //Bits per pixel should be 24
    unsigned int compressionScheme;
    unsigned int imgSize;
    unsigned int horizRes;
    unsigned int vertRes;
    unsigned int numColors;
    unsigned int impColors;

}DIBHeader;
#pragma pack(4)

//Both print functions just print out the information gathered by the dib and bmp headers
void printBMPHeader(BMPHeader *header)
{
    printf("=== BMP Header ===\n");
    printf("Type: %c%c\n", header->format[0], header->format[1]);
    printf("Size: %d\n", header->fileSize);
    printf("Reserved 1: %d\n", header->reserved1);
    printf("Reserved 2: %d\n", header->reserved2);
    printf("Image offset: %d\n", header->offset);
}

void printDIBHeader(DIBHeader *header)
{
    printf("\n=== DIB Header ===\n");
    printf("Size: %d\n", header->size);
    printf("Width: %d\n", header->width);
    printf("Height: %d\n", header->height);
    printf("# color planes: %d\n", header->colorPlanes);
    printf("# bits per pixel: %d\n", header->BPP);
    printf("Compression scheme: %d\n", header->compressionScheme);
    printf("Image size: %d\n", header->imgSize);
    printf("Horizontal resolution: %d\n", header->horizRes);
    printf("Vertical resolution: %d\n", header->vertRes);
    printf("# colors in palette: %d\n", header->numColors);
    printf("# important colors: %d\n", header->impColors);

}


/**
 * @brief Swaps the most significant bits and the least significant
 * @param byte t
 * @return The correctly swapped byte as an unsigned char
 */
unsigned char swapBits(unsigned char byte) {
    unsigned char MSB = byte&0xF0; //Masking the MSB
    unsigned char LSB = byte&0x0F; //Masking the LSB

    return ((MSB >> 4) | (LSB << 4)); //Shifting the bits to their correct location and oring them together
}


/**
 * @brief This is the reveal image function which iterates through each pixel and uses the swap bits method to flip them, revealing the hidden image
 * @param bmp_file the bmp image file that you want to flip
 * @param header the dib header from the bmp file
 */
void revi(FILE *bmp_file, DIBHeader *header){
    unsigned char bgr[3];               //Char array to store each colors respective value
    int row_size = (header->width * 3); //This is calculating the row size multiplying the width by 3
    int padding = row_size%4;           //This gets the remainder of the row size divided by 4
    for(int y = 0; y<header->height; y++){
        for(int x = 0; x<header->width; x++){
            //For loops go through each pixel
            fread(bgr, sizeof(unsigned char), 3, bmp_file); //Reads the pixels values into the bgr array
            bgr[0] = swapBits(bgr[0]);
            bgr[1] = swapBits(bgr[1]);            //Swaps the bits of the pixel to reveal the hidden image
            bgr[2] = swapBits(bgr[2]);
            fseek(bmp_file, -3, SEEK_CUR);             //Moves file cursor back 3 to rewrite over them with the new values
            fwrite(bgr, sizeof(unsigned char), 3, bmp_file);    //The rewriting
        }
        fseek(bmp_file, padding, SEEK_CUR); //Makes sure to account for the padding and moves past it
    }
}

/**
 * @brief This is the method that hides the second bmp image within the first images least significant color bits, making it virtually unoticable
 * @param bmp_file1
 * @param header1
 * @param bmp_file2
 */
void hidef(FILE *bmp_file1, DIBHeader *header1, FILE *bmp_file2){
    unsigned char bgr1[3], bgr2[3];     //Creates two bgr pixel arrays for file one and two
    int row_size = (header1->width * 3);
    int padding = row_size%4;
    //if(padding!=0)
        //padding = 4-(row_size%4);        //Calculates the padding same as the reveal function
    for(int y = 0; y<header1->height; y++) {
        for (int x = 0; x<header1->width; x++) {
            //Goes through each pixel and reads the values into the array
            fread(bgr1, sizeof(unsigned char), 3, bmp_file1);
            fread(bgr2, sizeof(unsigned char), 3, bmp_file2);
            bgr1[0] = (bgr1[0] & 0xF0) | (bgr2[0] >> 4);        //Masks the first char value to save the msb and ors it with the second files pixel setting the second files msb to the firsts lsb
            bgr1[1] = (bgr1[1] & 0xF0) | (bgr2[1] >> 4);
            bgr1[2] = (bgr1[2] & 0xF0) | (bgr2[2] >> 4);
            fseek(bmp_file1, -3, SEEK_CUR);                 //Moves cursor to prepare for rewrite
            fwrite(bgr1, sizeof(unsigned char), 3, bmp_file1);  //rewrite
        }
        fseek(bmp_file1, padding, SEEK_CUR);
        fseek(bmp_file2, padding, SEEK_CUR);
    }
}
/**
 * @brief This function hides text that was written through the command line in the bmp file
 * @param bmp_file
 * @param header
 * @param text
 * @return A int to verify that the hiding was successful
 */
int hidet(FILE *bmp_file, DIBHeader *header, char* text)
{
    unsigned char bg[3];
    int row_size = header->width*3;
    int padding = row_size%4;
    for(int y = 0; y<header->height; y++)
    {
        for(int x = 0; x<header->width; x++){
            fread(bg, sizeof(unsigned char), 3, bmp_file);
                if(*text=='\0')
                {
                    bg[0] &= 0xF0;
                    bg[1] &= 0xF0;
                    fseek(bmp_file, -3, SEEK_CUR);
                    fwrite(bg, sizeof(unsigned char), 3, bmp_file);
                    return 1;
                }
                else{
                    bg[0] = bg[0]&0xF0 | (*text>>4);
                    bg[1] = bg[1]&0xF0 | (*text&0x0F);
                }
            fseek(bmp_file, -3, SEEK_CUR);
            fwrite(bg, sizeof(unsigned char), 3, bmp_file);
            text++;
        }
        fseek(bmp_file, padding, SEEK_CUR);
    }
    return 0;
}

/**
 * @brief This function takes a text file and hides it within the bmp file
 * @param bmp_file
 * @param header
 * @param txtFile
 * @return a int to verify whether or not it was successful
 */
int hidetf(FILE *bmp_file, DIBHeader *header, FILE *txtFile)
{
    unsigned char bg[3];
    int row_size = header->width*3;
    int padding = row_size%4;
    if(padding!=0)
        padding = 4-(row_size%4);
    char *line;
    size_t len =0;
    ssize_t read = getline(&line, &len, txtFile);
    for(int y = 0; y<header->height; y++)
    {
        for(int x = 0; x<header->width; x++){
            fread(bg, sizeof(unsigned char), 3, bmp_file);
            if(*line=='\0')
            {
                bg[0] &= 0xF0;
                bg[1] &= 0xF0;
                fseek(bmp_file, -3, SEEK_CUR);
                fwrite(bg, sizeof(unsigned char), 3, bmp_file);
                read = getline(&line, &len, txtFile);
                if(read == EOF){
                    bg[0] = bg[0]&0xF0 | (0x01);
                    bg[1] = bg[1]&0xF0 | (0x0A);
                    fseek(bmp_file, -3, SEEK_CUR);
                    fwrite(bg, sizeof(unsigned char), 3, bmp_file);
                    return 1;}
                bg[0] = bg[0]&0xF0 | (*line>>4);
                bg[1] = bg[1]&0xF0 | (*line&0x0F);
                fseek(bmp_file, -3, SEEK_CUR);
                fwrite(bg, sizeof(unsigned char), 3, bmp_file);
            }
            else{
                bg[0] = bg[0]&0xF0 | (*line>>4);
                bg[1] = bg[1]&0xF0 | (*line&0x0F);
                fseek(bmp_file, -3, SEEK_CUR);
                fwrite(bg, sizeof(unsigned char), 3, bmp_file);
            }
            line++;
        }
        fseek(bmp_file, padding, SEEK_CUR);
    }
    if(read!=EOF)
        return 1;
    return 0;
}

/**
 * @brief This function takes a bmp file and reveals the text thats hidden in it
 * @param bmp_file
 * @param header
 * @return a int to verify whether or not it was successful
 */
int revt(FILE *bmp_file, DIBHeader *header)
{
    unsigned char bg[3];               //Char array to store each colors respective value
    int row_size = (header->width * 3); //This is calculating the row size multiplying the width by 3
    int padding = row_size%4; //This gets the remainder of the row size divided by 4
    char c;
    if(padding!=0)
        padding = 4-(row_size%4);       //Checks if the padding is 0, if not it calculates the required padding
    FILE *textOut = fopen("Decoded_Text.txt", "w");
    for(int y = 0; y<header->height; y++){
        for(int x = 0; x<header->width; x++){
            //For loops go through each pixel
            fread(bg, sizeof(unsigned char), 3, bmp_file); //Reads the pixels values into the bgr array
            c = ((bg[0]&0x0F)<<4) | (bg[1]&0x0F);
            if(c=='\0'){
                fclose(textOut);
                return 1;
            }
            else
                fwrite(&c, sizeof(char), 1, textOut);
        }
        fseek(bmp_file, padding, SEEK_CUR); //Makes sure to account for the padding and moves past it
        return 0;
    }
    fclose(textOut);
    return 0;
}
int revtf(FILE *bmp_file, DIBHeader *header)
{
    unsigned char bg[3];               //Char array to store each colors respective value
    int row_size = (header->width * 3); //This is calculating the row size multiplying the width by 3
    int padding = row_size%4; //This gets the remainder of the row size divided by 4
    char c;
    if(padding!=0)
        padding = 4-(row_size%4);       //Checks if the padding is 0, if not it calculates the required padding
    FILE *textOut = fopen("Decoded_Text.txt", "w");
    for(int y = 0; y<header->height; y++){
        for(int x = 0; x<header->width; x++){
            //For loops go through each pixel
            fread(bg, sizeof(unsigned char), 3, bmp_file); //Reads the pixels values into the bgr array
            c = ((bg[0]&0x0F)<<4) | (bg[1]&0x0F);
            if(c==0x1A){
                fclose(textOut);
                return 1;
            }
            else if(c=='\n')
                fwrite("\n", sizeof(char), 1, textOut);
            else if(c!=0)
                fwrite(&c, sizeof(char), 1, textOut);
        }
        fseek(bmp_file, padding, SEEK_CUR); //Makes sure to account for the padding and moves past it
    }
    fclose(textOut);
    return 0;
}

int main(int argc, char *argv[]) {
    //Makes sure the program was called with something
    if(argc<3 || (!strcmp(argv[1], "--hidef") && argc<4)) {
        printf("ERROR: Missing arguments.\n");
        printf("Arguments: \n--revi <image.jpg> = reveals if there is a hidden image within\n");
        return 1;
    }


    FILE *bmp_file1;
    FILE *bmp_file2;
    FILE *txt_file;
    BMPHeader bmpHeader;
    DIBHeader dibHeader;
    BMPHeader bmpHeader2;
    DIBHeader dibHeader2;

    //Checks to see if the first argument was revi
    if(!strcmp(argv[1], "--revi"))
    {
        //If so it opens the file in rb+, read binary and the plus allows for writing without overwriting the previous data
        bmp_file1 = fopen(argv[2], "rb+");
    }
    else if((!strcmp(argv[1], "--revt")) || (!strcmp(argv[1], "--revtf")))
    {
        bmp_file1 = fopen((argv[2]), "rb");
    }
    else if(!strcmp(argv[1], "--hidet"))
    {
        bmp_file1 = fopen((argv[2]), "rb+");
    }
    //Same as revi but checks for hide and then opens bother files, also checks to make sure the second file and the headers are correct
    else if(!strcmp(argv[1], "--hidef")){
        bmp_file1 = fopen(argv[2], "rb+");
        bmp_file2 = fopen(argv[3], "rb+");
        if(bmp_file2 == NULL)
        {
            printf("ERROR: File not found.\n");
            return 1;
        }
        fread(&bmpHeader2, sizeof(BMPHeader), 1, bmp_file2);
        fread(&dibHeader2, sizeof(DIBHeader), 1, bmp_file2);
        if(bmpHeader2.format[0]!='B' || bmpHeader2.format[1]!='M' || dibHeader2.size!=40 || dibHeader2.BPP!=24)
        {
            printf("ERROR: The format is not supported.\n");
           // return 1;
        }

    }
    else if(!strcmp(argv[1], "--hidetf")){
        bmp_file1 = fopen(argv[2], "rb+");
        txt_file = fopen(argv[3], "r");
        if(txt_file == NULL)
        {
            printf("ERROR: Text file not found.\n");
            fclose(txt_file);
            return 1;
        }

    }
    //Accounts for calling the file and just printing the header information
    else
        bmp_file1 = fopen(argv[2], "rb");
    //Checking if the first file is null and then checks the bmp and dib headers
    if (bmp_file1 == NULL) {
        printf("ERROR: File not found.\n");
        return 1;
    }
    fread(&bmpHeader, sizeof(BMPHeader), 1, bmp_file1);
    fread(&dibHeader, sizeof(DIBHeader), 1, bmp_file1);

    if(bmpHeader.format[0]!='B' || bmpHeader.format[1]!='M' || dibHeader.size!=40 || dibHeader.BPP!=24)
    {
        printf("ERROR: The format is not supported.\n");
       // return 1;
    }

    //This is the if structure to call the required functions
    if(!strcmp(argv[1], "--revi"))
    {
        fseek(bmp_file1, bmpHeader.offset, SEEK_SET); //Moving file cursor to offset provided by the bmp
        revi(bmp_file1, &dibHeader);
    }
    else if(!strcmp(argv[1], "--revt"))
    {
        fseek(bmp_file1, bmpHeader.offset, SEEK_SET); //Moving file cursor to offset provided by the bmp
        if(revt(bmp_file1, &dibHeader))
            printf("Success!\n");
    }
    else if(!strcmp(argv[1], "--revtf"))
    {
        fseek(bmp_file1, bmpHeader.offset, SEEK_SET); //Moving file cursor to offset provided by the bmp
        if(revtf(bmp_file1, &dibHeader))
            printf("Success!\n");
    }
    else if(!strcmp(argv[1], "--hidet"))
    {
        fseek(bmp_file1, bmpHeader.offset, SEEK_SET); //Moving file cursor to offset provided by the bmp
        char text[1000];
        printf("Enter Text to be Encoded: ");
        scanf("%999[^\n]", text);
        printf("H Value: %x\n", text[0]);
        printf("H MSB: %x\n", (unsigned char)((text[0]&0xF0)>>4));
        printf("H LSB: %x\n", (text[0]&0x0F));
        if(hidet(bmp_file1, &dibHeader, text))
            printf("Success!\n");
    }
    else if(!strcmp(argv[1], "--hidetf"))
    {
        fseek(bmp_file1, bmpHeader.offset, SEEK_SET); //Moving file cursor to offset provided by the bmp
        if(hidetf(bmp_file1, &dibHeader, txt_file))
            printf("Success!\n");
        else
            printf("Error: Text File to Long for BMP\n");
    }
    else if(!strcmp(argv[1], "--hidef"))
    {
        if((dibHeader2.width!=dibHeader.width) || (dibHeader2.height!=dibHeader.height)) //Checks to make sure the two files have matching height and width otherwise throws error and ends
        {
            printf("ERROR: Mismatching Width/Height\n");
            return 1;
        }
        fseek(bmp_file1, bmpHeader.offset, SEEK_SET);
        fseek(bmp_file2, bmpHeader2.offset, SEEK_SET);
        hidef(bmp_file1, &dibHeader, bmp_file2);
        fclose(bmp_file2);
    }
    else if(!strcmp(argv[1], "--info")){
        printBMPHeader(&bmpHeader);
        printDIBHeader(&dibHeader);
    }
    fclose(bmp_file1);
    return 0;
}

//

#ifndef bmproutine_hpp
#define bmproutine_hpp

#include <array>
#include <cstdint>
#include <ostream>
#include <string>

//======================================================================
/* BMP file
  A decent reference is at : https://en.wikipedia.org/wiki/BMP_file_format#File_structure
 Essentially the format is:
  BMP FILE
      File Header
      DIB Header
      Pallette (if any)
      Pixel Data



 File header
   The file header consists of:

 Offset      Size        Purpose
 0          2 bytes     The header field used to identify the BMP and DIB file is 0x42 0x4D in hexadecimal,
                        same as BM in ASCII. The following entries are possible:
                                BM - Windows 3.1x, 95, NT, ... etc.
                                BA - OS/2 struct bitmap array
                                CI - OS/2 struct color icon
                                CP - OS/2 const color pointer
                                IC - OS/2 struct icon
                                PT - OS/2 pointer
2           4 bytes     The size of the BMP file in bytes
6           2 bytes     Reserved; actual value depends on the application that creates the image, if created manually can be 0
8           2 bytes     Reserved; actual value depends on the application that creates the image, if created manually can be 0
10          4 bytes     The offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found.

DIB Header
There are several, but we are only going to worry about one type
Offset (dec)    Size (bytes)        Windows BITMAPINFOHEADER
14                  4               the size of this header, in bytes (40)
18                  4               the bitmap width in pixels (signed integer)
22                  4               the bitmap height in pixels (signed integer)
26                  2               the number of color planes (must be 1)
28                  2               the number of bits per pixel, which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.
30                  4               the compression method being used. See the next table for a list of possible values
34                  4               the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.
38                  4               the horizontal resolution of the image. (pixel per metre, signed integer)
42                  4               the vertical resolution of the image. (pixel per metre, signed integer)
46                  4               the number of colors in the color palette, or 0 to default to 2n
50                  4               the number of important colors used, or 0 when every color is important; generally ignored
 
Color Table
 We are not using one, we dont need to include one. See reference above for more information
 
Pixel Data
 The data is sequentially, with one row of data after the other. Each row is padded to be a multiple of 32 bit values (so one may have to add pad bytes)
 In addition, the order of the rows, is  descending scan lines
*/
//======================================================================
//==============================================================================================================================
// BmpHeader
//==============================================================================================================================
// Given this definition, lets define a few structures
struct BmpHeader {
    std::uint16_t identifer ;
    std::uint32_t bmpFileSize ;
    std::array<std::uint16_t,2> applicationSpecific ;
    std::uint32_t dataOffset ;
    auto save(std::ostream &output) const -> void ;
    BmpHeader() ;
};
//==============================================================================================================================
// DibHeader
//==============================================================================================================================

struct DibHeader {
    std::uint32_t dibSize ;
    std::int32_t pixelWidth ;
    std::int32_t pixelHeight ;
    std::uint16_t colorPlanes ;
    std::uint16_t colorDepth ;
    std::uint32_t compression ;
    std::uint32_t imageSize ;
    std::uint32_t horizontalResolution ;
    std::uint32_t verticalResolution ;
    std::uint32_t colorsInPallette ;
    std::uint32_t importantColors ;
    auto save(std::ostream &output) const -> void ;
    DibHeader() ;
    DibHeader(std::int32_t width, std::int32_t height, std::uint16_t colorDepth = 16);
    auto padBytes() const -> int ;
};

#endif /* bmproutine_hpp */

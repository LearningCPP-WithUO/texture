
// A great reference for what is contained in these is at: https://en.cppreference.com/w/cpp
#include <algorithm>  // Useful functions on collections
#include <array> // A non expandable vector
#include <cstdio> // The C std io library, we need for std::snprintf
#include <cstdlib> // We include this to get the standard return codes defined
#include <filesystem> // This is c++17 and above.  This provides the ability to manipulate the filesystem in a standard way
#include <fstream>  // This allows us to read/write files via c++ stream ability
#include <iostream> // We include this to get access to the input/output streams (std::cin, std::cerr, std::cout, and std::clog)
#include <memory> // This is if we want to use "smart" pointers for memory allocation/manipulation (we dont, but included as an excersize to modify code from a vector for data to smart pointer
#include <stdexcept> // This is included to get the standard exception types
#include <string> // We use std::string
#include <vector>  // This includes the "vector" container.  We will store our data that we read from the uo file in a vector

// We include our bmp structures we will use
#include "bmproutine.hpp"  // Note the use "" versuse <>.  This indicates a "user" supplied header versuse system, and the search order is different.

//=======================================================================================================
using namespace std::string_literals ;  // We want to be able to use "strings" (not C strings).  This allows us to append a "s" to the string and indicate it is a std::string


//=================================================================================================
// Define some global variables
//=================================================================================================
// Define where to look for the uodata files
#if defined(_WIN32)  // This is a preprocessor definition. We only want the following code if windows (note _WIN32 is defined for both 32 and 64 bit windows)
// Note the "\\" being used.  Since we want a backward slash, in a text string that is a special character. So we prefix it with a backward slash as well.
auto UOPATH = std::filesystem::path("C:\\Program Files (x86)\\Electronic Arts\\Ultima Online Classic\\");

#else  // For non window platforms
auto UOPATH = std::filesystem::path("uodata");  // This will look in "uodata" that is in the current directory.
#endif



// =======================================================================================================================
// Some information on the calling of the "main" routine, what are argc,argv?
// =======================================================================================================================
/*
 Main is the routine that is passed control by the program loader.  It is passed two arguments.
 The first argument, argc, is the number of parameters that were on the command line following the program.
 The second parameter, is an array of character strings.  There is always one parameter passed,
 and that is the program name itself (so you can see how the program was initialed).
 So the command:
    myprogram this is a test
 would have argc set to 5, and argv would have five entries;
 argv[0] = myprogram
 argv[1] = this
 argv[2] = is
 argv[3] = a
 argv[4] = test
 
 Main can return a single parameter (integer value), that is the return code that the command inteerpretor uses. THis is helpful if you are writing scripts that have your program in it and you want to check if it succedded (it can test this value).
 Traditionally, a value of 0 is success, and a value of 1 is failed.  There are symbols defined in the cstdlib that sets EXIT_FAILURE to one and EXIT_SUCCESS to 0 to make the code more readiable.
 */
int main(int argc, const char * argv[]) {
    // auto is a modern c++ construct that will infer the variable type.  We set our return value to initially "good"
    auto return_value = EXIT_SUCCESS ;
    /*
     The "try/catch" approach is used to when your code may "throw" an exception.
     Exceptions are a way to flag error conditions
     and return control.  If one inherits from std::exception (found in the header <stdexcept>
     for their exceptions (or off the subclasses are are defined from std::exception
     such as std::runtime_error, std::out_of_range, etc) then one is guaranteed
     the method "what()" will be present. And one can get a descriptive text.
     */
    // We will put all of our code in a "try" block, incase anything throws an exception that is "bubbled up".
    try {
        // This is where we will put our code
        
        // ************************************************************************************************
        // UO texture data files definitions
        // ************************************************************************************************
        // The "texidx.mul" file is an index into texmaps.mul file.  it consists of groups three 32 bit integers
        // The first integer is an offset in bytes into the texmaps.mul file where the data for this entry resides
        // The second integer is the length in bytes of the data
        // The third integer is an indicator if it is a 64x64 (value 0) or a 128x128 (value 1) texture
        //
        // The texmaps.mul is a file that consists of "pixel data" for the textures. It is streams of 16 bit color values
        // With the size, and coniguration of the entry provided in the texidx.mul file, one can interpret the data.
        //
        // UO 16 bit color value.  UO uses a non alpha 0555 16 bit color scheme. That means the msb (Most signficant bit) is not used.
        // The remaining groups of bits (grouped in 5 bits) rpresent the color value for Red, Green, Blue. It is worth noting
        // that UO did not use "true black" (R:0,G:0,B:0). That was for "no color" or transparent (they did this with stencils back in the day).
        // So the color word looks like this:
        // 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00   Bits
        //  X  R  R  R  R  R  G  G  G  G  G  B  B  B  B  B
        
        // Lets create the full file path for the two files we are interested in:
        auto idxpath = UOPATH / std::filesystem::path("texidx.mul") ;  // the / operator concatenates file path types, and inserts a directory separator between them
        auto mulpath = UOPATH / std::filesystem::path("texmaps.mul") ;
        
        // Try to open the files
        auto idx = std::ifstream(idxpath.string(),std::ios::binary) ; // we use the binary attribute as the data is binary
        auto mul = std::ifstream(mulpath.string(),std::ios::binary) ;
        if (!idx.is_open()){
            // For some reason we could not open the file!
            throw std::runtime_error("Unable to open the index file: "s + idxpath.string()); // Note the use of the "s" at the end of the first string. We want to do concantation with +, and std::strings can do that.
        }
        if (!mul.is_open()){
            // For some reason we could not open the file!
            throw std::runtime_error("Unable to open the data file: "s + mulpath.string()); // Note the use of the "s" at the end of the first string. We want to do concantation with +, and std::strings can do that.
        }

        //
        auto entrynum = 0 ; // This will be our counter so we know what entry number we are on.
        auto offset = std::uint32_t(0) ; // This is what we will read into for our offset
        auto length = std::uint32_t(0) ; // This is what we will read into for our length ;
        auto flag = std::uint32_t(0) ; // This is what we will read into for our flag (indicator)
        
        while (idx.good() && !idx.eof()){  //Loop while the stream is good and not end of file
            idx.read(reinterpret_cast<char*>(&offset), 4) ; // fstreams need a char* for the buffer that data will be read into. So we take the address of our variable (&)
                                                            // That would make a std::uint32_t*.  We use reinterpret_cast to make it a char*
                                                            // The 4 is the number of bytes to read
            idx.read(reinterpret_cast<char*>(&length),4) ;
            idx.read(reinterpret_cast<char*>(&flag),4) ;
            if (idx.gcount()==4) {  // we want to ensure we read 4 bytes with the last read
                
                // We did!, not see if the offset and length are valid
                if (offset != 0xFFFFFFFF && length != 0 && length != 0xFFFFFFFF) { // We check to ensure offset is not "-1", length not 0 (no bytes), nor -1
                    // Ok, this entry is pointing to valid data!
                    // we want to read the data from the file.
                    // We will do this with a vector, but a unique_ptr would be a valid alternative
                    auto data = std::vector<std::uint16_t>(length/2,0) ;  // Make a vector of unsigned 16 bit words, the size of "length" in words (why /2), and initialize them to 0
                    // we will use this data buffer to read into
                    mul.seekg(offset,std::ios::beg); // Seek to where the data is
                    mul.read(reinterpret_cast<char*>(data.data()),length);
                    // Data now has our texture data
                    auto width = (flag == 1?128:64) ; // If the flag is 1, it is 128x128, otherwise 64x64
                    // Create our two BMP structures we will use
                    auto dib = DibHeader(width, width) ; // Since the textures we are dealing with are a square, we can use width for both height and width
                    auto bmp = BmpHeader() ; // our bmp file header
                    auto pad = dib.padBytes() ; // Doe we hav any pad bytes we need to add for each line (we dont, but we have this here to show what one should be worried about).
                    // Now we can calculate our total file size
                    bmp.bmpFileSize = 14 + 40 + (((width*2)+ pad) * width); // size of the file header + size of dib header + size of pixeldata (including pad if any)
                    bmp.dataOffset = 54 ; // Since we are not using pallette, we know the pixel data will be the size of the bmp file header, plus the size of the DIB headers
                    // Now lets create our file
                    char filestring[11] ; // We will make a character string space to hold the file name
                    filestring[10] = 0 ; // We will "null" terminate the string (we should set the entire array to 0, but since we "KNOW" we will fill the rest with characters from our sprintnf, we can get aways with this.
                    std::snprintf(filestring, 11, "0x%04X.bmp",entrynum); // make the file name just be the entry number in hex. Make all entry names be 4 characters, and pad with 0 (what the 04 does). Upper case the hex (why capital X)
                    auto output = std::ofstream(filestring,std::ios::binary) ;  // Create the bmp file
                    bmp.save(output); // Write the bmp header to the file
                    dib.save(output) ; // Write the dib header to the file
                    // Create a "zero" piece of data to write for any pad values
                    auto zero = std::uint8_t(0) ;
                    // Remember the BMP starts with the scan lines inverted, so we have to invert it here
                    // This will write data line by line, so we can add pad values if we had any
                    for (auto i = 0 ; i < width;i++){
                        auto offset = ((width -i)-1) * ((width*2)+pad); // THis is calculating the offset to put the bottom row first in the bmp file (scan lines are reversed)
                        output.write(reinterpret_cast<const char*>(data.data())+ offset, width*2) ;
                        // Now write any pad bytes we ned
                        for (auto z=0 ; z < pad;z++) {
                            output.write(reinterpret_cast<char*>(&zero),1) ;
                        }
                    }
                }
                entrynum++ ; // Increment the entry counter to the next one
            }
        }
        std::cout <<"Processed " << entrynum << " entries" << std::endl;
    }
    // If any of our routines throw an exception that is based of std::exception
    catch(const std::exception &e){
        std::cerr << e.what()<< std::endl;
        return_value = EXIT_FAILURE;
    }
    // If it is an excpetion that is not based off std::exception
    catch(...) {
        std::cerr << "Unknown execption" << std::endl;
        return_value = EXIT_FAILURE;

    }
    // return our return value
    return return_value;
}
//=====================================================================================================================================


//

#include "bmproutine.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================

//==============================================================================================================================
// BmpHeader
//==============================================================================================================================
//==============================================================================================================================
BmpHeader::BmpHeader():bmpFileSize(0),dataOffset(0),identifer(0) {
    std::fill(applicationSpecific.begin(), applicationSpecific.end(), 0);
    identifer = 0x4D42  ; // 'B''M"
}
//==============================================================================================================================
auto BmpHeader::save(std::ostream &output) const -> void {
    output.write(reinterpret_cast<const char*>(&identifer), 2);
    output.write(reinterpret_cast<const char*>(&bmpFileSize),4);
    output.write(reinterpret_cast<const char*>(applicationSpecific.data()),4);
    output.write(reinterpret_cast<const char*>(&dataOffset),4);
}

//==============================================================================================================================
// DibHeader
//==============================================================================================================================
//==============================================================================================================================
// We are going to make the verticalRelation be 72DPI, which
DibHeader::DibHeader(): dibSize(40),pixelWidth(0),pixelHeight(0),colorPlanes(1),colorDepth(0),compression(0),imageSize(0),horizontalResolution(72*39.37),verticalResolution(72*39.37),colorsInPallette(0),importantColors(0){
    
}
//==============================================================================================================================
DibHeader::DibHeader(std::int32_t width, std::int32_t height, std::uint16_t colorDepth):DibHeader() {
    this->colorDepth = colorDepth ;
    this->pixelWidth = width ;
    this->pixelHeight = height ;
}
//==============================================================================================================================
auto DibHeader::padBytes() const -> int {
    return static_cast<int>((this->pixelWidth * (colorDepth>=8?colorDepth/8:1) ) % 4);
}
//==============================================================================================================================
auto DibHeader::save(std::ostream &output) const -> void {
    output.write(reinterpret_cast<const char*>(&dibSize), 4);
    output.write(reinterpret_cast<const char*>(&pixelWidth),4);
    output.write(reinterpret_cast<const char*>(&pixelHeight),4);
    output.write(reinterpret_cast<const char*>(&colorPlanes),2);
    output.write(reinterpret_cast<const char*>(&colorDepth),2);
    output.write(reinterpret_cast<const char*>(&compression),4);
    output.write(reinterpret_cast<const char*>(&imageSize),4);
    output.write(reinterpret_cast<const char*>(&horizontalResolution),4);
    output.write(reinterpret_cast<const char*>(&verticalResolution),4);
    output.write(reinterpret_cast<const char*>(&colorsInPallette),4);
    output.write(reinterpret_cast<const char*>(&importantColors),4);
}

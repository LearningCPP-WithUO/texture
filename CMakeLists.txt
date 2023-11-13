cmake_minimum_required(VERSION 3.24)
project(texture VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
# *************************************************************************
# The basic executable we are making
# *************************************************************************
# We define the groupings for the files
#
add_executable(texture
    texture/main.cpp
    texture/bmproutine.cpp
    texture/bmproutine.hpp
)
# *************************************************************************
# Platform specific settings
# *************************************************************************

if (WIN32)
target_compile_definitions(texture PRIVATE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CONSOLE
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:Debug>:_DEBUG>
)
    
target_compile_options( texture PRIVATE
        $<$<CONFIG:Release>:/O2>
)
else()

# If we had other settings we wanted to do
target_compile_options(texture PRIVATE
    $<$<CONFIG:Release>:-O2>
)

endif(WIN32)
# *************************************************************************
# the directories we need on the include path
# *************************************************************************
target_include_directories(texture
    PUBLIC
        ${PROJECT_SOURCE_DIR}/texture
)

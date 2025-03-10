cmake_minimum_required(VERSION 3.30)
project(simba_spectra)

set(CMAKE_CXX_STANDARD 23)

if(NOT CMAKE_BUILD_TYPE)
        message( "Build Type is not set, defaulting to Release..." )
        set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(simba_spectra main.cpp
        include/simba/pcap.hpp
        include/simba/decoder.hpp
        include/simba/types.hpp
        include/simba/json.hpp
)


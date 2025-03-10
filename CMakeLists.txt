cmake_minimum_required(VERSION 3.30)
project(simba_spectra)

set(CMAKE_CXX_STANDARD 23)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(simba_spectra main.cpp
        include/simba/pcap.hpp
        include/simba/decoder.hpp
        include/simba/types.hpp
        include/simba/json.hpp
)



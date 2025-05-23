cmake_minimum_required(VERSION 3.31)
project(CPPTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)

add_executable(CPPTest main.cpp
        mca_file.cpp
        mca_file.hpp
        zlibutil.cpp
        zlibutil.hpp
        nbt/nbtparser.cpp
        nbt/nbtparser.hpp
        world/chunk.cpp
        world/chunk.hpp
        world/chunk_section.cpp
        world/chunk_section.hpp
        world/block_states.cpp
        world/block_states.hpp
        world/biomes.cpp
        world/biomes.hpp
        world/block_entities.cpp
        world/block_entities.hpp
        world/heightmaps.cpp
        world/heightmaps.hpp
        world/blending_data.cpp
        world/blending_data.hpp
        world/structures.cpp
        world/structures.hpp
        world/protochunk.cpp
        world/protochunk.hpp
        nbt/nbtcompound.cpp
        nbt/nbtcompound.hpp
)

target_link_libraries(CPPTest PRIVATE ZLIB::ZLIB)

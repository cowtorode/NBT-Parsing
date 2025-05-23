//
// Created by cory on 5/11/25.
//

#include <iostream>
#include "chunk.hpp"

void Chunk::set_version(int32_t i)
{
    version = i;
}

void Chunk::set_status(const std::string& s)
{
    status = s;
}

void Chunk::set_cx(int32_t chunk_x)
{
    cx = chunk_x;
}

void Chunk::set_cy(int32_t chunk_y)
{
    cy = chunk_y;
}

void Chunk::set_cz(int32_t chunk_z)
{
    cz = chunk_z;
}

void Chunk::set_last_update(int64_t i)
{
    last_update = i;
}

void Chunk::set_inhabited_time(int64_t i)
{
    inhabited_time = i;
}

ChunkSection* Chunk::get_sections() const
{
    return sections;
}

BlendingData* Chunk::get_blending_data()
{
    return &blending_data;
}

void Chunk::print()
{
    std::cout << "status: " << status << std::endl
              << "version: " << version << std::endl
              << "cx: " << cx << std::endl
              << "cy: " << cy << std::endl
              << "cz: " << cz << std::endl
              << "inhabited_time: " << inhabited_time << std::endl
              << "last_update: " << last_update << std::endl
              << "blending_max: " << blending_data.max_section << std::endl
              << "blending_min: " << blending_data.min_section << std::endl;
}
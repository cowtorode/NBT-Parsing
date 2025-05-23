//
// Created by cory on 5/11/25.
//

#ifndef CPPTEST_CHUNK_HPP
#define CPPTEST_CHUNK_HPP


#include <string>
#include "chunk_section.hpp"
#include "block_entities.hpp"
#include "heightmaps.hpp"
#include "blending_data.hpp"
#include "structures.hpp"

class Chunk
{
public:
    void set_version(int32_t i);

    void set_status(const std::string& s);

    void set_last_update(int64_t last_update);

    void set_inhabited_time(int64_t inhabited_time);

    // debug
    void print();
private:
    friend void route_xpos(Chunk*, int32_t);

    friend void route_ypos(Chunk*, int32_t);

    friend void route_zpos(Chunk*, int32_t);

    friend ChunkSection* push_chunk(Chunk*);

    friend BlendingData* push_chunk_blending_data(Chunk*);

    void set_cx(int32_t cx);

    void set_cy(int32_t cy);

    void set_cz(int32_t cz);

    ChunkSection* get_sections() const;

    BlendingData* get_blending_data();

    std::string status;
    int32_t version;
    int32_t cx;
    int32_t cy;
    int32_t cz;
    int64_t last_update;
    int64_t inhabited_time;

    ChunkSection* sections;
    BlockEntities tile_entities;
    Heightmaps heightmaps;
    BlendingData blending_data;
    Structures structures;
};


#endif //CPPTEST_CHUNK_HPP

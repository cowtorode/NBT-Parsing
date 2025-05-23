//
// Created by cory on 5/11/25.
//

#ifndef CPPTEST_CHUNK_SECTION_HPP
#define CPPTEST_CHUNK_SECTION_HPP


#include "block_states.hpp"
#include "biomes.hpp"

class ChunkSection
{
public:
private:
    int8_t y;
    BlockStates states;
    Biomes biomes;
    int8_t* block_light;
    int8_t* sky_light;
};


#endif //CPPTEST_CHUNK_SECTION_HPP

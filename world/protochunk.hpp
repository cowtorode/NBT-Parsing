//
// Created by cory on 5/14/25.
//

#ifndef CPPTEST_PROTOCHUNK_HPP
#define CPPTEST_PROTOCHUNK_HPP


#include "chunk.hpp"

class CarvingMasks
{
    char* air;
    char* liquid;
};

class ProtoChunk : public Chunk
{
public:
private:
    CarvingMasks carving_masks;
};


#endif //CPPTEST_PROTOCHUNK_HPP

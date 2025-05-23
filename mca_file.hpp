//
// Created by cory on 5/10/25.
//

#ifndef CPPTEST_MCA_FILE_HPP
#define CPPTEST_MCA_FILE_HPP


#include <cstdint>
#include "nbt/nbtparser.hpp"
#include "world/chunk.hpp"

struct MCAHeader
{
    uint32_t offsets[1024];
    uint32_t timestamps[1024];
};

struct MCAOffset
{
    uint32_t offset;
    uint32_t sectors;
};

/**
 * Header (8-KiB)
 *   Offset Table (4-KiB)
 *     1024 4-byte entries
 *       Each entry contains chunk offset (measured in sectors) as well
 *       as the size of the chunk in sectors, with a sector being 4 KiB:
 *
 *       Entry (4-byte)
 *       0 1 2                3
 *       offset (big endian)  sector count
 *   Timestamp Table (4-KiB)
 *     1024 4-byte entries
 *       Each entry represents the "last modification time of [the] chunk
 *       in epoch seconds."
 * (https://minecraft.fandom.com/wiki/Region_file_format#Header)
 *
 * Payload (N * 4-KiB)
 *   Contains N sectors, each sector being 4-KiB in size. Some chunks may
 *   take up multiple sectors.
 *     Each sector starts with a 4-byte length prefix (big endian), which
 *     corresponds to the length in bytes of the decompressed chunk.
 *     The next byte in the sector refers to the type of compression that
 *     the chunk was compressed under, 1 for GZip, 2 for Zlib, and 3 for
 *     it being uncompressed, although 2 is the only one used in practice.
 *     All additional bytes are the compressed chunk.
 * (https://minecraft.fandom.com/wiki/Region_file_format#Payload)
 * (https://minecraft.fandom.com/wiki/NBT_format)
 * (https://minecraft.wiki/w/Chunk_format)
 */
class MCAFile
{
public:
    explicit MCAFile(const char* dir);

    ~MCAFile();

    void open_mca();

    void read_header();

    [[nodiscard]] Chunk* get_chunk(int32_t cx, int32_t cz);

    void close_mca();
private:
    [[nodiscard]] MCAOffset get_offset(int32_t cx, int32_t cz) const;

    [[nodiscard]] uint32_t get_timestamp(int32_t cx, int32_t cz) const;

    const char* dir;
    int fd;
    MCAHeader header;
    NBTParser chunkbuf;
};


#endif //CPPTEST_MCA_FILE_HPP

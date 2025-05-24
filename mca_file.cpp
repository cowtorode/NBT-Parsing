//
// Created by cory on 5/10/25.
//

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include "zlibutil.hpp"
#include "mca_file.hpp"
#include "world/chunk.hpp"

static void route_version(Chunk* chunk, int32_t version)
{
    chunk->set_version(version);
}

void route_xpos(Chunk* chunk, int32_t cx)
{
    chunk->set_cx(cx);
}

void route_ypos(Chunk* chunk, int32_t cy)
{
    chunk->set_cy(cy);
}

void route_zpos(Chunk* chunk, int32_t cz)
{
    chunk->set_cz(cz);
}

static void route_status(Chunk* chunk, const std::string& status)
{
    chunk->set_status(status);
}

static void route_last_update(Chunk* chunk, int64_t last_update)
{
    chunk->set_last_update(last_update);
}

static void route_inhabited_time(Chunk* chunk, int64_t inhabited_time)
{
    chunk->set_inhabited_time(inhabited_time);
}

static void route_min_section(BlendingData* blending_data, int32_t min_section)
{
    blending_data->min_section = min_section;
}

static void route_max_section(BlendingData* blending_data, int32_t max_section)
{
    blending_data->max_section = max_section;
}

static void route_block_state_properties_name()
{

}

static void route_block_state_palette_name()
{

}

static void route_block_state_data()
{

}

static void route_biome_palette(Biomes* biomes, int32_t len, const std::string palette[])
{

}

static void route_biome_data(Biomes* biomes, int32_t len, int64_t data[])
{

}

static void route_sections_y(ChunkSection* section, int8_t y)
{
}

static void route_sections_block_light(ChunkSection* section, int32_t len, char data[])
{

}

static void route_sections_sky_light(ChunkSection* section, int32_t len, char data[])
{

}

static NBTRouter make_properties()
{
    NBTRouter properties;

    properties.set_string_router("Name", (string_router) route_block_state_properties_name);

    return properties;
}

static NBTRouter make_block_state_palette(const NBTRouter& properties)
{
    NBTRouter block_state_palette;

    block_state_palette.set_string_router("Name", (string_router) route_block_state_palette_name);
    block_state_palette.set_compound_router("Properties", properties);

    return block_state_palette;
}

static NBTRouter make_block_states(const NBTRouter& block_state_palette)
{
    NBTRouter block_states;

    block_states.set_long_array_router("data", (long_array_router) route_block_state_data);
    block_states.set_compound_list_router("palette", block_state_palette);

    return block_states;
}

static NBTRouter make_biomes()
{
    NBTRouter biomes;

    biomes.set_string_list_router("palette", (string_list_router) route_biome_palette);
    biomes.set_long_array_router("data", (long_array_router) route_biome_data);

    return biomes;
}

ChunkSection* push_chunk(Chunk* chunk)
{
    return chunk->get_sections();
}

ChunkSection* inc_section_ptr(ChunkSection* section)
{
    return section + 1;
}

ChunkSection* prepare_chunk_for_sections(Chunk* chunk, int32_t len)
{
    chunk->new_sections(len);
    return chunk->get_sections();
}

static NBTRouter make_sections(const NBTRouter& block_states, const NBTRouter& biomes)
{
    NBTRouter sections;

    sections.prepare_for_compound = (handle_mutator) push_chunk;
    sections.next_handle_l = (handle_mutator) inc_section_ptr;
    sections.prepare_for_compound_list = (compound_list_preparer) prepare_chunk_for_sections;

    sections.set_byte_router("Y", (byte_router) route_sections_y);
    sections.set_compound_router("block_states", block_states);
    sections.set_compound_router("biomes", biomes);
    sections.set_byte_array_router("BlockLight", (byte_array_router) route_sections_block_light);
    sections.set_byte_array_router("SkyLight", (byte_array_router) route_sections_sky_light);

    return sections;
}

BlendingData* push_chunk_blending_data(Chunk* chunk)
{
    return chunk->get_blending_data();
}

static NBTRouter make_blending_data()
{
    NBTRouter blending_data;

    blending_data.prepare_for_compound = (handle_mutator) push_chunk_blending_data;

    blending_data.set_int_router("min_section", (int_router) route_min_section);
    blending_data.set_int_router("max_section", (int_router) route_max_section);

    return blending_data;
}

static NBTRouter make_block_entities()
{
    NBTRouter block_entities;

    return block_entities;
}

static NBTRouter make_heightmaps()
{
    NBTRouter heightmaps;

    return heightmaps;
}

static NBTRouter make_blank(const NBTRouter& sections, const NBTRouter& blending_data, const NBTRouter& block_entities, const NBTRouter& heightmaps)
{
    NBTRouter root;

    root.set_string_router("Status", (string_router) route_status);
    root.set_int_router("DataVersion", (int_router) route_version);
    root.set_int_router("xPos",  (int_router) route_xpos);
    root.set_int_router("yPos", (int_router) route_ypos);
    root.set_int_router("zPos", (int_router) route_zpos);
    root.set_long_router("LastUpdate", (long_router) route_last_update);
    root.set_long_router("InhabitedTime", (long_router) route_inhabited_time);

    root.set_compound_list_router("sections", sections);
    root.set_compound_router("blending_data", blending_data);
    root.set_compound_router("block_entities", block_entities);
    root.set_compound_router("Heightmaps", heightmaps);

    return root;
}

static NBTRouter make_root(const NBTRouter& blank)
{
    NBTRouter root;

    root.set_compound_router("", blank);

    return root;
}

static const NBTRouter properties = make_properties();
static const NBTRouter block_state_palette = make_block_state_palette(properties);
static const NBTRouter block_states = make_block_states(block_state_palette);
static const NBTRouter biomes = make_biomes();
/*
 "sections": [{
   "Y": TAG_Byte,
   "block_states":
   {
     "block_state_palette": [{
        Name: TAG_String
        Properties: {
          Name: TAG_String
        }
     }, {...}],
     "data": TAG_Long_Array
   },
   "biomes":
   {
     "block_state_palette": TAG_List TAG_String,
     "data": TAG_Long_Array
   }
   "BlockLight": TAG_Byte_Array,
   "SkyLight": TAG_Byte_Array
 }, {...}]
 */
static const NBTRouter sections = make_sections(block_states, biomes);
static const NBTRouter blending_data = make_blending_data();
static const NBTRouter block_entities = make_block_entities();
static const NBTRouter heightmaps = make_heightmaps();
static const NBTRouter blank = make_blank(sections, blending_data, block_entities, heightmaps);
static const NBTRouter root = make_root(blank);

MCAFile::MCAFile(const char* dir) : dir(dir), fd(-1)
{
    chunkbuf.set_router(root);
}

void MCAFile::open_mca()
{
    fd = open(dir, 0, O_RDONLY);

    if (fd == -1)
    {
        perror("MCAFile::open_mca(): open()");
    }
}

void MCAFile::read_header()
{
    ssize_t res = read(fd, &header, sizeof(MCAHeader));

    if (res == -1)
    {
        perror("MCAFile::read_header(): read()");
    } else if (res < sizeof(MCAHeader))
    {
        std::cerr << "Unable to route entire mca header." << std::endl;
    }
}

static inline uint32_t relative_chunk_coords_to_index(uint32_t cx, uint32_t cz)
{
    return cx + cz * 32;
}

MCAOffset MCAFile::get_offset(int32_t cx, int32_t cz) const
{
    uint32_t raw = header.offsets[relative_chunk_coords_to_index(cx, cz)];

    // 02 37 00 00

    MCAOffset rax{};
    rax.offset = __builtin_bswap32((raw & 0xffffff) << 8);
    rax.sectors = (raw & 0xff000000) >> 24;

    return rax;
}

uint32_t MCAFile::get_timestamp(int32_t cx, int32_t cz) const
{
    return header.timestamps[4096 + relative_chunk_coords_to_index(cx, cz)];
}

void MCAFile::close_mca()
{
    if (fd != -1)
    {
        int res = close(fd);

        if (res == -1)
        {
            perror("MCAFile::close_mca(): close()");
        }

        fd = -1;
    }
}

MCAFile::~MCAFile()
{
    close_mca();
}

Chunk* MCAFile::get_chunk(int32_t cx, int32_t cz)
{
    MCAOffset offset = get_offset(cx, cz);

    if (offset.sectors)
    {
        // go to where the Chunk is at
        __off_t seek = lseek(fd, offset.offset * 4096, SEEK_SET);

        if (seek == -1)
        {
            perror("MCAFile::get_chunk(MCAOffset): lseek()");
            return nullptr;
        }

        // Read the compressed chunk
        char buf[offset.sectors * 4096];
        ssize_t res = read(fd, buf, sizeof(buf));

        if (res == -1)
        {
            perror("MCAFile::get_chunk(int32_t, int32_t): read()");
            return nullptr;
        }

        // Length of compressed chunk
        uint32_t sizeof_compressed = __builtin_bswap32(*reinterpret_cast<int*>(buf));

        char decompressed[16 * 4096];

        ssize_t sizeof_decompressed = decompress(buf + 5, sizeof_compressed, decompressed, sizeof(decompressed));

        std::cout << "sizeof_decompressed: " << sizeof_decompressed << std::endl;

        auto* chunk = new Chunk;

        chunkbuf.feed(decompressed, sizeof_decompressed);

        try
        {
            chunkbuf.parse(chunk);
        } catch (const InvalidNBTTypeException& inbtte)
        {
            delete chunk;
            return nullptr;
        }
        return chunk;
    }
    return nullptr;
}

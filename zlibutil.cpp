//
// Created by cory on 5/11/25.
//

#include <zlib.h>
#include "zlibutil.hpp"

int compress(const char* input, size_t input_len, char* output, size_t* output_len)
{
    z_stream stream = {nullptr};
    stream.next_in = (Bytef*) input;
    stream.avail_in = input_len;
    stream.next_out = (Bytef*) output;
    stream.avail_out = *output_len;

    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
        return -1;
    }

    if (deflate(&stream, Z_FINISH) != Z_STREAM_END)
    {
        deflateEnd(&stream);
        return -2;
    }

    *output_len = stream.total_out;
    deflateEnd(&stream);
    return 0;
}

ssize_t decompress(const char* input, size_t input_len, char* output, size_t output_len)
{
    z_stream stream{};
    stream.next_in = (Bytef*) input;
    stream.avail_in = input_len;
    stream.next_out = (Bytef*) output;
    stream.avail_out = output_len;

    if (inflateInit(&stream) != Z_OK)
    {
        return -1;
    }

    int result = inflate(&stream, Z_FINISH);

    if (result != Z_STREAM_END)
    {
        inflateEnd(&stream);
        return -2;
    }

    inflateEnd(&stream);
    return stream.total_out;  // actual decompressed size
}

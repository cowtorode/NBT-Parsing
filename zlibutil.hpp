//
// Created by cory on 5/11/25.
//

#ifndef CPPTEST_ZLIBUTIL_HPP
#define CPPTEST_ZLIBUTIL_HPP


#include <cstddef>

int compress(const char* input, size_t input_len, char* output, size_t* output_len);

ssize_t decompress(const char* input, size_t input_len, char* output, size_t output_len);


#endif //CPPTEST_ZLIBUTIL_HPP

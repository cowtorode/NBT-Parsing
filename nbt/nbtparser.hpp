//
// Created by cory on 4/11/25.
//

#ifndef CLAMS_READBUFFER_HPP
#define CLAMS_READBUFFER_HPP


#include <stack>
#include <unordered_map>
#include <string>
#include <cstdint>
#include "nbtcompound.hpp"

typedef void*(*handle_mutator)(void*);

typedef void(*byte_router)(void*, int8_t);
typedef void(*short_router)(void*, int16_t);
typedef void(*int_router)(void*, int32_t);
typedef void(*long_router)(void*, int64_t);
typedef void(*float_router)(void*, float);
typedef void(*double_router)(void*, double);
typedef void(*byte_array_router)(void*, int32_t, char[]);
typedef void(*string_router)(void*, const std::string&);

typedef void(*byte_list_router)(void*, int32_t, char[]);
typedef void(*short_list_router)(void*, int32_t, int16_t[]);
typedef void(*int_list_router)(void*, int32_t, int32_t[]);
typedef void(*long_list_router)(void*, int32_t, int64_t[]);
typedef void(*float_list_router)(void*, int32_t, float[]);
typedef void(*double_list_router)(void*, int32_t, double[]);
typedef void(*string_list_router)(void*, int32_t, const std::string[]);

typedef void(*int_array_router)(void*, int32_t, int32_t[]);
typedef void(*long_array_router)(void*, int32_t, int64_t[]);

class NBTRouter
{
public:
    handle_mutator next_handle = [](void* handle){ return handle; };

    void set_compound_router(const std::string& str, const NBTRouter& router);

    void set_compound_list_router(const std::string& str, const NBTRouter& router);

    void compounds_find(const std::string& str, const NBTRouter*& next) const;

    void set_byte_router(const std::string& str, byte_router router);

    void set_short_router(const std::string& str, short_router router);

    void set_int_router(const std::string& str, int_router router);

    void set_long_router(const std::string& str, long_router router);

    void set_float_router(const std::string& str, float_router router);

    void set_double_router(const std::string& str, double_router router);

    void set_byte_array_router(const std::string& str, byte_array_router router);

    void set_string_router(const std::string& str, string_router router);

    void set_byte_list_router(const std::string& str, byte_list_router router);

    void set_short_list_router(const std::string& str, short_list_router router);

    void set_int_list_router(const std::string& str, int_list_router router);

    void set_long_list_router(const std::string& str, long_list_router router);

    void set_float_list_router(const std::string& str, float_list_router router);

    void set_double_list_router(const std::string& str, double_list_router router);

    void set_string_list_router(const std::string& str, string_list_router router);

    void set_int_array_router(const std::string& str, int_array_router router);

    void set_long_array_router(const std::string& str, long_array_router router);

    void route_byte(const std::string& str, void* handle, int8_t value) const;

    void route_short(const std::string& str, void* handle, int16_t value) const;

    void route_int(const std::string& str, void* handle, int32_t value) const;

    void route_long(const std::string& str, void* handle, int64_t value) const;

    void route_float(const std::string& str, void* handle, float value) const;

    void route_double(const std::string& str, void* handle, double value) const;

    void route_byte_array(const std::string& str, void* handle, int32_t len, char* value) const;

    void route_string(const std::string& str, void* handle, const std::string& value) const;

    void route_byte_list(const std::string& str, void* handle, int32_t len, char value[]) const;

    void route_short_list(const std::string& str, void* handle, int32_t len, int16_t value[]) const;

    void route_int_list(const std::string& str, void* handle, int32_t len, int32_t value[]) const;

    void route_long_list(const std::string& str, void* handle, int32_t len, int64_t value[]) const;

    void route_float_list(const std::string& str, void* handle, int32_t len, float value[]) const;

    void route_double_list(const std::string& str, void* handle, int32_t len, double value[]) const;

    void route_string_list(const std::string& str, void* handle, int32_t len, const std::string value[]) const;

    void route_int_array(const std::string& str, void* handle, int32_t len, int32_t value[]) const;

    void route_long_array(const std::string& str, void* handle, int32_t len, int64_t value[]) const;
private:
    std::unordered_map<std::string, const void*> routes;
};

/**
 * Thrown when you try to route completely out of the bounds of the buffers fed to the NBTParser.
 */
class BufferOverflowException : std::exception {};

class InvalidNBTTypeException : std::exception {};

class NBTParser
{
public:
    NBTParser();

    void parse(void* handle);

    void parse(NBTCompound& compound);

    void set_router(const NBTRouter& router);

    void feed(char* buffer, size_t size);
private:
    void skip(uint64_t bytes);

    int8_t read_char();

    uint16_t read_ushort();

    int16_t read_short();

    uint32_t read_uint();

    int32_t read_int();

    uint64_t read_ulong();

    int64_t read_long();

    float read_float();

    double read_double();

    std::string read_string();

    void route_byte(const std::string& name);

    void route_short(const std::string& name);

    void route_int(const std::string& name);

    void route_long(const std::string& name);

    void route_float(const std::string& name);

    void route_double(const std::string& name);

    void route_byte_array(const std::string& name);

    void route_string(const std::string& name);

    void route_list(const std::string& name);

    void route_compound_list(const std::string& name, int32_t len);

    void route_compound(const std::string& name);

    void route_int_array(const std::string& name);

    void route_long_array(const std::string& name);

    void route(int8_t type);

    [[nodiscard]] inline size_t sector_remaining() const;

    [[nodiscard]] inline const NBTRouter& router();

    void set_handle(void* handle);

    [[nodiscard]] inline void* handle();

    /**
     * Sets the next router and handle in the stack
     */
    void push(const NBTRouter& router);

    struct StackElement
    {
        const NBTRouter& router;
        void* handle;

        StackElement(const NBTRouter& router, void* handle);

        StackElement(const NBTRouter& router);
    };

    char* cursor;
    char* end;
    std::stack<StackElement> parser_stack;
};


#endif //CLAMS_READBUFFER_HPP

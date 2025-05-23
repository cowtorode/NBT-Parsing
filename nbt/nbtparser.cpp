//
// Created by cory on 4/11/25.
//

#define PRINT_UNROUTED
#define PRINT_NBT

#include <bit>

#if defined(PRINT_UNROUTED) || defined(PRINT_NBT)
#include <iostream>
#endif

#ifdef PRINT_UNROUTED
#define unrouted(stmt) stmt
#elif
#define unrouted(stmt)
#endif

#ifdef PRINT_NBT
#define debug(stmt) stmt
#elif
#define debug(stmt)
#endif

#include "nbtparser.hpp"

#define TAG_End (0)
#define TAG_Byte (1)
#define TAG_Short (2)
#define TAG_Int (3)
#define TAG_Long (4)
#define TAG_Float (5)
#define TAG_Double (6)
#define TAG_Byte_Array (7)
#define TAG_String (8)
#define TAG_List (9)
#define TAG_Compound (10)
#define TAG_Int_Array (11)
#define TAG_Long_Array (12)

NBTParser::StackElement::StackElement(const NBTRouter& router) : router(router)
{}

NBTParser::StackElement::StackElement(const NBTRouter& router, void* handle) : router(router), handle(handle)
{}

static const NBTRouter EMPTY_NBT_ROUTER{};

NBTParser::NBTParser() : end(nullptr), cursor(nullptr)
{}

inline size_t NBTParser::sector_remaining() const
{
    return end - cursor;
}

void NBTParser::push(const NBTRouter& router)
{
    parser_stack.push({router, router.next_handle(handle())});
}

void NBTParser::set_router(const NBTRouter& r)
{
    // clear the stack
    // (so stupid)
    for (int i = 0; i < parser_stack.size(); ++i)
    {
        parser_stack.pop();
    }

    parser_stack.push(r);
}

void NBTParser::set_handle(void* handle)
{
    parser_stack.top().handle = handle;
}

const NBTRouter& NBTParser::router()
{
    return parser_stack.top().router;
}

void* NBTParser::handle()
{
    return parser_stack.top().handle;
}

void NBTParser::feed(char* buffer, size_t size)
{
    cursor = buffer;
    end = buffer + size;
}

void NBTParser::skip(uint64_t bytes)
{
    cursor += bytes;
}

int8_t NBTParser::read_char()
{
    if (sector_remaining() < sizeof(int8_t))
    {
        throw BufferOverflowException();
    }

    return *cursor++;
}

uint16_t NBTParser::read_ushort()
{
    if (sector_remaining() < sizeof(uint16_t))
    {
        throw BufferOverflowException();
    }

    // route the bytes
    uint16_t rax = __builtin_bswap16(*reinterpret_cast<uint16_t*>(cursor));
    // increment the cursor
    cursor += sizeof(uint16_t);
    // return the bytes
    return rax;
}

int16_t NBTParser::read_short()
{
    return std::bit_cast<short>(read_ushort());
}

uint32_t NBTParser::read_uint()
{
    if (sector_remaining() < sizeof(uint32_t))
    {
        throw BufferOverflowException();
    }

    // route the bytes
    uint32_t rax = __builtin_bswap32(*reinterpret_cast<uint32_t*>(cursor));
    // increment the cursor
    cursor += sizeof(uint32_t);
    // return the bytes
    return rax;
}

int32_t NBTParser::read_int()
{
    return std::bit_cast<int>(read_uint());
}

uint64_t NBTParser::read_ulong()
{
    if (sector_remaining() < sizeof(uint64_t))
    {
        throw BufferOverflowException();
    }

    // route the bytes
    uint64_t rax = __builtin_bswap64(*reinterpret_cast<uint64_t*>(cursor));
    // increment the cursor
    cursor += sizeof(uint64_t);
    // return the bytes
    return rax;
}

int64_t NBTParser::read_long()
{
    return std::bit_cast<long>(read_ulong());
}

float NBTParser::read_float()
{
    return std::bit_cast<float>(read_uint());
}

double NBTParser::read_double()
{
    return std::bit_cast<double>(read_ulong());
}

std::string NBTParser::read_string()
{
    uint16_t length = read_ushort();

    if (sector_remaining() < length)
    {
        throw BufferOverflowException();
    }

    std::string rax(cursor, length);
    cursor += length;
    return rax;
}

int indent = 0;

void NBTParser::route_byte(const std::string& name)
{
    int8_t byte = read_char();

    router().route_byte(name, handle(), byte);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << (int) byte << std::endl;)
}

void NBTParser::route_short(const std::string& name)
{
    int16_t data = read_short();

    router().route_short(name, handle(), data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << data << std::endl;)
}

void NBTParser::route_int(const std::string& name)
{
    int32_t data = read_int();

    router().route_int(name, handle(), data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << data << std::endl;)
}

void NBTParser::route_long(const std::string& name)
{
    int64_t data = read_long();

    router().route_long(name, handle(), data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << data << std::endl;)
}

void NBTParser::route_float(const std::string& name)
{
    float data = read_float();

    router().route_float(name, handle(), data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << data << std::endl;)
}

void NBTParser::route_double(const std::string& name)
{
    double data = read_double();

    router().route_double(name, handle(), data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << data << std::endl;)
}

void NBTParser::route_byte_array(const std::string& name)
{
    int32_t len = read_int();

    if (sector_remaining() < len)
    {
        throw BufferOverflowException();
    }

    char* buf = cursor;
    cursor += len;

    router().route_byte_array(name, handle(), len, buf);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": " << std::string(buf, len) << std::endl;)
}

void NBTParser::route_string(const std::string& name)
{
    std::string str = read_string();

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": ";)

    router().route_string(name, handle(), str);

    debug(std::cout << str << '"' << std::endl;)
}

void NBTParser::route_list(const std::string& name)
{
    int8_t tag_id = read_char();
    int32_t len = read_int();
    debug(std::cout << std::string(indent, ' ') << '"' << name << "\" (len: " << len << "): ";)

    switch (tag_id)
    {
        case TAG_End: // Empty List
            debug(std::cout << std::endl;)
            break;
        case TAG_Byte:
        {
            cursor += len;
            debug(std::cout << "[data]" << std::endl;)
            break;
        }
        case TAG_Short:
            cursor += len * sizeof(int16_t);
            debug(std::cout << "[data]" << std::endl;)
            break;
        case TAG_Int:
            cursor += len * sizeof(int32_t);
            debug(std::cout << "[data]" << std::endl;)
            break;
        case TAG_Long:
            cursor += len * sizeof(int64_t);
            debug(std::cout << "[data]" << std::endl;)
            break;
        case TAG_Float:
            debug(std::cout << "[data]" << std::endl;)
            break;
        case TAG_Double:
            debug(std::cout << "[data]" << std::endl;)
            break;
        case TAG_String:
            debug(std::cout << "{";
            for (int i = 0; i < len - 1; ++i)
            {
                std::cout << '"' << read_string() << "\", ";
            }
            std::cout << '"' << read_string() << "\"}" << std::endl;)
            break;
        case TAG_Compound:
        {
            route_compound_list(name, len);
            break;
        }
        default:
            debug(std::cout << "tag: " << (int) tag_id << std::endl;)
            // excp
            break;
    }
}

void NBTParser::route_compound_list(const std::string& name, int32_t len)
{
    debug(std::cout << std::endl;)

    const NBTRouter* next = nullptr;

    router().compounds_find(name, next);

    if (next)
    {
        // there is a router for this next compound
        parser_stack.push(*next);
    } else
    {
        parser_stack.push(EMPTY_NBT_ROUTER);
    }

    for (int i = 0; i < len; ++i)
    {
        int8_t type;

        // while (type != TAG_End)
        while ((type = read_char()))
        {
            route(type);
        }
    }

    parser_stack.pop();
}

void NBTParser::route_compound(const std::string& name)
{
    const NBTRouter* next = nullptr;

    router().compounds_find(name, next);

    if (next)
    {
        // there is a router for this next compound
        push(*next);
    } else
    {
        parser_stack.push(EMPTY_NBT_ROUTER);
    }

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": "<< '{' << std::endl;
    indent += 2;)

    int8_t type;

    // while (type != TAG_End)
    while ((type = read_char()))
    {
        route(type);
    }

    debug(indent -= 2;
    std::cout << std::string(indent, ' ') << '}' << std::endl;)

    parser_stack.pop();
}

void NBTParser::route_int_array(const std::string& name)
{
    int32_t len = read_int();
    int32_t data[len];

    for (int32_t i = 0; i < len; ++i)
    {
        data[i] = read_int();
    }

    router().route_int_array(name, handle(), len, data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": {";

    for (int i = 0; i < len - 1; ++i)
    {
        std::cout << data[i] << ", ";
    }

    std::cout << data[len - 1] << '}' << std::endl;)
}

void NBTParser::route_long_array(const std::string& name)
{
    int32_t len = read_int();
    int64_t data[len];

    for (int32_t i = 0; i < len; ++i)
    {
        data[i] = read_long();
    }

    router().route_long_array(name, handle(), len, data);

    debug(std::cout << std::string(indent, ' ') << '"' << name << "\": {";

    for (int i = 0; i < len - 1; ++i)
    {
        std::cout << data[i] << ", ";
    }

    std::cout << data[len - 1] << '}' << std::endl;)
}

void NBTParser::route(int8_t type)
{
    switch (type)
    {
        case TAG_End:
            break;
        case TAG_Byte:
            route_byte(read_string());
            break;
        case TAG_Short:
            route_short(read_string());
            break;
        case TAG_Int:
            route_int(read_string());
            break;
        case TAG_Long:
            route_long(read_string());
            break;
        case TAG_Float:
            route_float(read_string());
            break;
        case TAG_Double:
            route_double(read_string());
            break;
        case TAG_Byte_Array:
            route_byte_array(read_string());
            break;
        case TAG_String:
            route_string(read_string());
            break;
        case TAG_List:
            route_list(read_string());
            break;
        case TAG_Compound:
            route_compound(read_string());
            break;
        case TAG_Int_Array:
            route_int_array(read_string());
            break;
        case TAG_Long_Array:
            route_long_array(read_string());
            break;
        default:
            throw InvalidNBTTypeException();
    }
}

void NBTParser::parse(void* handle)
{
    set_handle(handle);

    while (sector_remaining() > 0)
    {
        route(read_char());
    }
}

void NBTParser::parse(NBTCompound& compound)
{

}

void NBTRouter::set_compound_router(const std::string& str, const NBTRouter& router)
{
    routes[str] = &router;
}

void NBTRouter::set_compound_list_router(const std::string& str, const NBTRouter& router)
{
    routes[str] = &router;
}

void NBTRouter::compounds_find(const std::string& str, const NBTRouter*& next) const
{
    auto itr = routes.find(str);

    if (itr != routes.end())
    {
        next = (const NBTRouter*) itr->second;
    }
}

void NBTRouter::set_byte_router(const std::string& str, byte_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_short_router(const std::string& str, short_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_int_router(const std::string& str, int_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_long_router(const std::string& str, long_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_float_router(const std::string& str, float_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_double_router(const std::string& str, double_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_byte_array_router(const std::string& str, byte_array_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_string_router(const std::string& str, string_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_byte_list_router(const std::string& str, byte_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_short_list_router(const std::string& str, short_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_int_list_router(const std::string& str, int_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_long_list_router(const std::string& str, long_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_float_list_router(const std::string& str, float_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_double_list_router(const std::string& str, double_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_string_list_router(const std::string& str, string_list_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_int_array_router(const std::string& str, int_array_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::set_long_array_router(const std::string& str, long_array_router router)
{
    routes[str] = (void*) router;
}

void NBTRouter::route_byte(const std::string& str, void* handle, int8_t value) const
{
    auto itr = routes.find(str);

    if (itr != routes.end())
    {
        ((byte_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_short(const std::string& str, void* handle, int16_t value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((short_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_int(const std::string& str, void* handle, int32_t value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((int_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_long(const std::string& str, void* handle, int64_t value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((long_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_float(const std::string& str, void* handle, float value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((float_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_double(const std::string& str, void* handle, double value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((double_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_byte_array(const std::string& str, void* handle, int32_t len, char* value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((byte_array_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_string(const std::string& str, void* handle, const std::string& value) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((string_router) itr->second)(handle, value);
    }
}

void NBTRouter::route_byte_list(const std::string& str, void* handle, int32_t len, char value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((byte_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_short_list(const std::string& str, void* handle, int32_t len, int16_t value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((short_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_int_list(const std::string& str, void* handle, int32_t len, int32_t value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((int_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_long_list(const std::string& str, void* handle, int32_t len, int64_t value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((long_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_float_list(const std::string& str, void* handle, int32_t len, float value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((float_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_double_list(const std::string& str, void* handle, int32_t len, double value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((double_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_string_list(const std::string& str, void* handle, int32_t len, const std::string value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((string_list_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_int_array(const std::string& str, void* handle, int32_t len, int32_t value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((int_array_router) itr->second)(handle, len, value);
    }
}

void NBTRouter::route_long_array(const std::string& str, void* handle, int32_t len, int64_t value[]) const
{
    auto itr = routes.find(str);
    if (itr != routes.end())
    {
        ((long_array_router) itr->second)(handle, len, value);
    }
}

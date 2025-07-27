#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <string>
#include <stdexcept>
// This program aims to take in a struct and a reflection, and serialise it into binary, and deserialize it

// Delimiter approach (SOH) → works like text protocols, but needs escaping and scanning. A text protocol is simply a wire‑format where every message or frame is encoded as human‑readable text (usually ASCII or UTF‑8), rather than packed binary.

//Length‑prefix approach → unambiguous, faster, no escaping, and what most binary formats (protobuf, msgpack, etc.) use.

// 1: Gather reflection meta data
// you need the type for each type ( int, char, string, possibly can be a nested struct)
// declaration order
// length for the strings / arrays

// 2: define wire format rules
// fixed primitives mapped to a fixed number of bytes
// for dynamic container like string / vector, you add length followed by its constituents
// for fixed size container like std::array<T, N>, you already know N at reflection time so dont have to serialise it
// for nested structs, serialize fields recursively

// buffer is a contiguous, growable array of raw bytes & uint8_t is the standard unsigned integer type that represents one byte 

// inline means 1. its ok for this function to have identical definitins in multiple translation units and
// 2. it requests the compiler substituting the fns body at each call site


// Reflect::Object is not real. c++ has not implemented std::reflect yet.

// Big endian
void append_uint32(std::vector<uint8_t>& buf, uint32_t v) {
    buf.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >>  8) & 0xFF));
    buf.push_back(static_cast<uint8_t>( v        & 0xFF));
}

uint32_t read_uint32_be(const std::vector<uint8_t>& buf, size_t pos) {
    return (uint32_t(buf[pos])   << 24) |
           (uint32_t(buf[pos + 1]) << 16) |
           (uint32_t(buf[pos + 2]) <<  8) |
           (uint32_t(buf[pos + 3])      );
}


void serialize(const Reflect::Object& obj, std::vector<uint8_t>& buf) {
    for (const auto& field : obj.fields()) {
        switch (field.type) {
            case Type::Int32: {
                int32_t v = obj.get<int32_t>(field);
                append_int32_be(buf, v);
                break;
            }
            case Type::String: {
                auto& s = obj.get<std::string>(field);
                append_uint32_be(buf, (uint32_t)s.size());
                buf.insert(buf.end(), s.begin(), s.end());
                break;
            }
            case Type::Struct: {
                auto nested = obj.getNested(field);
                serialize(nested, buf);
                break;
            }
            default:
                throw std::runtime_error("Unsupported field type");
        }
    }
}


void deserialize(const std::vector<uint8_t>& buf,
                 size_t& pos,
                 Reflect::Object& obj)
{
    for (const auto& field : obj.fields()) {
        switch (field.type) {
            case Type::Int32: {
                if (pos + 4 > buf.size()) throw std::runtime_error("…");
                int32_t v = read_int32_be(buf, pos);
                pos += 4;
                obj.set<int32_t>(field, v);
                break;
            }
            case Type::String: {
                if (pos + 4 > buf.size()) throw std::runtime_error("…");
                uint32_t n = read_uint32_be(buf, pos);
                pos += 4;
                if (pos + n > buf.size()) throw std::runtime_error("…");
                std::string s((char*)(buf.data() + pos), n);
                pos += n;
                obj.set<std::string>(field, std::move(s));
                break;
            }
            case Type::Struct: {
                auto nested = obj.getNested(field);
                deserialize(buf, pos, nested);
                break;
            }
            default:
                throw std::runtime_error("Unsupported field type");
        }
    }
}





            







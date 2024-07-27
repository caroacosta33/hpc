#ifndef SERIALIZATION
#define SERIALIZATION

#include "../types.h"
#include <iostream>

#include "../cereal/archives/binary.hpp"
#include "../cereal/types/unordered_map.hpp"
#include "../cereal/types/vector.hpp"
#include "../cereal/types/string.hpp"

template<typename T>
std::vector<char> serialize(const T& data) {
    std::ostringstream oss;
    {
        cereal::BinaryOutputArchive archive(oss);
        archive(data);
    }
    std::string str = oss.str();
    return std::vector<char>(str.begin(), str.end());
}

template<typename T>
T deserialize(const std::vector<char>& buffer) {
    std::istringstream iss(std::string(buffer.begin(), buffer.end()));
    T data;
    {
        cereal::BinaryInputArchive archive(iss);
        archive(data);
    }
    return data;
}

// Serialize unordered_map
template<typename K, typename V>
std::vector<char> serialize_map(const std::unordered_map<K, V>& map) {
    return serialize(map);
}

// Deserialize unordered_map
template<typename K, typename V>
std::unordered_map<K, V> deserialize_map(const std::vector<char>& buffer) {
    return deserialize<std::unordered_map<K, V>>(buffer);
}

#endif
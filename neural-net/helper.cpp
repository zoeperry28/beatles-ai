#include <filesystem>

#include "helper.hpp"

bool IsWavFile(std::string path) 
{
    return std::filesystem::path(path).extension() == ".wav";
}

bool IsFolder(std::string path) 
{
    return std::filesystem::path(path).has_extension();
}


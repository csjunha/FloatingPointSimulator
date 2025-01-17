#include "./file_reader.h"

FileReader::FileReader(std::string &filename)
{
    file.open(filename);

    if (!file || !file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
}

FileReader::~FileReader()
{
    if (file.is_open())
        file.close();
}

bool FileReader::read_line(std::string &line)
{
    if (std::getline(file, line))
    {
        return true;
    }
    return false;
}

#include "./file_writer.h"

FileWriter::FileWriter(std::string &filename, bool append)
{
    std::ios::openmode mode = std::ios::out;
    if (append)
        mode |= std::ios::app;

    file.open(filename, mode);

    if (!file || !file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
}

FileWriter::~FileWriter()
{
    if (file.is_open())
        file.close();
}

void FileWriter::write_line(std::string &line)
{
    file << line << std::endl;
}

void FileWriter::write_as_binary(uint64_t num, uint8_t num_bits)
{
    std::string line(num_bits, '0');

    for (int i = num_bits - 1; i >= 0; i--)
    {
        if (num & ((uint64_t)1 << i))
            line[num_bits - 1 - i] = '1';
    }

    this->write_line(line);
}

bool FileWriter::is_open() const
{
    return file.is_open();
}

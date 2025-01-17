#ifndef __FILE_WRITER_H__
#define __FILE_WRITER_H__

#include <string>
#include <fstream>
#include <iostream>

class FileWriter
{
public:
    explicit FileWriter(std::string &filename, bool append = false);

    ~FileWriter();

    void write_line(std::string &line);
    void write_as_binary(uint64_t num, uint8_t num_bits);

    bool is_open() const;

private:
    std::ofstream file;

};

#endif // __WRITER_H__

#ifndef __FILE_READER_H__
#define __FILE_READER_H__

#include <string>
#include <fstream>
#include <iostream>

class FileReader
{
public:
    explicit FileReader(std::string &filename);

    ~FileReader();

    bool read_line(std::string &line);

private:
    std::ifstream file;
};

#endif // __WRITER_H__

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>

class SizeSeqHandler
{

public:

    SizeSeqHandler();
    ~SizeSeqHandler();

    void addId(std::string id, long size);
    void removeId(std::string id);
    long getSizeForId(std::string id);
    void deleteTempFile();

private:

    std::map<std::string, long> idSize;
};
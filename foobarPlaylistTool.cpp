#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

// 1M buffer size
#define READ_BUFF_SIZE 1048576

const uint8_t FPL_MAGIC[16] = {
    0xe1, 0xa0, 0x9c, 0x91,
    0xf8, 0x3c, 0x77, 0x42,
    0x85, 0x2c, 0x3b, 0xcc,
    0x14, 0x01, 0xd3, 0xf2
};

class FooItem
{
public:
    FooItem(std::filesystem::path filePath);

private:
    std::filesystem::path m_filePath;

};

enum class ParseState
{
    IDLE = 0,
    READ_FILE = 1
};


void parseFPL(const std::filesystem::path& fplFilePath, bool hasDriveLetters)
{
    std::vector<char> buf;

    {
        // file read-in and .fpl magic check
        buf.reserve(READ_BUFF_SIZE);

        char curChar;
        std::ifstream fileStream(fplFilePath);

        int readChars = 0;

        while (fileStream.get(curChar)) 
        {
            buf.push_back(curChar);

            readChars++;

            if (readChars == 16)
            {

                // first 16 bytes seem to be a kind of magic
                // at least all of my playlists share this pattern
                for (int i = 0; i < 16; i++)
                {
                    if ((uint8_t) buf[i] != FPL_MAGIC[i])
                    {
                        std::cout << "File does not seem to be a Foobar2000 playlist file!" << std::endl;
                        return;

                    }

                }

            }

        }

    }

    uint32_t* stopIndex = (uint32_t*) &buf[16];
    *stopIndex += 16 + 4;

    if (*stopIndex >= buf.size())
    {
        std::cout << "Parsing error - File corrupt!" << std::endl;
        return;

    }

    std::vector<std::string> parsedContent;

    std::vector<char> strBuf;
    strBuf.reserve(buf.size());

    char el;
    for (int i = 20; i < *stopIndex; i++)
    {
        el = buf[i];

        strBuf.push_back(el);

        if (el == '\0')
        {
            char* curString = &strBuf[0];

            parsedContent.emplace_back(curString);

            strBuf.clear();

        }

    }

    std::vector<std::string> files;

    for (auto& str : parsedContent)
    {
        if (str.length() >= 7 && str.substr(0, 7) == "file://")
        {
            char* filePath = &str[7];

            files.emplace_back(filePath);

        }

    }

    std::vector<std::filesystem::path> filePaths;

    for (std::string strPath : files)
    {   
        // as foobar2000 is only available on Windows
        // there should be no issue replacing all backslashes
        // as they are there not considered valid name chars
        std::replace(strPath.begin(), strPath.end(), '\\', '/');

        std::filesystem::path filePath;

        filePath = std::filesystem::path(strPath);

        std::cout << filePath.string() << std::endl;

        if (!(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)))
        {
            std::cout << "\tFile not found!" << std::endl;

        }

    }

}


void parseInputs(const std::vector<std::string>& commList)
{
    ParseState state = ParseState::IDLE;

    for (const std::string& inStr : commList)
    {
        // process any remaining commands
        switch (state)
        {
            case ParseState::READ_FILE:
                {
                    std::filesystem::path inFile(inStr);

                    if (std::filesystem::exists(inFile) &&
                        std::filesystem::is_regular_file(inFile))
                    {
                        std::cout << "\t" << inFile.filename() << std::endl;
                        if (inFile.extension() == std::filesystem::path(".fpl")) {
                            parseFPL(inFile, true);

                        } else {
                            std::cout << "File does not seem to be a Foobar2000 playlist file!" << std::endl;

                        }
                        
                    } else {
                        std::cerr << "Given filepath does not contain a file!" << std::endl;

                    }

                }
            
            default:
                state = ParseState::IDLE;
                break;

        }

        // command received
        if (inStr[0] == '-')
        {
            if (inStr.length() <= 1)
                return;

            char input = inStr[1];

            switch (input)
            {
                case 'f':
                    std::cout << "Reading in playlist file..." << std::endl;
                    state = ParseState::READ_FILE;
                    break;
                
                default:
                    std::cout << "Command <" << input << "> not found!" << std::endl;
                    break;

            }

        }

    }

}

int main(int argc, char* argv[])
{
    std::vector<std::string> inputArgs;

    if (argc > 1)
    {
        for (int arg = 0; arg < argc; arg++)
        {
            char* current = argv[arg];

            inputArgs.emplace_back(current);

        }

        parseInputs(inputArgs);

    }

    return 0;

}
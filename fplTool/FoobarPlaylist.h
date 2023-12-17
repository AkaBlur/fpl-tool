#pragma once

#include <filesystem>
#include <regex>
#include <vector>

// 1M buffer size for file buffer
#define READ_BUFF_SIZE 1048576

namespace FPLTool {
    class FoobarPlaylist
    {
    public:
        FoobarPlaylist(const std::filesystem::path& fplFile) :
            m_fplPath(fplFile) {};

        void parseFPL();

        void regexOnPath(const std::regex& deleteReg, const std::string& appendStr);
        
        void checkExisting(bool removeDeads);

        void printFiles();

    private:
        // stored fpl playlist file
        std::filesystem::path m_fplPath;

        std::vector<std::filesystem::path> m_playlistFiles;

        const uint8_t FPL_MAGIC[16] = {
            0xe1, 0xa0, 0x9c, 0x91,
            0xf8, 0x3c, 0x77, 0x42,
            0x85, 0x2c, 0x3b, 0xcc,
            0x14, 0x01, 0xd3, 0xf2
        };

    };

}


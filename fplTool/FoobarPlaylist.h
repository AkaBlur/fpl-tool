#pragma once

#include <filesystem>
#include <regex>
#include <vector>

#include "CommandQueue.h"
#include "MediaEntry.h"

// 1M buffer size for file buffer
#define FILE_BUFF_SIZE 1048576
// 4k buffer size for chunks
#define READ_BUFF_SIZE 4096
// FPL magic size [bytes]
#define FPL_MAGIC_SIZE 16
// FPL metasize size [bytes]
#define FPL_METASIZE_SIZE 4
// start of meta block
#define FPL_META_START 20

namespace FPLTool {
class FoobarPlaylist
{
public:
    FoobarPlaylist(const std::filesystem::path& fplFile) :
        m_fplPath(fplFile) {};

    // parse .fpl binary
    void parseFPL();

    // applies a regex to the filepath for each entry
    void regexOnPath(const std::regex& deleteReg, const std::string& appendStr);
    
    // checks if all files exist and optionally remove non-existing ones
    void checkExisting(bool removeDeads);

    // prints the playlist normally formatted
    void printEntrynames();

    // print the playlist in given output mode
    void printCustomMode(CommandQueue::CommandOutputMode outputMode);

    // USE WITH CAUTION! - No size checks!
    // returns a null-terminated string from the file buffer
    std::string getStringFromBuf(uint64_t offset) const;

private:
    // stored fpl playlist file
    std::filesystem::path m_fplPath;
    // buffer for the raw file contents, saved for later processing if required
    std::vector<char> m_fileBuffer;

    std::vector<MediaEntry> m_playlistEntries;

    const uint8_t FPL_MAGIC[16] = {
        0xe1, 0xa0, 0x9c, 0x91,
        0xf8, 0x3c, 0x77, 0x42,
        0x85, 0x2c, 0x3b, 0xcc,
        0x14, 0x01, 0xd3, 0xf2
    };

    

};

}


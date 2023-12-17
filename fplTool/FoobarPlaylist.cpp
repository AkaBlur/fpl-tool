#include <algorithm>
#include <fstream>

#include "FoobarPlaylist.h"
#include "Log.h"

namespace FPLTool
{
    void FoobarPlaylist::parseFPL()
    {
        std::vector<char> buf;

        {
            // file read-in and .fpl magic check
            buf.reserve(READ_BUFF_SIZE);

            char curChar;
            std::ifstream fileStream(m_fplPath);

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
                            LOG_ERROR("File '%s' does not seem to be a Foobar2000 playlist file!",
                                m_fplPath.string().c_str());
                            return;

                        }

                    }

                }

            }

        }

        // check for file length and pointer to end of file list
        uint32_t* stopIndex = (uint32_t*) &buf[16];
        *stopIndex += 16 + 4;

        if (*stopIndex >= buf.size())
        {
            LOG_ERROR("Parsing error - File corrupt!");
            return;

        }

        // parsing for all keywords inside fpl
        std::vector<std::string> parsedContent;

        std::vector<char> strBuf;
        strBuf.reserve(buf.size());

        char el;
        for (uint32_t i = 20; i < *stopIndex; i++)
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

        // parsing for "file://" keyword
        for (auto& str : parsedContent)
        {
            if (str.length() >= 7 && str.substr(0, 7) == "file://")
            {
                char* filePath = &str[7];
                std::string strPath(filePath);

                // as foobar2000 is only available on Windows
                // there should be no issue replacing all backslashes
                // as they are there not considered valid name chars
                std::replace(strPath.begin(), strPath.end(), '\\', '/');

                // fpl file is encoded with UTF-8 chars
                // TODO: change this when transitioning from c++-17
                m_playlistFiles.emplace_back(std::filesystem::u8path(strPath));
                LOG_DEBUG("File added\n\t%s", m_playlistFiles.back().u8string().c_str());

            }

        }

    }

    void FoobarPlaylist::regexOnPath(const std::regex& deleteReg, const std::string& appendStr)
    {
        for (auto pathIt = m_playlistFiles.begin(); pathIt != m_playlistFiles.end(); pathIt++)
        {
            std::filesystem::path curPath((*pathIt));
            std::string filePath(std::regex_replace(curPath.string(), deleteReg, appendStr));

            (*pathIt) = std::filesystem::path(filePath);

        }

    }

    void FoobarPlaylist::checkExisting(bool removeDeads)
    {
        std::vector<std::filesystem::path> existingFiles;
        
        for (auto& filePath : m_playlistFiles)
        {
            if (!(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)))
            {
                // TODO: change when transitioning from c++-17 (or not? idk)
                LOG_INFO("%s - File not found!", filePath.u8string().c_str());

            } else {
                existingFiles.push_back(filePath);

            }

        }

        if (removeDeads)
        {
            m_playlistFiles = existingFiles;

        }

    }
    
    void FoobarPlaylist::printFiles()
    {
        for (auto& filePath : m_playlistFiles)
        {
            LOG_OUTPUT("%s", filePath.string().c_str());

        }

    }

}

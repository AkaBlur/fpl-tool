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

            // while reading the file, UTF-8 doesn't matter
            // atleast if we don't apply some conversion inbetween (like constructing some UTF-8 string from const char*)
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

        // At this stage UTF-8 conversion will take over!
        // parsing for "file://" keyword
        for (auto& str : parsedContent)
        {
            if (str.length() >= 7 && str.substr(0, 7) == "file://")
            {
                const char8_t* filePath = reinterpret_cast<const char8_t*>(&str[7]);
                std::u8string strPath(filePath);

                // as foobar2000 is only available on Windows
                // there should be no issue replacing all backslashes
                // as they are there not considered valid name chars
                std::replace(strPath.begin(), strPath.end(), '\\', '/');

                // fpl file is encoded with UTF-8 chars
                m_playlistFiles.emplace_back(std::filesystem::path(strPath));
                LOG_DEBUG("File added\n\t%s", m_playlistFiles.back().u8string().c_str());

            }

        }

    }

    void FoobarPlaylist::regexOnPath(const std::regex& deleteReg, const std::string& appendStr)
    {
        for (auto pathIt = m_playlistFiles.begin(); pathIt != m_playlistFiles.end(); pathIt++)
        {
            std::filesystem::path curPath((*pathIt));
            //NOTE: regex and UTF-8
            /*
            * This is a workaround to get UTF-8 encoded characters in filenames working (which can be a thing, atleast in my libraries).
            * First:
            *       Converting the current path (u8String) to string, via reinterpret_cast
            *       but conversion is from c-style const char8_t* to const char*.
            *       Here no UTF-8 conversion is happening, compiler just changes the types.
            * 
            * Second:
            *       Doing regex_replace on normal string path
            * 
            * Third:
            *       First step in reverse and construct a new UTF-8 path (from u8String)
            * 
            * The only issue is, that there is no way regex is going to work with UTF-8 characters.
            */
            std::u8string u8Path(curPath.u8string());
            std::string strPath(reinterpret_cast<const char*>(u8Path.c_str()));
            
            strPath = std::regex_replace(strPath, deleteReg, appendStr);
            u8Path = std::u8string(reinterpret_cast<const char8_t*>(strPath.c_str()));

            (*pathIt) = std::filesystem::path(u8Path);

        }

    }

    void FoobarPlaylist::checkExisting(bool removeDeads)
    {
        std::vector<std::filesystem::path> existingFiles;
        
        for (auto& filePath : m_playlistFiles)
        {
            if (!(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)))
            {
                // printing char8_t works (but Windows console won't display it)
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
            // printing char8_t works (but Windows console won't display it)
            LOG_OUTPUT("%s", filePath.u8string().c_str());

        }

    }

}

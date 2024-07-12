#include <algorithm>

#include "FoobarPlaylist.h"
#include "MediaEntry.h"
#include "Log.h"

namespace FPLTool
{
void MediaEntry::SetLocation(const std::string& location)
{
    switch (m_mediaType)
    {
        case MediaEntry::MediaType::AUDIO_FILE:
        {
            // filenames can be encoded with UTF-8 characters
            const char8_t* filepath = reinterpret_cast<const char8_t*>(location.c_str());
            std::u8string fileUTF8(filepath);

#ifndef PLATFORM_WIN
            // on non-Windows platforms the replacement needs to be done
            std::replace(fileUTF8.begin(), fileUTF8.end(), '\\', '/');
#endif
            m_filepath = fileUTF8;
            break;

        }
        
        case MediaEntry::MediaType::AUDIO_STREAM:
            m_fileURL = std::string(location);
            break;

        case MediaEntry::MediaType::NOT_SET:
            LOG_ERROR("MediaType not set!");
            break;

        default:
            break;

    }

}

std::u8string const MediaEntry::GetLocation()
{
    switch (m_mediaType)
    {
        case MediaEntry::MediaType::AUDIO_FILE:
            return std::u8string(reinterpret_cast<const char8_t*>(m_filepath.u8string().c_str()));
            break;
        
        case MediaEntry::MediaType::AUDIO_STREAM:
            return std::u8string(reinterpret_cast<const char8_t*>(m_fileURL.c_str()));
            break;

        case MediaEntry::MediaType::NOT_SET:
            LOG_ERROR("MediaType not set!");

        default:
            return 0;
            break;

    }

}

std::u8string const MediaEntry::GetFPLEntryLocation()
{
    switch (m_mediaType)
    {
        case MediaEntry::MediaType::AUDIO_FILE:
        {
            std::u8string fileUTF8(m_filepath.u8string());

#ifndef PLATFORM_WIN
            // when on non-windows platform replacement is done back to backslash
            std::replace(fileUTF8.begin(), fileUTF8.end(), '/', '\\');
#endif

            return fileUTF8;
            break;

        }
        
        case MediaEntry::MediaType::AUDIO_STREAM:
            return std::u8string(reinterpret_cast<const char8_t*>(m_fileURL.c_str()));
            break;

        case MediaEntry::MediaType::NOT_SET:
            LOG_ERROR("MediaType not set!");

        default:
            return 0;
            break;

    }
}

std::vector<std::string> MediaEntry::FindPrimaryKey(const std::vector<char>& filebuffer, const std::string &key)
{
    // no primary keys set
    if (m_mediaTrackInfo.entries.primaryKeyCount == 0)
        return std::vector<std::string>();
    
    // index of the primary values for given keys
    // in case of discontiguous keys start and end will differ
    // therefore all values in the range are applicable for the given key
    uint32_t entryStartIndex = 0;
    uint32_t entryEndIndex = 0;

    std::vector<std::string> foundValues;

    // for each primary key there are two values: index and offset
    // the index may be discontiguous, but starts at zero
    for (std::vector<uint32_t>::const_iterator it = m_mediaTrackInfo.entries.primaryKeys.cbegin();
        it != m_mediaTrackInfo.entries.primaryKeys.cend(); it++)
    {
        // start and end index values for values to look-up
        // takes care of possible duplicates inside of them
        entryStartIndex = *(it);
        entryEndIndex = entryStartIndex;

        // immediately jump to the offset value for the given primary key
        it++;

        if ((it + 1) != m_mediaTrackInfo.entries.primaryKeys.cend())
        {
            // a primary key entry is followed after this one
            entryEndIndex = *(it + 1) - 1;
            
        } else
        {
            // end of primary keys
            // takes care of duplicates at the end
            entryEndIndex = (m_mediaTrackInfo.entries.primaryValueCount - 1);

        }

        // possible primary key
        std::string entryKey(&filebuffer[FPL_META_START + *(it)]);

        if (entryKey == key)
        {
            for (uint32_t i = entryStartIndex; i <= entryEndIndex; i++)
            {
                uint32_t valueByteOffset = m_mediaTrackInfo.entries.primaryValues[i];
                foundValues.emplace_back(&filebuffer[FPL_META_START + valueByteOffset]);

            }

        }
        
    }

    return foundValues;

}
}
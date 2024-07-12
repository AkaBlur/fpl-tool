#include <algorithm>
#include <cstring>
#include <fstream>

#include "FoobarPlaylist.h"
#include "MediaEntry.h"
#include "Log.h"
#include "Util.h"

namespace FPLTool
{
void FoobarPlaylist::parseFPL()
{
    {
        // file read-in and .fpl magic check
        m_fileBuffer.reserve(FILE_BUFF_SIZE);
        // while reading the file, UTF-8 doesn't matter
        // atleast if we don't apply some conversion inbetween (like constructing some UTF-8 string from const char*)
        char readBuf[READ_BUFF_SIZE];
        uint64_t lastRead = 0;
        std::ifstream fileStream(m_fplPath, (std::ios::binary | std::ios::ate));

        //HACK: read-in of ifstream on Windows is strange
        // for safety reason whole file gets put into stream, reads till eof and then jump back to start
        if (fileStream)
        {
            fileStream.seekg(0);

        } else
        {
            LOG_ERROR("Error while reading in file: %s", m_fplPath.string().c_str());

        }

        // first block read
        fileStream.read(readBuf, READ_BUFF_SIZE);
        lastRead = fileStream.gcount();

        // first 16 bytes seem to be a kind of magic
        for (int i = 0; i < FPL_MAGIC_SIZE; i++)
        {
            if ((uint8_t)readBuf[i] != FPL_MAGIC[i])
            {
                LOG_ERROR("File '%s' does not seem to be a Foobar2000 playlist file!",
                    m_fplPath.string().c_str());
                return;

            }

        }

        m_fileBuffer.insert(m_fileBuffer.end(), readBuf, &readBuf[lastRead]);

        while (fileStream)
        {
            fileStream.read(readBuf, READ_BUFF_SIZE);
            lastRead = fileStream.gcount();
            
            m_fileBuffer.insert(m_fileBuffer.end(), readBuf, &readBuf[lastRead]);

        }

        LOG_DEBUG("%lld byte read", m_fileBuffer.size());
        std::ios_base::iostate streamState = fileStream.rdstate();

        if (streamState == std::ios_base::goodbit)
            LOG_DEBUG("File read - No error");

        if (CHECK_BIT(streamState, std::ios_base::badbit))
            LOG_ERROR("File read - Stream Error!");

        if (CHECK_BIT(streamState, std::ios_base::failbit) && !(CHECK_BIT(streamState, std::ios_base::eofbit)))
            LOG_ERROR("File read - I/O exception!");

        if (CHECK_BIT(streamState, std::ios_base::eofbit))
            LOG_DEBUG("File read - End of file reached");

        fileStream.close();

    }

    // check for file length and pointer to end of file metadata list
    // end of metadata block
    uint32_t metaStop = *((uint32_t*) &m_fileBuffer[16]);
    metaStop += FPL_MAGIC_SIZE + FPL_METASIZE_SIZE;
    std::size_t bufSize = m_fileBuffer.size();
    auto itEnd = m_fileBuffer.end();
    if (metaStop >= m_fileBuffer.size())
    {
        LOG_ERROR("Parsing error - File corrupt!");
        return;

    }

    // parsing for all track information inside fpl file
    uint32_t trackCount = *(uint32_t*) &m_fileBuffer[metaStop];

#if defined(_DEBUG) || FPL_DEBUG == 1
    int debugEntryCounter = 0;
#endif

    auto it = m_fileBuffer.begin() + metaStop + 4;
    while (it != m_fileBuffer.end())
    {
        MediaEntry mediaEntry;

        //XXX: alignment of struct for media track information
        /* 
            Due to the compiler adding some padding in the struct one can not
            just memcpy the whole data from the buffer into the struct.
        */
        memcpy(&mediaEntry.m_mediaTrackInfo.flags, &(*it), 4);
        it += 4;
        memcpy(&mediaEntry.m_mediaTrackInfo.fileNameOffset, &(*it), 4);
        it += 4;
        memcpy(&mediaEntry.m_mediaTrackInfo.subsongIndex, &(*it), 4);
        it += 4;
        
        if (CHECK_BIT(mediaEntry.m_mediaTrackInfo.flags, 0x1))
        {
            memcpy(&mediaEntry.m_mediaTrackInfo.fileSize, &(*it), 8);
            it += 8;
            memcpy(&mediaEntry.m_mediaTrackInfo.fileTime, &(*it), 8);
            it += 8;
            memcpy(&mediaEntry.m_mediaTrackInfo.duration, &(*it), 8);
            it += 8;
            memcpy(&mediaEntry.m_mediaTrackInfo.rpgAlbum, &(*it), 4);
            it += 4;
            memcpy(&mediaEntry.m_mediaTrackInfo.rpgTrack, &(*it), 4);
            it += 4;
            memcpy(&mediaEntry.m_mediaTrackInfo.rpkAlbum, &(*it), 4);
            it += 4;
            memcpy(&mediaEntry.m_mediaTrackInfo.rpkTrack, &(*it), 4);
            it += 4;
            memcpy(&mediaEntry.m_mediaTrackInfo.entryCount, &(*it), 4);
            it += 4;

            MediaEntry::MediaTrackEntry entry;

            memcpy(&entry.primaryKeyCount, &(*it), 4);
            it += 4;
            memcpy(&entry.secondaryKeyCount, &(*it), 4);
            it += 4;
            memcpy(&entry.secondaryKeyOffset, &(*it), 4);
            it += 4;

            entry.primaryKeys = std::vector<uint32_t>(2 * entry.primaryKeyCount, 0);
            //XXX: interestingly this check isn't necessary with gcc compiler, MSVC crashes though
            if (entry.primaryKeyCount != 0)
            {
                memcpy(&entry.primaryKeys[0], &(*it), (4 * 2 * entry.primaryKeyCount));
                it += (4 * 2 * entry.primaryKeyCount);
            }
            
            // before known as unk0
            // number of primary values following
            memcpy(&entry.primaryValueCount, &(*it), 4);
            it += 4;

            //XXX: primaryKeys stupid counting system
            /* 
                primary keys are stored as pairs consisting of an increasing index and
                    an additional offset value for the actual meta key value stored inside
                    the meta data.
                In case of a non-continuous indexing, the corresponding value for the key should
                    be duplicated inside the primaryKeys section.

                The last main key index is at primaryKeyCount * 2 - 2.
                To account for a different starting key other than zero, the
                    starting key is subtracted.
                Therefore the count of all keys inside primaryKeys should be:
                    theoreticalKeyCount = lastKeyIndex + 1 - firstKeyIndex
                
                The difference to the actual key count shows any missing numbers.
                    missingCounts = theoreticalKeyCount - primaryKeyCount

                In every cases the theoreticalKeyCount needs to be copied for the primaryKeyValues.
                This gives the ammount of uint32 that need to be read from the primaryKeys section.
             */
            if (entry.primaryKeyCount > 0)
            {
                entry.primaryValues = std::vector<uint32_t>(entry.primaryValueCount, 0);
                memcpy(&entry.primaryValues[0], &(*it), (4 * entry.primaryValueCount));
                it += (4 * entry.primaryValueCount);

            }

            entry.secondaryKeys = std::vector<uint32_t>(2 * entry.secondaryKeyCount, 0);
            //XXX: same as primaryKeys and primaryKeyCount
            if (entry.secondaryKeyCount != 0)
            {
                memcpy(&entry.secondaryKeys[0], &(*it), (4 * 2 * entry.secondaryKeyCount));
                it += (4 * 2 * entry.secondaryKeyCount);

            }

            mediaEntry.m_mediaTrackInfo.entries = entry;

        }
        
        if (CHECK_BIT(mediaEntry.m_mediaTrackInfo.flags, 0x4))
        {
            LOG_DEBUG("Padding inserted");
            mediaEntry.m_mediaTrackInfo.hasPadding = true;
            it += 64;
        }

        m_playlistEntries.push_back(mediaEntry);

#if defined(_DEBUG) || (FPL_DEBUG == 1)
        debugEntryCounter++;
        LOG_DEBUG("Entry num:\t %d", debugEntryCounter);
#endif

    }

    int count = 0;
    for (MediaEntry& mediaEntry : m_playlistEntries)
    {
        std::string entrydef = std::string(&m_fileBuffer[FPL_META_START + mediaEntry.m_mediaTrackInfo.fileNameOffset]);
        
        std::size_t identPos = entrydef.find("://");

        std::string identifier = entrydef.substr(0, identPos);
        std::string entryname = entrydef.substr(identPos + 3);

        LOG_DEBUG("Num: %d,\t Identifier: %s", count, identifier.c_str());
        std::filesystem::path test;

        if (identifier == "file")
        {
            mediaEntry.m_mediaType = MediaEntry::MediaType::AUDIO_FILE;
            mediaEntry.SetLocation(entryname);

            LOG_DEBUG("File added\n\t%s", reinterpret_cast<const char*>(mediaEntry.GetLocation().c_str()));

        } else if (identifier == "http")
        {
            mediaEntry.m_mediaType = MediaEntry::MediaType::AUDIO_STREAM;
            mediaEntry.SetLocation(entryname);

            LOG_DEBUG("Stream added\n\t%s", reinterpret_cast<const char*>(mediaEntry.GetLocation().c_str()));

        } else
        {
            LOG_ERROR(".fpl entry does not exist! - <%s>", identifier.c_str());

        }

        count++;

    }

}

void FoobarPlaylist::regexOnPath(const std::regex& deleteReg, const std::string& appendStr)
{
    for (auto pathIt = m_playlistEntries.begin(); pathIt != m_playlistEntries.end(); pathIt++)
    {
        std::filesystem::path curPath((*pathIt).GetLocation());
        //HACK: regex and UTF-8
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

        (*pathIt).SetLocation(strPath);

    }

}

void FoobarPlaylist::checkExisting(bool removeDeads)
{
    std::vector<MediaEntry> existingEntries;
    
    for (auto& plEntry : m_playlistEntries)
    {
        // URL entries (e.g. streams) are assumed to be always existing
        if (plEntry.m_mediaType == MediaEntry::MediaType::AUDIO_STREAM)
        {
            existingEntries.push_back(plEntry);
            continue;

        }

        std::filesystem::path filepath(plEntry.GetLocation());

        if (!(std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath)))
        {
            // printing char8_t works (but Windows console won't display it)
            LOG_INFO("%s - File not found!", reinterpret_cast<const char*>(filepath.u8string().c_str()));

        } else {
            existingEntries.push_back(plEntry);

        }

    }

    if (removeDeads)
    {
        m_playlistEntries = existingEntries;

    }

}

void FoobarPlaylist::printEntrynames()
{
    for (auto& plEntry : m_playlistEntries)
    {
        // printing char8_t works (but Windows console won't display it)
        LOG_OUTPUT("%s\n", reinterpret_cast<const char*>(plEntry.GetLocation().c_str()));

    }

}

void FoobarPlaylist::printCustomMode(CommandQueue::CommandOutputMode outputMode)
{
    switch (outputMode)
    {
    case CommandQueue::CommandOutputMode::M3U:
    {
        for (auto& plEntry : m_playlistEntries)
        {
            // printing char8_t works (but Windows console won't display it)
            LOG_OUTPUT("%s\n", reinterpret_cast<const char*>(plEntry.GetLocation().c_str()));

        }

        break;
    }

    case CommandQueue::CommandOutputMode::EXTM3U:
    {
        LOG_OUTPUT("#EXTM3U\n");

        for (auto& plEntry : m_playlistEntries)
        {
            LOG_OUTPUT("#EXTINF:%.0f",
                plEntry.GetDurationSeconds());

            std::vector<std::string> plAuthors(plEntry.FindPrimaryKey(m_fileBuffer, "artist"));
            std::vector<std::string> plTitles(plEntry.FindPrimaryKey(m_fileBuffer, "title"));

            if (plAuthors.size() > 0)
            {
                LOG_OUTPUT(",");

                for (int i = 0; i < plAuthors.size(); i++)
                {
                    LOG_OUTPUT("%s", plAuthors[i].c_str());

                    if (i < (plAuthors.size() - 1))
                    {
                        LOG_OUTPUT(" & ");

                    }

                }

                LOG_OUTPUT(" - ");

            }

            if (plTitles.size() > 0)
            {
                for (int i = 0; i < plTitles.size(); i++)
                {
                    LOG_OUTPUT("%s", plTitles[i].c_str());

                    if (i < (plTitles.size() - 1))
                    {
                        LOG_OUTPUT(" & ");

                    }

                }

            }

            LOG_OUTPUT("\n");

            // printing char8_t works (but Windows console won't display it)
            LOG_OUTPUT("%s\n", reinterpret_cast<const char*>(plEntry.GetLocation().c_str()));

        }

        break;

    }
    
    case CommandQueue::CommandOutputMode::NONE:
    {
        LOG_INFO("Wrong input mode given! Outputting should not continue!");

        for (auto& plEntry : m_playlistEntries)
        {
            // printing char8_t works (but Windows console won't display it)
            LOG_OUTPUT("%s\n", reinterpret_cast<const char*>(plEntry.GetLocation().c_str()));

        }

        break;
    }

    default:
        break;

    }

}

std::string FoobarPlaylist::getStringFromBuf(uint64_t offset) const
{
    if (offset >= m_fileBuffer.size())
    {
        LOG_ERROR("Offset in fpl file buffer out of bounds!");
        return std::string("");

    }

    std::string strInBuf((m_fileBuffer.cbegin() + offset), std::find((m_fileBuffer.cbegin() + offset), m_fileBuffer.cend(), '\0'));

    return strInBuf;

}
}

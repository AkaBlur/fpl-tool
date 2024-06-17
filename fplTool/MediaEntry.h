#pragma once

#include <filesystem>
#include <memory>
#include <vector>

namespace FPLTool
{
class FoobarPlaylist;

class MediaEntry
{
public:
    enum class MediaType
    {
        AUDIO_FILE   = 0,
        AUDIO_STREAM = 1,

        NOT_SET      = 100

    };

    struct MediaTrackEntry
    {
        uint32_t primaryKeyCount;
        uint32_t primaryValueCount;
        uint32_t secondaryKeyCount;
        uint32_t secondaryKeyOffset;
        
        std::vector<uint32_t> primaryKeys;
        std::vector<uint32_t> primaryValues;
        std::vector<uint32_t> secondaryKeys;

    };

    struct MediaTrack
    {
        uint32_t flags;
        uint32_t fileNameOffset;
        uint32_t subsongIndex;
        uint64_t fileSize;
        uint64_t fileTime;
        double duration;
        float rpgAlbum;
        float rpgTrack;
        float rpkAlbum;
        float rpkTrack;
        uint32_t entryCount;

        MediaTrackEntry entries;
        bool hasPadding;

    };

    MediaEntry() : m_mediaType(MediaType::NOT_SET) {};

    // set location of the playlist entry, either filepath or URL
    void SetLocation(const std::string& entryLocation);
    // get location as c_str, can be encoded as UTF-8 char string
    // standard OS-specific location (aware of slash and backslash)
    std::u8string const GetLocation();
    // will always return the original .fpl location entry (only with backslash)
    std::u8string const GetFPLEntryLocation();

    // return track duration in seconds
    double GetDurationSeconds() { return this->m_mediaTrackInfo.duration; };

    // tries to find a value for the given key inside the given file buffer for the given entry
    // may return multiple values for a single key
    std::vector<std::string> FindPrimaryKey(const std::vector<char>& filebuffer, const std::string& key);

    MediaTrack m_mediaTrackInfo;
    MediaType m_mediaType;

private:
    std::filesystem::path m_filepath;
    std::string m_fileURL;

};
}
#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

namespace FPLTool
{
    class CommandQueue
    {
    public:
        CommandQueue(int argc, char* argv[]);

        bool parseCommands();
        void playCommands();

        enum class CommandOutputMode
        {
            NONE = 0,
            M3U = 1,
            EXTM3U = 2
        };

    private:
        struct CommandSettings
        {
            CommandSettings() : 
                checkFileExistence(false), stringAppendFiles(""), isRegex(false), isCustomOutput(false), outputMode(CommandOutputMode::NONE) {};

            // NECESSARY
            std::filesystem::path inputFile;

            bool checkFileExistence;

            // regex for filepath manipulation
            std::regex regexDeleteFiles;
            bool isRegex;
            std::string stringAppendFiles;

            bool isCustomOutput;
            CommandOutputMode outputMode;

        };

        uint32_t m_CmdCount;
        std::vector<std::string> m_inputCommands;

        CommandSettings m_CmdSettings;

        CommandOutputMode m_parseOutputModeString(const std::string& outputModeRef);

    };

}
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

    private:
        struct CommandSettings
        {
            CommandSettings() : checkFileExistence(false), stringAppendFiles("") {};

            // NECESSARY
            std::filesystem::path inputFile;

            bool checkFileExistence;

            // regex for filepath manipulation
            std::regex regexDeleteFiles;
            bool isRegex;
            std::string stringAppendFiles;

        };

        uint32_t m_CmdCount;
        std::vector<std::string> m_inputCommands;

        CommandSettings m_CmdSettings;

    };

}
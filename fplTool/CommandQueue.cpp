#include "CommandQueue.h"
#include "FoobarPlaylist.h"
#include "Log.h"

namespace FPLTool
{
    CommandQueue::CommandQueue(int argc, char* argv[])
    {
        for (int i = 1; i < (argc); i++)
        {
            m_inputCommands.emplace_back(argv[i]);
            LOG_DEBUG("Command: %s", argv[i]);

        }

        m_CmdCount = argc;

    }

    bool CommandQueue::parseCommands()
    {
        if (m_inputCommands.size() == 0)
        {
            PRINT_USAGE;
            return false;

        }

        // input file - fpl
        std::filesystem::path inputFile(m_inputCommands.back());

        if (!(std::filesystem::exists(inputFile) && std::filesystem::is_regular_file(inputFile)))
        {
            LOG_ERROR("Given input file '%s' does not exist!", inputFile.string().c_str());

            return false;

        } else {
            LOG_DEBUG("Setting input file: '%s'", inputFile.string().c_str());

            m_CmdSettings.inputFile = inputFile;

        }

        // no further commands
        if (m_CmdCount <= 2)
        {
            return true;
        }

        for (auto cmdIter = m_inputCommands.cbegin(); cmdIter != (m_inputCommands.cend() - 1); cmdIter++)
        {
            std::string strCmd = (*(cmdIter));
            char currentCmd;

            if (strCmd.size() >= 2)
            {
                currentCmd = strCmd[1];

            } else {
                LOG_ERROR("Command not recognized! <%s>", strCmd.c_str());
                return false;

            }

            switch (currentCmd)
            {
                // check file existence
                case 'e':
                    m_CmdSettings.checkFileExistence = true;
                    LOG_DEBUG("Checking for file existence");

                    break;

                // regex expressions
                case 'r':
                    {
                        if (strCmd.find(':') == std::string::npos)
                        {
                            LOG_ERROR("Wrong use of regex command! Usage is -r:d (and -r:a)!");
                            return false;

                        }

                        std::string regexCmd = strCmd.substr((strCmd.find(':') + 1));

                        if (regexCmd == "d")
                        {
                            cmdIter++;

                            if (cmdIter == (m_inputCommands.cend() - 1))
                            {
                                LOG_ERROR("Regex for input file not given! -r:d <REGEX>");
                                return false;
                                
                            }

                            m_CmdSettings.regexDeleteFiles = std::regex(*(cmdIter));
                            LOG_DEBUG("Input file regex set: %s", (*(cmdIter)).c_str());

                            m_CmdSettings.isRegex = true;

                        } else if (regexCmd == "a") {
                            cmdIter++;

                            if (cmdIter == (m_inputCommands.cend() - 1))
                            {
                                LOG_ERROR("String for file regex replacement not given! -r:a <STR>");
                                return false;
                                
                            }

                            m_CmdSettings.stringAppendFiles = std::string(*(cmdIter));
                            LOG_DEBUG("Regex replacement for file set: %s", (*(cmdIter)).c_str());

                        } else {
                            LOG_ERROR("Wrong use of regex command! Usage is -r:d (and -r:a)!");
                            return false;

                        }

                        break;

                    }
                
                default:
                    LOG_ERROR("Command not recognized! <%c>", currentCmd);
                    return false;

                    break;

            }

        }

        return true;

    }

    void CommandQueue::playCommands()
    {
        if (!(std::filesystem::exists(m_CmdSettings.inputFile) &&
            (std::filesystem::is_regular_file(m_CmdSettings.inputFile))))
        {
            LOG_ERROR("Commands not setup!");
            return;

        }

        LOG_DEBUG("Playing command set");
        FoobarPlaylist currentPlaylist(m_CmdSettings.inputFile);

        LOG_DEBUG("Parsing fpl");
        currentPlaylist.parseFPL();

        if (m_CmdSettings.isRegex)
        {
            LOG_DEBUG("Applying regex to files");

            currentPlaylist.regexOnPath(m_CmdSettings.regexDeleteFiles, m_CmdSettings.stringAppendFiles);

        }

        if (m_CmdSettings.checkFileExistence)
        {
            LOG_DEBUG("Checking for existence");

            currentPlaylist.checkExisting(true);

        }

        currentPlaylist.printFiles();

    }

}
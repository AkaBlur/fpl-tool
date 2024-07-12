#include "CommandQueue.h"
#include "Log.h"

int main(int argc, char* argv[])
{
    FPLTool::CommandQueue queue(argc, argv);
    if (!queue.parseCommands())
        return 0;

    queue.playCommands();

    return 0;

}
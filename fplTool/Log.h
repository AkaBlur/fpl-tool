#pragma once

#include <iostream>

#if DEBUG==1
#define LOG_DEBUG(x, ...)  fprintf(stderr, "[DEBUG] - " x "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(x, ...)
#endif

#define LOG_INFO(x, ...)   fprintf(stderr, "[INFO] - " x "\n", ##__VA_ARGS__)
#define LOG_ERROR(x, ...)  fprintf(stderr, "[ERROR] - " x "\n", ##__VA_ARGS__)

#define LOG_OUTPUT(x, ...) fprintf(stdout, "" x "\n", ##__VA_ARGS__)

#define PRINT_USAGE fprintf(stdout, "foobar2000 Playlist Tool - extract information from .fpl files\n\
Version:\t %s\n\
Build-Date:\t %s\n\
\n\
Usage:\n\
\tfplTool [OPTIONS] FILENAME\n\
\n\
FILENAME - input foobar2000 playlist file (.fpl)\n\
\n\
OPTIONS:\n\
\t-e\t\tEnable file checking, will remove all non-existing files from output\n\
\t\t\tIs executed after all regex options are applied!\n\n\
\t-r:d <REGEX>\tRegex, used for input formatting;\n\
\t\t\tWill remove any match of REGEX in playlist filepath entries\n\
\t-r:a <STRING>\tString, only in combination with -r:d used!\n\
\t\t\tWill replace the removed filepath parts from -r:d with STRING\n\
\t\t\tSTRING is formatted after ECMAscript standard for String.prototype.replace\n\
", FPL_VERSION, BUILD_DATE)

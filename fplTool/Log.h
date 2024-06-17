#pragma once

#include <iostream>

#if defined _DEBUG || (FPL_DEBUG == 1)
#define LOG_DEBUG(x, ...)  fprintf(stderr, "[DEBUG] - " x "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(x, ...) (void)0;
#endif

#define CUSTOM_OUTPUT_MODES "\t\t\tM3U\t - Normal M3U\n\
\t\t\tEXTM3U\t - Extended M3U format"

#define LOG_INFO(x, ...)   fprintf(stderr, "[INFO] - " x "\n", ##__VA_ARGS__)
#define LOG_ERROR(x, ...)  fprintf(stderr, "[ERROR] - " x "\n", ##__VA_ARGS__)

#define LOG_OUTPUT(x, ...) fprintf(stdout, "" x, ##__VA_ARGS__)

#define PRINT_USAGE fprintf(stdout, "foobar2000 Playlist Tool - extract information from .fpl files\n\
Version:\t %s\n\
Build-Date:\t %s\n\
Platform:\t %s\n\
\n\
Usage:\n\
\tfplTool [OPTIONS] FILENAME\n\
\n\
FILENAME - input foobar2000 playlist file (.fpl)\n\
\n\
OPTIONS:\n\
\t-e\t\t Enable file checking, will remove all non-existing files from output\n\
\t\t\tIs executed after all regex options are applied!\n\n\
\t-r:d <REGEX>\t Regex, used for input formatting;\n\
\t\t\tWill remove any match of REGEX in playlist filepath entries\n\
\t-r:a <STRING>\t String, only in combination with -r:d used!\n\
\t\t\tWill replace the removed filepath parts from -r:d with STRING\n\
\t\t\tSTRING is formatted after ECMAscript standard for String.prototype.replace\n\
\t-o <OUTPUT-MODE>\t Changes the default output formatting\n\
\t\tOUTPUT-MODE: Available Modes\n\
%s\n\
", FPL_VERSION, BUILD_DATE, FPL_BUILD_PLATFORM, CUSTOM_OUTPUT_MODES);

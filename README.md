# fplTool

🐱 **A Tool for parsing foobar2000 Playlist files** 🐱

## Features
* Command-line tool
* File input: **.fpl**
* Check for Missing Files with **-e**
* In-operation Replacement in Filepaths (manipulate paths from files) with regex options **-r:a** & **-r:d**
* Custom Output Options with **-o**

This program is OS-aware when compiled. <br>
Therefore the appropriate path separator will be used. <br>
(`\` on Windows and `/` on UNIX-like systems)


## How-To-Use

The basic principle for this tool is to read in a foobar2000 playlist file and print all containing files to the output.<br>
Inbetween input and output there are several options that can be used for parsing and manipulation.<br><br>
For example you can use this tool to create a simple `.m3u` playlist file.
```bash
fpltool myplaylist.fpl > myM3Uplaylist.m3u
```
---
<br>

**Basic usage:**

```
fplTool myplaylist.fpl
```

This will read in all playlist files from the given `myplaylist.fpl` playlist and print them as output.

---
<br>

**Check for missing files:**

Enable with option `-e` <br>
When this option is supplied all files are checked whether they exist or not. <br>
Non-existing files are printed to `stderr`. <br>
With this option only files that exist are output.

---
<br>

**Apply patterns to filepaths**

This options allows the user to supply a pattern that is used when filepaths are read in.<br>
👆 **These options will be applied before file-check with `-e`** 👆 <br><br>
There are two options you can use:

```bash
fplTool -r:d <REGEX>
```

or

```bash
fplTool -r:d <REGEX> -r:a <STRING>
```

---
<br>

**Use regex for deletion** - `r:d <REGEX>`

When supplying this option the regular expression `REGEX` is applied to all filepaths from the playlist. Portions that get matched **will be removed** from the path. <br>
This option is intended to be used when changing all paths from all playlist files. <br>

> [!IMPORTANT]
> REGEX can only support basic **8-bit chars**! (nothing above U+00FF)

**Example** <br>
Remove drive letters from files

General playlist:

> `fplTool myplaylist.fpl` <br><br>
> Q:/music/1.mp3 <br>
> Q:/music/2.mp3 <br>
> Q:/music/3.mp3 <br>
> Q:/music/4.mp3 <br>
> Q:/music/5.mp3 <br>

Removing all drive letters:

> `fplTool -r:d "Q:/" myplaylist.fpl` <br><br>
> music/1.mp3 <br>
> music/2.mp3 <br>
> music/3.mp3 <br>
> music/4.mp3 <br>
> music/5.mp3 <br>

---
<br>

**Use regex for replacement** - `r:d <REGEX>` and `r:a <STRING>`

When supplying both options the first operation will still remove the matched `<REGEX>` string. <br>
Following the deletion, all matched portions inside the filepaths will be replaced with `<STRING>`. <br>
This string can be anything, but the format here is defined in the ECMAscript standard 262. This can be used to reference the removed portions of the path in the given string. <br><br>
For a detailed overview check:
* [regex_replace](https://en.cppreference.com/w/cpp/regex/regex_replace), used here
* [ECMAscript syntax](https://262.ecma-international.org/5.1/#sec-15.5.4.11)

<br>

**Example** <br>
Substiute filepaths (f.e. if you moved your library):

General playlist:

> `fplTool myplaylist.fpl` <br><br>
> Q:/music/1.mp3 <br>
> Q:/music/2.mp3 <br>
> Q:/music/3.mp3 <br>
> Q:/music/4.mp3 <br>

Substitute to accomodate new paths:

> `fplTool -r:d "/music/" -r:a "/music/newmix/" myplaylist.fpl` <br><br>
> Q:/music/newmix/1.mp3 <br>
> Q:/music/newmix/2.mp3 <br>
> Q:/music/newmix/3.mp3 <br>
> Q:/music/newmix/4.mp3 <br>
> Q:/music/newmix/5.mp3 <br>

or if moved to a different disk:

> `fplTool -r:d "Q:" -r:a "Z:" myplaylist.fpl` <br><br>
> Z:/music/1.mp3 <br>
> Z:/music/2.mp3 <br>
> Z:/music/3.mp3 <br>
> Z:/music/4.mp3 <br>
> Z:/music/5.mp3 <br>

---
<br>

**Custom Output Options** - `-o <OPTION>`

This option will produce an output that is formatted to a specific spec.

**Currently implemented formats:**
- M3U
- EXTM3U (extended M3U)

These options currently only output text-based playlists. They can be used to pipe the output to a file.

## Building

**Prerequisites**

* cmake (> 3.12)
* a C++ toolchain (tested with gcc 11.4)

---
<br>

To build from source, first clone the repository. <br>
Then build like an usual `cmake` project.

```bash
mkdir build
cmake -S . -B build
cmake --build build
```

If you want to build in debug mode, set the option: `BUILD_DEBUG=1`

```bash
mkdir build
cmake -S . -B build -DBUILD_DEBUG=1
cmake --build build
```

## Installation

Just invoke cmake with install but select the target **build directory**.

> [!IMPORTANT]
> This may require **sudo** or **system administrator** privileges!

```bash
# if build with: cmake -S . -B build
cmake --install build
```

## Development

Currently I'm working on this project to give a reliable option from transitioning from foobar2000 to VLC (and to Linux as there is no native foobar2000 version 🙃). For this I tend to use this as an option to save me some hassle and rebuild my complete playlist from scratch. <br>
Therefore probably some features will be also implemented that accomodate updating existing playlist files or stuff like that. <br><br>
At the moment the most limiting factor (besides time) is the distinct lack of information about the format of the .fpl-file format. I reverse-engineered some portions and I also found quite some forum posts about that format, but somehow nobody has a clear solution.
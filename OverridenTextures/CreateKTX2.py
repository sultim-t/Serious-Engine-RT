# Copyright (c) 2021 Sultim Tsyrendashiev
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import sys
import os
import subprocess
import pathlib

DEFAULT_INPUT_FOLDER_NAME = "Raw"
DEFAULT_OUTPUT_FOLDER_NAME = "Compressed"

INPUT_EXTENSIONS = [".png"]
OUTPUT_EXTENSION = ".ktx2"

CACHE_FILE_NAME = "CreateKTX2Cache.txt"


def printInPowerShell(msg, color):
    p = subprocess.run([
        "PowerShell",
        "Write-Host",
        "\"" + msg + "\"",
        "-ForegroundColor", color
    ])


def main():
    if "--help" in sys.argv or "--h" in sys.argv or "-help" in sys.argv or "-h" in sys.argv:
        print("Usage: CreateKTX2.py")
        print("")
        print("  CreateKTX2 compresses PNG files from input folder to KTX2 files with")
        print("  BC7 format to output folder. Folders of the files are preserved the")
        print("  same as in the input folder.")
        print("")
        print("  Requires compressonatorcli:")
        print("  https://github.com/GPUOpen-Tools/compressonator")
        return

    if not os.path.exists(CACHE_FILE_NAME):
        try:
            with open(CACHE_FILE_NAME, "w"):
                pass
        except OSError:
            print("> Coudn't create cache file")
            return
    with open(CACHE_FILE_NAME, "r+") as cacheFile:
        cache = {}

        try:
            name = None
            # odd line  - filename
            # even line - st_mtime
            for line in cacheFile.read().splitlines():
                if name is None:
                    name = line
                else:
                    cache[name] = int(line)
                    name = None
        except:
            cache = {}

    for currentPath, folders, files in os.walk(DEFAULT_INPUT_FOLDER_NAME):
        for file in files:
            fullRelativeFilename = os.path.join(currentPath, file)

            # remove DEFAULT_INPUT_FOLDER_NAME from filename
            filename = os.path.relpath(fullRelativeFilename, DEFAULT_INPUT_FOLDER_NAME)

            pathNoExt, ext = os.path.splitext(filename)
            isImg = ext in INPUT_EXTENSIONS

            if not isImg:
                continue

            isNew = filename not in cache

            lastModifTime = int(pathlib.Path(fullRelativeFilename).stat().st_mtime)
            isOutdated = filename in cache and lastModifTime != cache[filename]

            if isNew or isOutdated:
                print("> Converting " + filename)

                inputFile = os.path.join(DEFAULT_INPUT_FOLDER_NAME, filename)
                outputFile = os.path.join(DEFAULT_OUTPUT_FOLDER_NAME, pathNoExt + OUTPUT_EXTENSION)

                # create folder
                pathlib.Path(os.path.dirname(outputFile)).mkdir(parents=True, exist_ok=True)

                r = subprocess.run([
                    "compressonatorcli.exe",
                    "-fd", "BC7",
                    "-miplevels", "20",
                    "-EncodeWith", "GPU",
                    inputFile,
                    outputFile],
                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

                # if success, save to cache
                if "Done Processing" in r.stdout:
                    cache[filename] = lastModifTime
                else:
                    print(r.stdout)

    with open(CACHE_FILE_NAME, "w") as cacheFile:
        for name, tm in cache.items():
            cacheFile.write(name + "\n" + str(tm) + "\n")


if __name__ == '__main__':
    main()

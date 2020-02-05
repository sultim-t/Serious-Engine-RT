""" Copyright (c) 2020 Sultim Tsyrendashiev
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. """

import sys

def main(argv):
  if len(argv) != 3:
    return

  shaderFilePath = argv[1]
  shaderName = argv[2]

  with open(shaderFilePath, 'rb') as spirvFile:
    print('#pragma once\n')
    print('unsigned char ' + shaderName + '_Spirv[] = {')

    byteCount = 0

    byte = spirvFile.read(1)
    while byte:
      
      byteCount += 1

      lineEnd = '' if byteCount % 12 else '\n'
      print('0x' + byte.hex().upper() + ', ', end = lineEnd)

      byte = spirvFile.read(1)

    print('};')
    print('unsigned int ' + shaderName + '_Size = ' + str(byteCount) + ';')

main(sys.argv)
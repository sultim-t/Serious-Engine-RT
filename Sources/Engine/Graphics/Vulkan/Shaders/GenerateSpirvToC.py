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
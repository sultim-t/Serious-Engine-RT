
if not exist %cd%\Compiled mkdir Compiled
if not exist %cd%\CompiledSpirv mkdir CompiledSpirv

for %%f in (*.vert) do (
	glslangValidator.exe -V %%f -o CompiledSpirv/%%~nfVert.spv
	GenerateSpirvToC.py CompiledSpirv/%%~nfVert.spv %%~nfVert > Compiled/%%~nfVertSpv.h
)

for %%f in (*.frag) do (
	glslangValidator.exe -V %%f -o CompiledSpirv/%%~nfFrag.spv
	GenerateSpirvToC.py CompiledSpirv/%%~nfFrag.spv %%~nfFrag > Compiled/%%~nfFragSpv.h
)

for %%f in (*.comp) do (
	glslangValidator.exe -V %%f -o CompiledSpirv/%%~nfComp.spv
	GenerateSpirvToC.py CompiledSpirv/%%~nfComp.spv %%~nfComp > Compiled/%%~nfCompSpv.h
)
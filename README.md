# About
Basic NES emulator. Passes Blargg's CPU test. Can run some games without mapper (MMC1 is present, but not properly implemented). CPU/PPU timing is wrong. APU is not implemented.

## Requirements 
Repository contains two projects. Emulator Core - relies only on standard library. Emulator App - uses GLFW3 and GLEW to render 'Emulator Core' output.
GLFW3 and GLEW are expected to be installed on your system separately.

## License
- UNLICENSE for this repository (see `UNLICENSE.txt` for more details)
- Premake is licensed under BSD 3-Clause (see included LICENSE.txt file for more details)
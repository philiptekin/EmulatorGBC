# EmulatorGBC

## How to build

### Build Release
cmake --workflow release

### Build Debug
cmake --workflow debug

## How to run

There will be a build folder after doing the previous step. The release is in the build/release folder and debug is in build/debug folder.

Just do

./build/release/EmulatorGBC

./build/debug/EmulatorGBC

## How to run tests

### Release build tests
ctest --preset release

### Debug build tests
ctest --preset debug

## How to run formatting and apply rules

### Pre-commit
This repo uses pre-commit with clang-format, so use

pre-commit install

pre-commit run -a


## How to run clang-tidy

Use the clang tidy vscode extension and you might have to disable microsoft c++ extension as they collide

Create a symlink of compile_commands.json of the build you mainly use, otherwise there will be a lot of innacurate errors in the IDE:

For debug:
ln -s build/debug/compile_commands.json compile_commands.json

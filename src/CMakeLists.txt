# CMakeLists.txt --- CMake file for 16cdb
# Copyright (c) Joe Jevnik 2014

set(16CDB_FILES debug.c
                machine/operations.c
                machine/memory.c
                commands.c
                processor.c)

set(EXECUTABLE_OUTPUT_PATH ${16CANDLES_BINARY_DIR})

add_executable(16cdb ${16CDB_FILES})
target_link_libraries(16cdb readline pthread readline)

# cmake -B build . && cmake --build build -j
cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(SaveManager)

# For whatever reason, the C++ stdlib is broken on my Mac.
#
#   % echo "#include <vector>" | c++ -x c++ -
#   <stdin>:1:10: fatal error: 'vector' file not found
#       1 | #include <vector>
#         |          ^~~~~~~~
#   1 error generated.
if(APPLE)
  set(CMAKE_CXX_FLAGS "-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1")
endif()

include(FetchContent)
FetchContent_Declare(
  minizip
  GIT_REPOSITORY https://github.com/zlib-ng/minizip-ng.git
  GIT_TAG 4.0.10
)

set(MZ_COMPAT OFF)
set(MZ_ZLIB ON)
set(MZ_ZLIB_FLAVOR auto)
set(MZ_BZIP2 OFF)
set(MZ_LZMA OFF)
set(MZ_ZSTD OFF)
set(MZ_LIBCOMP OFF)
set(MZ_FETCH_LIBS WIN32)
set(MZ_FORCE_FETCH_LIBS OFF)
set(MZ_PKCRYPT OFF)
set(MZ_WZAES OFF)
set(MZ_OPENSSL OFF)
set(MZ_LIBBSD OFF)
set(MZ_ICONV ON)
set(MZ_COMPRESS_ONLY OFF)
set(MZ_DECOMPRESS_ONLY OFF)
set(MZ_FILE32_API OFF)
set(MZ_BUILD_TESTS OFF)
set(MZ_BUILD_UNIT_TESTS OFF)
set(MZ_BUILD_FUZZ_TESTS OFF)
set(MZ_CODE_COVERAGE OFF)

FetchContent_MakeAvailable(minizip)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
qt_standard_project_setup()

set(SOURCE_FILES
    src/main_window.hpp
    src/main_window.cpp
    src/data.hpp
    src/data.cpp
    src/app.hpp
    src/app.cpp
    src/utils.hpp
    src/zip.hpp
    src/zip.cpp
)

set(LIBS
  MINIZIP::minizip
  Qt6::Widgets
  Qt6::Sql
)

set(INCLUDES
  ${minizip_SOURCE_DIR}
)

qt_add_executable(hello ${SOURCE_FILES} src/main.cpp)
set_property(TARGET hello PROPERTY CXX_STANDARD 23)
set_property(TARGET hello PROPERTY COMPILE_WARNING_AS_ERROR TRUE)
target_link_libraries(hello PRIVATE ${LIBS})
target_include_directories(hello PRIVATE ${INCLUDES})

if(WIN32)
  set_target_properties(hello PROPERTIES WIN32_EXECUTABLE ON)
  add_custom_command(TARGET hello POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Qt6_ROOT}/plugins/platforms/qwindows.dll
    $<TARGET_FILE_DIR:hello>/platforms/qwindows.dll

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Qt6_ROOT}/bin/Qt6Core.dll
    ${Qt6_ROOT}/bin/Qt6Gui.dll
    ${Qt6_ROOT}/bin/Qt6Sql.dll
    ${Qt6_ROOT}/bin/Qt6Widgets.dll
    $<TARGET_FILE_DIR:hello>
  )
endif()

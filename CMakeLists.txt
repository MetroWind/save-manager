# Build instruction for Windows:
#
# cmake -B build . -DQt6_ROOT=D:/Qt/6.9.1/msvc2022_64
# cmake --build build --config Release -j
# cmake --install build --config Release --prefix build
#
# Run in build/dist

cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(SimpleSaveManager)

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
FetchContent_Declare(
  ryml
  GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
  GIT_TAG v0.9.0
  GIT_SHALLOW FALSE  # ensure submodules are checked out
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

FetchContent_MakeAvailable(minizip ryml)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
qt_standard_project_setup()

set(SOURCE_FILES
    src/app.cpp
    src/app.hpp
    src/data.cpp
    src/data.hpp
    src/game.cpp
    src/game.hpp
    src/glob.hpp
    src/main_window.cpp
    src/main_window.hpp
    src/models.cpp
    src/models.hpp
    src/save_store.cpp
    src/save_store.hpp
    src/utils.cpp
    src/utils.hpp
    src/zip.cpp
    src/zip.hpp
)

set(LIBS
  MINIZIP::minizip
  Qt6::Widgets
  Qt6::Sql
  ryml::ryml
)

set(INCLUDES
  ${minizip_SOURCE_DIR}
)

qt_add_executable(save-manager ${SOURCE_FILES} src/main.cpp)
target_compile_definitions(save-manager PRIVATE "DEBUG=$<CONFIG:Debug>")
set_property(TARGET save-manager PROPERTY CXX_STANDARD 23)
set_property(TARGET save-manager PROPERTY COMPILE_WARNING_AS_ERROR TRUE)
target_link_libraries(save-manager PRIVATE ${LIBS})
target_include_directories(save-manager PRIVATE ${INCLUDES})

if(WIN32)
  set_target_properties(save-manager PROPERTIES WIN32_EXECUTABLE ON)
  add_custom_command(TARGET save-manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Qt6_ROOT}/plugins/platforms/qwindows.dll
    $<TARGET_FILE_DIR:save-manager>/platforms/qwindows.dll

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Qt6_ROOT}/plugins/sqldrivers/qsqlite.dll
    $<TARGET_FILE_DIR:save-manager>/sqldrivers/qsqlite.dll

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${Qt6_ROOT}/bin/Qt6Core.dll
    ${Qt6_ROOT}/bin/Qt6Gui.dll
    ${Qt6_ROOT}/bin/Qt6Sql.dll
    ${Qt6_ROOT}/bin/Qt6Widgets.dll
    ${zlib_BINARY_DIR}/Release/zlib-ng2.dll
    $<TARGET_FILE_DIR:save-manager>
  )

  install(TARGETS save-manager
    EXPORT InstallTargets
    DESTINATION dist
  )
  install(FILES
    $<TARGET_RUNTIME_DLLS:save-manager>
    ${CMAKE_SOURCE_DIR}/games.yaml
    DESTINATION dist)
  install(FILES ${Qt6_ROOT}/plugins/platforms/qwindows.dll
    DESTINATION dist/platforms)
  install(FILES ${Qt6_ROOT}/plugins/sqldrivers/qsqlite.dll
    DESTINATION dist/sqldrivers)
endif()

if(APPLE)
  set_target_properties(save-manager PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER rocks.mws.save-manager
    MACOSX_BUNDLE_BUNDLE_NAME save-manager
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
  )
endif()

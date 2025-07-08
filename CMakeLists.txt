# cmake -B build . && cmake --build build -j
cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(SaveManager)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
qt_standard_project_setup()

set(SOURCE_FILES
    src/main_window.hpp
    src/main_window.cpp
    src/data.hpp
    src/data.cpp
)

set(LIBS
  Qt6::Widgets
  Qt6::Sql
)

set(INCLUDES
  ${libmw_SOURCE_DIR}/includes
)

qt_add_executable(hello ${SOURCE_FILES} src/main.cpp)
set_property(TARGET hello PROPERTY CXX_STANDARD 23)
set_property(TARGET hello PROPERTY COMPILE_WARNING_AS_ERROR TRUE)
target_link_libraries(hello PRIVATE ${LIBS})
target_include_directories(shrt PRIVATE ${INCLUDES})
set_target_properties(hello PROPERTIES WIN32_EXECUTABLE ON)

add_custom_command(TARGET hello POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${Qt6_ROOT}/plugins/platforms/qwindows.dll
                       $<TARGET_FILE_DIR:hello>/platforms/qwindows.dll

                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${Qt6_ROOT}/bin/Qt6Core.dll
                       ${Qt6_ROOT}/bin/Qt6Gui.dll
                       ${Qt6_ROOT}/bin/Qt6Widgets.dll
                       $<TARGET_FILE_DIR:hello>
                   )

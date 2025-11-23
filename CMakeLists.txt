cmake_minimum_required(VERSION 3.22)
project(LibrarySoft C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-fms-extensions)
add_executable(LibrarySoft WIN32
        main.c
        lists.c
        objects.c
        formFunctions.c
        res/Librarysoft.rc)

if(MSVC)
    target_link_options(LibrarySoft PRIVATE /MANIFEST:NO)
endif()

add_custom_command(TARGET LibrarySoft POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LibrarySoft>/Data"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/DataSeed"
        "$<TARGET_FILE_DIR:LibrarySoft>/Data"
        VERBATIM
)
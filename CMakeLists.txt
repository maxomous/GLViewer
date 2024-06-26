
cmake_minimum_required(VERSION 2.8.12)

####################################################
#               Project Properties
####################################################

project("GLViewer"
    DESCRIPTION "An Simplified OpenGL 3.0 Interface using GLFW & ImGui"
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

####################################################
#          Include Source File Directories
####################################################

set(DIRS "")

####################################################
#           Create List of Source Files
####################################################

## Add Source Files from the main directory
file(GLOB SRC_FILES *.c *.cpp *.h *.hpp)
## Add Source Files from the other directories
foreach(DIR ${DIRS})
    # Find all source files & append to list
    file(GLOB SRC_FILES_IN_FOLDER ${DIR}/*.c ${DIR}/*.cpp ${DIR}/*.h ${DIR}/*.hpp)
    list(APPEND SRC_FILES ${SRC_FILES_IN_FOLDER})
endforeach()

## Show the files being added & put each file on it's own line
if(1)
    message("Compling Project '" ${PROJECT_NAME} "'\nSource Files:")
    foreach(SRC_FILE ${SRC_FILES}) 
        message(${SRC_FILE}) 
    endforeach()
endif()

####################################################
#                 Build Target
####################################################

## Build target with source files
add_executable(${PROJECT_NAME} ${SRC_FILES}) # add_library / add_executable

####################################################
#                Include Librarys
####################################################

## Include Library from cmakefile
add_subdirectory(glcore)

# link libraries
target_link_libraries(${PROJECT_NAME}
    GLCore
)

####################################################
#             Set Compiler Options
####################################################

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -g) 

####################################################

cmake_minimum_required(VERSION 2.8.9)
project(word2vec)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(MSVC)
 set(LIBS ${LIBS} pthreadVC2)
else()  
 set(LIBS ${LIBS} pthread)
endif()

add_subdirectory(src)
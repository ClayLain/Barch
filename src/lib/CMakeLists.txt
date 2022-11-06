set(headers
    archive.h
    bitmap.h
    byte_utils.h
)

set(source
    archive.cpp
    byte_utils.cpp
)

add_library(BarchLib STATIC ${source} ${headers})
target_include_directories(BarchLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

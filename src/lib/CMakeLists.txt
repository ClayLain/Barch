set(headers
    archive.h
    bitmap.h
)

set(source
    archive.cpp
)

add_library(BarchLib STATIC ${source} ${headers})
target_include_directories(BarchLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

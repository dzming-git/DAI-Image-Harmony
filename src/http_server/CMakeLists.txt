set(SOURCES
    http_server_builder.h
    http_server_builder.cpp
    http_server.h
    http_server.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(http_server ${SOURCES})

# opencv
find_package(OpenCV REQUIRED)
target_include_directories(http_server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(http_server PUBLIC
    ${OpenCV_LIBS}
)

# libhv
set(HV_LIBRARY /usr/local/lib/libhv.so)
target_link_libraries(http_server PUBLIC
    ${HV_LIBRARY}
)
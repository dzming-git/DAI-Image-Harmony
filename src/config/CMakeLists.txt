set(SOURCES
    config.h
    config.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")

# yaml-cpp
set(YAMLCPP_LIBRARY /usr/local/lib/libyaml-cpp.a)

add_library(config ${SOURCES})

target_include_directories(config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(config PUBLIC
    ${YAMLCPP_LIBRARY}
)

set(SOURCES
    webcamera_hikvision.h
    webcamera_hikvision.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(webcamera_hikvision ${SOURCES})

find_package(OpenCV REQUIRED)

# HCNetSDK
set(HCNetSDK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/HCNetSDK/include)
set(HCNetSDK_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/HCNetSDK/lib/)
file(GLOB HCNetSDK_LIBS "${HCNetSDK_LIB_DIR}/*.so*")
set(HCNetSDK_COM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/HCNetSDK/lib/HCNetSDKCom)
file(GLOB HCNetSDK_COM_LIBS "${HCNetSDK_COM_LIB_DIR}/*.so*")
set(HCNetSDK_LIBS ${HCNetSDK_LIBS} ${HCNetSDK_COM_LIBS})

target_include_directories(webcamera_hikvision PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${HCNetSDK_INCLUDE_DIRS}
)

target_link_libraries(webcamera_hikvision PUBLIC
    ${OpenCV_LIBS}
    ${HCNetSDK_LIBS}
)

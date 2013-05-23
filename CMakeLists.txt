cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED QtCore)

include_directories(${QT_INCLUDES} ${OPENCV_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(spraytimekeeper_SRCS spraytimekeeper.cpp spraytimekeeperrequest.cpp spraytimekeeperschedule.cpp)

qt4_automoc(${spraytimekeeper_SRCS})

ADD_LIBRARY(spraytimekeeper STATIC ${spraytimekeeper_SRCS})
	   
TARGET_LINK_LIBRARIES(spraytimekeeper ${QT_QTCORE_LIBRARY})

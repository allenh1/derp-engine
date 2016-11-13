QT += network sql core
CONFIG += c++14 debug

SOURCES += src/main.cpp \
           src/ParseHTML.cpp \
           src/crawler.cpp \
           src/file_downloader.cpp \
           src/tcp_thread.cpp \
           src/tcp_connection.cpp \
           src/master_node.cpp
           		   
HEADERS += src/ParseHTML.hpp \ 
           src/crawler.hpp \
           src/file_downloader.hpp \
           src/tcp_thread.hpp \
           src/tcp_connection.hpp \
           src/master_node.hpp

TARGET = derp-engine


cmake_minimum_required(VERSION 3.9.5)
project(derp_engine VERSION 0.0.1)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Sql Network)

set(project_headers
  include/derp-engine/file_downloader.hpp
  include/derp-engine/tcp_thread.hpp
  include/derp-engine/tcp_connection.hpp
  include/derp-engine/master_node.hpp
)

# moc the headers
foreach(header "${project_headers}")
  qt5_wrap_cpp(moc_sources "${header}")
endforeach()

set(project_sources
  src/main.cpp
  src/ParseHTML.cpp
  src/crawler.cpp
  src/file_downloader.cpp
  src/tcp_thread.cpp
  src/tcp_connection.cpp
  src/master_node.cpp
  src/bsearch-dictionary.cpp
)

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME}
  ${project_sources}
  ${project_headers}
  ${ui_wrap}
  ${moc_sources}
  ${qt_resources}
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC Qt5::Sql Qt5::Network Qt5::Core
)

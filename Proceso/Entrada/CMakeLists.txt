cmake_minimum_required(VERSION 3.16)

project(Entrada LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Entrada main.cpp)

include(GNUInstallDirs)
install(TARGETS Entrada
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

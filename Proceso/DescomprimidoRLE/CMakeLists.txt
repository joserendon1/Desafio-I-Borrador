cmake_minimum_required(VERSION 3.16)

project(DescomprimidoRLE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DescomprimidoRLE main.cpp)

include(GNUInstallDirs)
install(TARGETS DescomprimidoRLE
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

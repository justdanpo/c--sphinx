cmake_minimum_required(VERSION 2.6)
project(c--)

set(CMM_SRC main.cpp toka.cpp tokb.cpp tokc.cpp toke.cpp tokr.cpp errors.cpp debug.cpp outobj.cpp outpe.cpp disasm.cpp switch.cpp outle.cpp pointer.cpp new_type.cpp class.cpp res.cpp optreg.cpp libobj.cpp)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUGMODE")

add_definitions(-D__CONSOLE__)

add_executable(c-- ${CMM_SRC})

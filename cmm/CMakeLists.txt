cmake_minimum_required(VERSION 2.6)
project(c--)

#check platform dependend features
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckTypeSize)

CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
if(NOT HAVE_STRCASECMP)
  CHECK_FUNCTION_EXISTS(stricmp HAVE_STRICMP)
  if(NOT HAVE_STRICMP)
    CHECK_FUNCTION_EXISTS(_stricmp HAVE__STRICMP)
  endif()
endif()
#_strcmpi, #strcmpi

CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)
if(NOT HAVE_STRNCASECMP)
  CHECK_FUNCTION_EXISTS(stricmp HAVE_STRNICMP)
  if(NOT HAVE_STRNICMP)
    CHECK_FUNCTION_EXISTS(_stricmp HAVE__STRNICMP)
  endif()
endif()

CHECK_FUNCTION_EXISTS(mbsrtowcs HAVE_MBSRTOWCS)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${PROJECT_BINARY_DIR})

#project source files
set(CMM_SRC main.cpp util.cpp platform.cpp toka.cpp tokb.cpp tokc.cpp toke.cpp tokr.cpp errors.cpp debug.cpp outobj.cpp outpe.cpp disasm.cpp switch.cpp outle.cpp pointer.cpp new_type.cpp class.cpp res.cpp optreg.cpp libobj.cpp)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUGMODE")

add_definitions(-D__CONSOLE__)

add_executable(c-- ${CMM_SRC})

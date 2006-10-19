# Define the project name.
project(adonthell)

cmake_minimum_required(VERSION 2.4.0)

add_definitions(-DVERSION="\\"0.4.0\\"" -DHAVE_CONFIG_H=1)


##########################
# Manage option
##########################

if(NOT ADONTHELL_DATADIR)
       if(WIN32)
		set(ADONTHELL_DATADIR ".")
       else(WIN32)
              SET(ADONTHELL_DATADIR "${CMAKE_INSTALL_PREFIX}/share/games/adonthell")
       endif(WIN32)
endif(NOT ADONTHELL_DATADIR)
message(STATUS
	"Adonthell data directory: ${ADONTHELL_DATADIR}")
add_definitions(-DDATA_DIR="\\"${ADONTHELL_DATADIR}\\"")


# FIXME: We need to manage the package lib directory !
add_definitions(-DPKGLIBDIR="\\"\\"")

include(config/UsePkgConfig.cmake)

##########################
# SDL
##########################

PKGCONFIG_FOUND(sdl SDL_FOUND)
IF(SDL_FOUND)
	PKGCONFIG(sdl SDL_INCLUDE_DIR SDL_LIBADD SDL_LIBRARY SDL_CFLAGS)
	SET(SDL_INCLUDE_DIR ${SDL_INCLUDE_DIR}/SDL)
ELSE(SDL_FOUND)
	find_package(SDL REQUIRED)
ENDIF(SDL_FOUND)

IF(SDL_FOUND)
	MESSAGE(STATUS
		"SDL has been found:")
	MESSAGE(STATUS
		"\tCFLAGS : ${SDL_INCLUDE_DIR}")
	MESSAGE(STATUS
		"\tLDFLAGS: ${SDL_LIBRARY}")
ELSE(SDL_FOUND)
	MESSAGE(FATAL_ERROR
		"SDL is required")
ENDIF(SDL_FOUND)


##########################
# SDL_mixer
##########################

find_package(SDL_mixer REQUIRED PATH_SUFFIXES SDL)
if(SDLMIXER_FOUND)
	# take a lucky guess
	if(${SDLMIXER_INCLUDE_DIR} MATCHES "NOTFOUND")
		set(SDLMIXER_INCLUDE_DIR ${SDL_INCLUDE_DIR})
	endif(${SDLMIXER_INCLUDE_DIR} MATCHES "NOTFOUND")
	
	MESSAGE(STATUS
		"SDL_mixer has been found:")
	MESSAGE(STATUS
		"\tCFLAGS : ${SDLMIXER_INCLUDE_DIR}")
	MESSAGE(STATUS
		"\tLDFLAGS: ${SDLMIXER_LIBRARY}")
else(SDLMIXER_FOUND)
	MESSAGE(FATAL_ERROR
		"SDL_mixer is required")
endif(SDLMIXER_FOUND)


#######################
# Python
#######################
find_package(PythonLibs REQUIRED)
if(PYTHON_INCLUDE_PATH)
	message(STATUS
		"Python has been found:")
	message(STATUS
		"\tCFLAGS : ${PYTHON_INCLUDE_PATH}")
	message(STATUS
		"\tLDFLAGS: ${PYTHON_LIBRARIES}")
else(PYTHON_INCLUDE_PATH)
	message(FATAL_ERROR
		"A Python installation is required")
endif(PYTHON_INCLUDE_PATH)

find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
	EXEC_PROGRAM(${PYTHON_EXECUTABLE}
		ARGS "-c 'import sys;vi = sys.version_info; print \"python%s.%s\" %(vi[0],vi[1])'"
		OUTPUT_VARIABLE PYTHON_VERSION_DIR	
	)
else(PYTHONINTERP_FOUND)
	message(FATAL_ERROR
		"Impossible to find the Python Interpreter")
endif(PYTHONINTERP_FOUND)


#######################
# PNG
#######################

PKGCONFIG_FOUND(libpng PNG_FOUND)
IF(PNG_FOUND)
	PKGCONFIG(libpng PNG_INCLUDE_DIR PNG_LIBADD PNG_LIBRARY PNG_CFLAGS)
ELSE(PNG_FOUND)
	find_package(PNG REQUIRED)
ENDIF(PNG_FOUND)

if(PNG_FOUND)
	MESSAGE(STATUS
		"PNG has been found:")
	MESSAGE(STATUS
		"\tCFLAGS : ${PNG_INCLUDE_DIR}")
	MESSAGE(STATUS
		"\tLDFLAGS: ${PNG_LIBRARY}")
else(PNG_FOUND)
	MESSAGE(FATAL_ERROR
		"PNG is required")
endif(PNG_FOUND)

#######################
# OpenGL
#######################
FIND_PACKAGE(OpenGL)
IF(OPENGL_FOUND)
	MESSAGE(STATUS
		"OpenGL has been found:")
	MESSAGE(STATUS
		"\tCFLAGS : ${OPENGL_INCLUDE_DIR}")
	MESSAGE(STATUS
		"\tLDFLAGS: ${OPENGL_LIBRARY}")
ELSE(OPENGL_FOUND)
	MESSAGE(STATUS
		"OpenGL renderer will not be built")
ENDIF(OPENGL_FOUND)


#######################
# SWIG
#######################

#find_package(SWIG REQUIRED)
include(config/FindSWIG.cmake)
if(SWIG_FOUND)
	MESSAGE(STATUS
		"SWIG is present on the system.")
else(SWIG_FOUND)
	# try to see if we have the swig programm and assume for now that this is sufficient
	FIND_PROGRAM(SWIG_EXECUTABLE NAMES swig PATHS /usr/local/bin )
	if(SWIG_EXECUTABLE)
		MESSAGE(STATUS "SWIG appears to be present on the system.")
		set(SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
		set(SWIG_FOUND YES)
	else(SWIG_EXECUTABLE)
		MESSAGE(FATAL_ERROR "Impossible to locate SWIG.")
	endif(SWIG_EXECUTABLE)
endif(SWIG_FOUND)


#######################
# LibXML2
#######################

PKGCONFIG_FOUND(libxml-2.0 LIBXML2_FOUND)
IF(LIBXML2_FOUND)
	PKGCONFIG(libxml-2.0 LIBXML2_INCLUDE_PATH LIBXML2_LIBADD LIBXML2_LIBRARIES LIBXML2_CFLAGS)
	SET(LIBXML2_INCLUDE_PATH "${LIBXML2_INCLUDE_PATH}/libxml2")
ELSE(LIBXML2_FOUND)
	include(config/FindLibxml2.cmake)
ENDIF(LIBXML2_FOUND)

if(LIBXML2_FOUND)
	message(STATUS
		"LibXml2 has been found:")
	message(STATUS
		"\tCFLAGS : ${LIBXML2_INCLUDE_PATH}")
	message(STATUS
		"\tLDFLAGS: ${LIBXML2_LIBRARIES}")	
else(LIBXML2_FOUND)
	message(FATAL_ERROR
		"LibXml2 is required.")
endif(LIBXML2_FOUND)


######################
# OS X specific
######################
if(APPLE)
	set(CMAKE_SHARED_LINKER_FLAGS "-flat_namespace -undefined suppress")
	set(CMAKE_MODULE_LINKER_FLAGS "-flat_namespace -undefined suppress")
endif(APPLE)


######################
# Build the config.h file !
######################

include (CheckIncludeFiles)
check_include_files (dlfcn.h HAVE_DLFCN_H)
check_include_files (inttypes.h HAVE_INTTYPES_H)
# FIXME: missing check on the z library.
check_include_files (memory.h HAVE_MEMORY_H)
check_include_files (malloc.h HAVE_MALLOC_H)
check_include_files (stdint.h HAVE_STDINT_H)
check_include_files (stdlib.h HAVE_STDLIB_H)
check_include_files (strings.h HAVE_STRINGS_H)
check_include_files (sys/stat.h HAVE_SYS_STAT_H)
check_include_files (sys/types.h HAVE_SYS_TYPES_H)
check_include_files (unistd.h HAVE_UNISTD_H)
check_include_files ("stdlib.h;stdarg.h;string.h" STDC_HEADERS)

include (CheckFunctionExists)
check_function_exists(nanosleep HAVE_NANOSLEEP)

include (CheckLibraryExists) 
check_library_exists(ltdl lt_dlerror "" HAVE_LIBLTDL)

if(NOT HAVE_LIBLTDL)
       message(FATAL_ERROR
		"Libltdl is required.")
endif(NOT HAVE_LIBLTDL)

# Now generate the configure file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)


# Add the root of the binary distribution to the include directories !
# Do it in order to fetch config.h, however, we may generate config.h in the
# src directory.
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)
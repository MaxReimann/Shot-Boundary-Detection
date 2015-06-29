cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(ShotBoundDetection)

#################################################
# PROJECT DESCRIPTION
#################################################

set(META_PROJECT_NAME        "sbd")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "1")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_AUTHOR_ORGANIZATION "PracticalVideoAnalysisSS15")

#################################################
# OPTIONS
#################################################

option(OPTION_LIMIT_CONFIGS  "Generate limited configs (Release; Debug)" ON)
option(OPTION_LOCAL_INSTALL  "Install to a local directory instead of the system" OFF)
option(OPTION_ERRORS_AS_EXCEPTION "Throw exceptions" OFF)
option(OPTION_PORTABLE_INSTALL    "Install to a local directory instead of the system" ON)


#################################################
# CMAKE CONFIGURATION
#################################################

set(VOCR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Include cmake modules from ./cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find the WinSDK libs
if (WIN32)
	set(CMAKE_PREFIX_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64" "C:\\Program Files (x86)\\Windows Kits\\8.1\\Lib\\winv6.3\\um\\x64")
	cmake_policy(SET CMP0020 NEW)
endif (WIN32)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set configuration types
if(OPTION_LIMIT_CONFIGS)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited Configs" FORCE)
endif()

# Project
project(${META_PROJECT_NAME} C CXX)

# Generate folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include custom cmake functions
include(cmake/Custom.cmake)
include(cmake/GitRevision.cmake)

#################################################
# PLATFORM AND ARCHITECTURE
#################################################

# Architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(X64 ON)
endif()

# Check for linux
if(UNIX AND NOT APPLE)
	set(LINUX 1)
endif()

# Setup platform specifics (compile flags, etc., ...)
if(MSVC)
	message(STATUS "Configuring for platform Windows/MSVC.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsMSVC.cmake)
elseif(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Configuring for platform Windows/GCC.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsGCC.cmake)
elseif(LINUX AND CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Configuring for platform Linux/GCC.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformLinuxGCC.cmake)
elseif(APPLE)
	message(STATUS "Configuring for platform MacOS.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformMacOS.cmake)
else()
	# Unsupported system/compiler
	message(WARNING "Unsupported platform/compiler combination")
endif()


#################################################
# DEPENDENCIES
#################################################

# OpenCV
find_package( OpenCV REQUIRED )

add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost COMPONENTS system filesystem regex program_options REQUIRED)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
if (NOT WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} /MP")
endif()
endif()

set (Caffe_DIR "/home/mpss2015/caffe-tmbo/build-cmake/")
find_package( Caffe )


#################################################
# SOURCES
#################################################

set(sources
	src/main.cpp
	src/main.hpp
	src/util.cpp
	src/util.hpp
	src/forwarddeclarations.hpp
	src/hard_cut/hardcut_detection.cpp
	src/hard_cut/hardcut_detection.hpp
	src/hard_cut/util.cpp
	src/hard_cut/util.hpp
	src/hard_cut/histogram/histogram.hpp
	src/hard_cut/histogram/histogram.cpp
	src/hard_cut/svm/svm.hpp
	src/hard_cut/svm/svm.cpp
    src/evaluation/evaluation.hpp
    src/evaluation/evaluation.cpp
    src/option_printing/custom_option_description.cpp
    src/option_printing/custom_option_description.hpp
    src/option_printing/option_printer.cpp
    src/option_printing/option_printer.hpp
    src/gold_standard/file_reader.hpp
    src/gold_standard/file_reader.cpp
    src/gold_standard/gold_standard_statistic.hpp
    src/gold_standard/gold_standard_statistic.cpp
    src/gold_standard/gold_standard_element.hpp
    src/gold_standard/gold_standard_element.cpp
    src/data_generation/transition_generator.cpp
    src/data_generation/transition_generator.hpp
    src/data_generation/data_generation.cpp 
    src/data_generation/data_generation.hpp
    src/soft_cut/classification/caffe_classifier.cpp
    src/soft_cut/classification/caffe_classifier.hpp
    src/soft_cut/classification/merger.cpp
    src/soft_cut/classification/merger.hpp
    src/soft_cut/io/file_writer.cpp
    src/soft_cut/io/file_writer.hpp
    src/soft_cut/io/file_reader.cpp
    src/soft_cut/io/file_reader.cpp
    src/soft_cut/softcut_detection.cpp 
    src/soft_cut/softcut_detection.hpp 

)

source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}/src"
	"\\\\.h$|\\\\.hpp$|\\\\.cpp$|\\\\.c$|\\\\.ui$|\\\\.qrc$" "Source Files" ${sources})

#################################################
# TARGET
#################################################

# Set target name
set(target ${META_PROJECT_NAME})

add_executable(${target} ${sources})


include_directories(
	${CMAKE_SOURCE_DIR}
	${OpenCV_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${Caffe_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR} #for the generated files
)



if (NOT WIN32)
	add_definitions(${Caffe_DEFINITIONS})
endif()

link_directories(${Boost_LIBRARY_DIR})

#prevent linking of caffe libaries on windows
if (WIN32) 
	SET(Caffe_LIBRARIES "")
endif()

target_link_libraries( ${target}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${Caffe_LIBRARIES}
)

#
#  CMakeLists.txt
#  Copyright 2021 ItJustWorksTM
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

cmake_minimum_required (VERSION 3.18)
cmake_policy (SET CMP0022 NEW)
set (CMAKE_POLICY_DEFAULT_CMP0069 NEW) # mosquitto's CMakeLists does not behave otherwise

project (pySMCE
    VERSION 1.5.4.5
    HOMEPAGE_URL "https://github.com/ItJustWorksTM/pySMCE"
    LANGUAGES CXX
)
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")
set (BUILD_SHARED_LIBS Off)
set (CMAKE_POSITION_INDEPENDENT_CODE On)
set (CMAKE_CXX_STANDARD 20) # Bump that down to 17 once libSMCE finally honors that
set (CMAKE_CXX_STANDARD_REQUIRED True)
set (CMAKE_CXX_EXTENSIONS Off)

include (CheckIPOSupported)
check_ipo_supported (RESULT ipo_result OUTPUT ipo_output)
if (ipo_result)
  set (CMAKE_INTERPROCEDURAL_OPTIMIZATION On)
else ()
  message (WARNING "IPO is not supported: ${ipo_output}")
endif ()

# Currently, scikit-build does not support FindPython.
if (SKBUILD)
  message (STATUS "Using SciKit Build")
  set (Python_EXECUTABLE "${PYTHON_EXECUTABLE}" CACHE INTERNAL "")
  set (Python_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}" CACHE INTERNAL "")
  set (Python_LIBRARY "${PYTHON_LIBRARY}" CACHE INTERNAL "")
endif ()

include (SetupSMCE)

set (CMAKE_CXX_STANDARD 20)

if (NOT TARGET Boost::python)
  if (PYSMCE_BOOST_LINKING STREQUAL "STATIC")
    set (Boost_USE_STATIC_LIBS On)
  endif ()
  find_package (Boost 1.74 COMPONENTS python REQUIRED)
endif ()

find_package (Python REQUIRED COMPONENTS Development.Module)

add_library (pySMCE MODULE)
target_link_libraries (pySMCE PRIVATE SMCE_static Python::Module Boost::python)
set_target_properties (pySMCE PROPERTIES
    PREFIX ""
    OUTPUT_NAME "smce"
)
if (WIN32 AND NOT CYGWIN)
  set_target_properties (pySMCE PROPERTIES SUFFIX ".pyd")
endif ()
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_precompile_headers (pySMCE PRIVATE "<boost/python.hpp>")
endif ()
target_sources (pySMCE PRIVATE
    src/pySMCE.cpp
    src/pyUtils.hpp
    src/Board.cpp
    src/BoardConf.cpp
    src/BoardView.cpp
    src/PluginManifest.cpp
    src/Sketch.cpp
    src/SketchConf.cpp
    src/Toolchain.cpp
)
if (NOT MSVC)
  target_compile_options (pySMCE PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror" "-Wcast-align" "-Wno-missing-field-initializers" "-Wno-unused-parameter")
else ()
  target_compile_options (pySMCE PRIVATE "/W4" "/permissive-" "/wd4244" "/wd4251" "/wd4459" "/WX")
endif ()
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set_target_properties (pySMCE PROPERTIES LINK_FLAGS_RELEASE -s)
  if (NOT APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options (pySMCE PRIVATE -static-libstdc++ -static-libgcc)
  endif ()
endif ()

install (TARGETS pySMCE DESTINATION pySMCE)
install (FILES src/__init__.py DESTINATION pySMCE)
install (FILES "${libsmce_BINARY_DIR}/SMCE_Resources.zip" DESTINATION pySMCE)

#
#  SetupSMCE.cmake
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

include_guard ()

include (FetchContent)
FetchContent_Declare (libsmce
    GIT_REPOSITORY "https://github.com/ItJustWorksTM/libSMCE"
    GIT_TAG master
    GIT_SHALLOW On
)
FetchContent_GetProperties (libsmce)
if (NOT libsmce_POPULATED)
  FetchContent_Populate (libsmce)

  file (READ "${libsmce_SOURCE_DIR}/CMakeLists.txt" libsmce_cmakelists)
  string (REPLACE "add_dependencies (SMCE ArdRtRes)" "add_dependencies (SMCE_static ArdRtRes)" libsmce_cmakelists "${libsmce_cmakelists}")
  file (WRITE "${libsmce_SOURCE_DIR}/CMakeLists.txt" "${libsmce_cmakelists}")

  set (SMCE_BUILD_SHARED Off CACHE INTERNAL "")
  set (SMCE_BUILD_STATIC On CACHE INTERNAL "")
  set (SMCE_CXXRT_LINKING "STATIC" CACHE INTERNAL "")
  set (SMCE_BOOST_LINKING "${PYSMCE_BOOST_LINKING}" CACHE INTERNAL "")
  add_subdirectory ("${libsmce_SOURCE_DIR}" "${libsmce_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif ()

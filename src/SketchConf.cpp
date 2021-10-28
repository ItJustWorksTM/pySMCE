/*
 *  SketchConf.cpp
 *  Copyright 2021 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <SMCE/SketchConf.hpp>
#include <boost/python.hpp>

namespace bpy = boost::python;

void export_sketchconf() {
    bpy::scope skc_scope =
        bpy::class_<smce::SketchConfig>{"SketchConfig", "Configuration for sketch building", bpy::init<>{}}
            .def("__init__", bpy::make_constructor(
                                 +[](std::string fqbn) { return new smce::SketchConfig{std::move(fqbn)}; },
                                 bpy::default_call_policies{}, bpy::args("fqbn")))
            .def_readwrite("fqbn", &smce::SketchConfig::fqbn, "Fully Qualified Board Name")
            .def_readwrite("extra_board_uris", &smce::SketchConfig::extra_board_uris,
                           "Extra board.txt URIs for ArduinoCLI")
            .def_readwrite("legacy_preproc_libs", &smce::SketchConfig::legacy_preproc_libs,
                           "Libraries to use during legacy preprocessing");

    bpy::class_<smce::SketchConfig::ArduinoLibrary>{"ArduinoLibrary",
                                                    "Library to pull from the Arduino library manager"}
        .def_readwrite("name", &smce::SketchConfig::ArduinoLibrary::name,
                       "Library name as found in the install command")
        .def_readwrite("version", &smce::SketchConfig::ArduinoLibrary::version, "Version string; empty if latest");
}

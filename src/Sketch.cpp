/*
 *  Sketch.cpp
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

#include <SMCE/Sketch.hpp>
#include <boost/python.hpp>

namespace bpy = boost::python;

void export_sketch() {
    bpy::class_<smce::Sketch, boost::noncopyable>{
        "Sketch", "Arduino Sketch", bpy::init<std::string, smce::SketchConfig>{bpy::args("self", "path", "config")}}
        .def(
            "get_source", +[](const smce::Sketch& self) { return self.get_source().string(); }, bpy::arg{"self"})
        .def_readonly("is_compiled", &smce::Sketch::is_compiled)
        .add_property(
            "uuid", +[](const smce::Sketch& self) { return self.get_uuid().to_hex(); });
}

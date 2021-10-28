/*
 *  Toolchain.cpp
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

#include <string>
#include <SMCE/Toolchain.hpp>
#include <boost/python.hpp>
#include "pyUtils.hpp"

namespace bpy = boost::python;

void export_toolchain() {
    bpy::class_<smce::Toolchain, boost::noncopyable>{"Toolchain", "Compilation environment for sketches",
                                                     bpy::init<std::string>{bpy::args("resources_dir")}}
        .add_property(
            "resource_dir", +[](const smce::Toolchain& tc) { return tc.resource_dir().string(); })
        .add_property("cmake_path", bpy::make_function(&smce::Toolchain::cmake_path,
                                                       bpy::return_value_policy<bpy::copy_const_reference>()))
        .def("check_suitable_environment", pysmce::WrapMemFn<&smce::Toolchain::check_suitable_environment>::ret_errc(),
             bpy::arg{"self"})
        .def("compile", pysmce::WrapMemFn<&smce::Toolchain::compile>::ret_errc(), bpy::args("self", "sketch"));
}

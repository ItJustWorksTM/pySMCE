/*
 *  PluginManifest.cpp
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

#include <SMCE/PluginManifest.hpp>
#include <boost/python.hpp>

namespace bpy = boost::python;

void export_plugman() {
    bpy::scope pm_scope =
        bpy::class_<smce::PluginManifest>{"PluginManifest", "Ardrivo plugin manifest contents", bpy::init<>{}}
            .def_readwrite("name", &smce::PluginManifest::name)
            .def_readwrite("version", &smce::PluginManifest::version)
            .def_readwrite("depends", &smce::PluginManifest::depends, "required plugins")
            .def_readwrite("uri", &smce::PluginManifest::uri,
                           "file:// of the source-root or http[s]:// of the tar/zip archive")
            .def_readwrite("patch_uri", &smce::PluginManifest::patch_uri, "same as uri but for patching")
            .def_readwrite("defaults", &smce::PluginManifest::defaults)
            .def_readwrite("incdirs", &smce::PluginManifest::incdirs)
            .def_readwrite("sources", &smce::PluginManifest::sources)
            .def_readwrite("linkdirs", &smce::PluginManifest::linkdirs)
            .def_readwrite("linklibs", &smce::PluginManifest::linklibs)
            .def_readwrite("development", &smce::PluginManifest::development,
                           "set to true to CONFIGURE_DEPENDS all the globs");

    bpy::enum_<smce::PluginManifest::Defaults>{"Defaults"}
        .value("arduino", smce::PluginManifest::Defaults::arduino)
        .value("single_dir", smce::PluginManifest::Defaults::single_dir)
        .value("c", smce::PluginManifest::Defaults::c)
        .value("none", smce::PluginManifest::Defaults::none)
        .value("cmake", smce::PluginManifest::Defaults::cmake);
}

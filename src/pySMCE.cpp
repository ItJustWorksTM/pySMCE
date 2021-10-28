/*
 *  pySMCE.cpp
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

#include <boost/python.hpp>

void export_board();
void export_boardconf();
void export_boardview();
void export_plugman();
void export_sketch();
void export_sketchconf();
void export_toolchain();

BOOST_PYTHON_MODULE(smce) {
    export_board();
    export_boardconf();
    export_boardview();
    export_plugman();
    export_sketch();
    export_sketchconf();
    export_toolchain();
}

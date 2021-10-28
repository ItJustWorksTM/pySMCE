/*
 *  Board.cpp
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

#include <SMCE/Board.hpp>
#include <SMCE/Sketch.hpp>
#include <boost/python.hpp>

namespace bpy = boost::python;

void export_board() {
    [[maybe_unused]] bpy::scope brd_scope =
        bpy::class_<smce::Board, boost::noncopyable>{"Board", bpy::init<>(bpy::arg{"self"})}
            .def("__init__", bpy::make_constructor(
                                 +[](const bpy::object& exit_notify) {
                                     return new smce::Board{[en = exit_notify](int ec) mutable { en(ec); }};
                                 },
                                 bpy::default_call_policies{}, bpy::arg{"exit_notify"}))
            .def_readonly("status", &smce::Board::status)
            .def("view", &smce::Board::view, bpy::arg{"self"})
            .def("attach_sketch", &smce::Board::attach_sketch, bpy::with_custodian_and_ward<2, 1>{})
            .def("get_sketch", &smce::Board::get_sketch,
                 bpy::return_value_policy<bpy::reference_existing_object>{}) // maybe collapse with above into property?
            .def("tick", &smce::Board::tick, bpy::arg{"self"})
            .def("reset", &smce::Board::reset, bpy::arg{"self"})
            .def("configure", &smce::Board::configure, bpy::args("self", "bconf"))
            .def("prepare", &smce::Board::prepare, bpy::arg{"self"})
            .def("start", &smce::Board::start, bpy::arg{"self"})
            .def("suspend", &smce::Board::suspend, bpy::arg{"self"})
            .def("resume", &smce::Board::resume, bpy::arg{"self"})
            .def("terminate", &smce::Board::terminate, bpy::arg{"self"})
            .def(
                "stop", +[](smce::Board& b, std::uint64_t ms) { return b.stop(std::chrono::milliseconds{ms}); },
                (bpy::arg{"self"}, bpy::arg{"timeout"} = 1000));

    bpy::enum_<smce::Board::Status>{"Status"}
        .value("clean", smce::Board::Status::clean)
        .value("configured", smce::Board::Status::configured)
        .value("prepared", smce::Board::Status::prepared)
        .value("running", smce::Board::Status::running)
        .value("suspended", smce::Board::Status::suspended)
        .value("stopped", smce::Board::Status::stopped);
}

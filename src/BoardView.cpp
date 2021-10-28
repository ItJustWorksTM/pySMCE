/*
 *  BoardView.cpp
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

#include <SMCE/BoardView.hpp>
#include <boost/python.hpp>

namespace bpy = boost::python;

void export_boardview() {
    bpy::class_<smce::BoardView>{"BoardView", "Mutable view of the virtual board.", bpy::no_init}
        .def_readonly("pins", &smce::BoardView::pins, "GPIO pins")
        .def_readonly("uart_channels", &smce::BoardView::uart_channels, "UART channels")
        .def_readonly("frame_buffers", &smce::BoardView::frame_buffers, "Camera/Screen frame-buffers")
        .add_property("valid", &smce::BoardView::valid, "Object validity check")

        .def("__bool__", &smce::BoardView::valid, bpy::arg{"self"})
        .add_property("stop_requested", &smce::BoardView::stop_requested,
                      "Whether or not there is an active stop request from the host");

    bpy::class_<smce::VirtualPins>{"VirtualPins", bpy::no_init}.def("__getitem__", &smce::VirtualPins::operator[],
                                                                    bpy::args("self", "pin_id"));

    {
        [[maybe_unused]] bpy::scope vpin_scope =
            bpy::class_<smce::VirtualPin>{"VirtualPin", bpy::no_init}
                .add_property("exists", &smce::VirtualPin::exists, "Object validity check")
                .def("__bool__", &smce::VirtualPin::exists, bpy::arg{"self"})
                .add_property("locked", &smce::VirtualPin::locked)
                .add_property("direction", &smce::VirtualPin::get_direction, &smce::VirtualPin::set_direction)
                .add_property("digital", &smce::VirtualPin::digital)
                .add_property("analog", &smce::VirtualPin::analog);
        bpy::enum_<smce::VirtualPin::DataDirection>{"DataDirection"}
            .value("in", smce::VirtualPin::DataDirection::in)
            .value("out", smce::VirtualPin::DataDirection::out);
    }

    bpy::class_<smce::VirtualDigitalDriver>{"VirtualDigitalDriver", "Digital driver for a GPIO pin", bpy::no_init}
        .add_property("exists", &smce::VirtualDigitalDriver::exists, "Object validity check")
        .def("__bool__", &smce::VirtualDigitalDriver::exists, bpy::arg{"self"})
        .add_property("can_read", &smce::VirtualDigitalDriver::can_read)
        .add_property("can_write", &smce::VirtualDigitalDriver::can_write)
        .add_property("value", &smce::VirtualDigitalDriver::read, &smce::VirtualDigitalDriver::write)
        .def("read", &smce::VirtualDigitalDriver::read, bpy::arg{"self"})
        .def("write", &smce::VirtualDigitalDriver::write, bpy::args("self", "value"));

    bpy::class_<smce::VirtualAnalogDriver>{"VirtualAnalogDriver", "Analog driver for a GPIO pin", bpy::no_init}
        .add_property("exists", &smce::VirtualAnalogDriver::exists, "Object validity check")
        .def("__bool__", &smce::VirtualAnalogDriver::exists, bpy::arg{"self"})
        .add_property("can_read", &smce::VirtualAnalogDriver::can_read)
        .add_property("can_write", &smce::VirtualAnalogDriver::can_write)
        .add_property("value", &smce::VirtualAnalogDriver::read, &smce::VirtualAnalogDriver::write)
        .def("read", &smce::VirtualAnalogDriver::read)
        .def("write", &smce::VirtualAnalogDriver::write);

    bpy::class_<smce::VirtualUarts>{"VirtualUarts", bpy::no_init}
        .def("__getitem__", &smce::VirtualUarts::operator[], bpy::args("self", "index"))
        .def("__len__", &smce::VirtualUarts::size, bpy::arg{"self"})
        .def("__iter__", bpy::iterator<smce::VirtualUarts>());

    bpy::class_<smce::VirtualUart>{"VirtualUart", bpy::no_init}
        .add_property("exists", &smce::VirtualUart::exists, "Object validity check")
        .def("__bool__", &smce::VirtualUart::exists, bpy::arg{"self"})
        .add_property("active", &smce::VirtualUart::is_active, &smce::VirtualUart::set_active)
        .def_readonly("rx", &smce::VirtualUart::rx)
        .def_readonly("tx", &smce::VirtualUart::tx);

    bpy::class_<smce::VirtualUartBuffer>{"VirtualUartBuffer", bpy::no_init}
        .add_property("exists", &smce::VirtualUartBuffer::exists, "Object validity check")
        .def("__bool__", &smce::VirtualUartBuffer::exists, bpy::arg{"self"})
        .def_readonly("max_size", &smce::VirtualUartBuffer::max_size)
        .def_readonly("size", &smce::VirtualUartBuffer::size)
        .def(
            "read",
            +[](smce::VirtualUartBuffer& vbuf, bpy::object& pyobj) {
                return vbuf.read(
                    {::PyByteArray_AsString(pyobj.ptr()), static_cast<std::size_t>(::PyByteArray_Size(pyobj.ptr()))});
            },
            bpy::args("self", "buffer"))
        .def(
            "read",
            +[](smce::VirtualUartBuffer& vbuf) {
                ::PyObject* pyptr = ::PyByteArray_FromStringAndSize(nullptr, 0);
                ::PyByteArray_Resize(pyptr, static_cast<::ssize_t>(vbuf.max_size()));
                const auto bytes_count =
                    vbuf.read({::PyByteArray_AsString(pyptr), static_cast<std::size_t>(::PyByteArray_Size(pyptr))});
                ::PyByteArray_Resize(pyptr, static_cast<::ssize_t>(bytes_count));
                return bpy::object{bpy::handle<>(pyptr)};
            },
            bpy::arg{"self"})
        .def(
            "write",
            +[](smce::VirtualUartBuffer& vbuf, bpy::object& pyobj) {
                if (PyByteArray_Check(pyobj.ptr()))
                    return vbuf.write({::PyByteArray_AsString(pyobj.ptr()),
                                       static_cast<std::size_t>(::PyByteArray_Size(pyobj.ptr()))});
                return vbuf.write(
                    {::PyBytes_AsString(pyobj.ptr()), static_cast<std::size_t>(::PyBytes_Size(pyobj.ptr()))});
            },
            bpy::args("self", "buffer"))
        .def("front", &smce::VirtualUartBuffer::front, bpy::arg{"self"});

    bpy::class_<smce::FrameBuffers>{"FrameBuffers", bpy::no_init}.def("__getitem__", &smce::FrameBuffers::operator[],
                                                                      bpy::args("self", "key"));

    bpy::class_<smce::FrameBuffer>{"FrameBuffer", bpy::no_init}
        .add_property("exists", &smce::FrameBuffer::exists, "Object validity check")
        .def("__bool__", &smce::FrameBuffer::exists, bpy::arg{"self"})
        .def_readonly("direction", &smce::FrameBuffer::direction)
        .add_property("needs_horizontal_flip",
                      static_cast<bool (smce::FrameBuffer::*)()>(&smce::FrameBuffer::needs_horizontal_flip),
                      static_cast<void (smce::FrameBuffer::*)(bool)>(&smce::FrameBuffer::needs_horizontal_flip))
        .add_property("needs_vertical_flip",
                      static_cast<bool (smce::FrameBuffer::*)()>(&smce::FrameBuffer::needs_vertical_flip),
                      static_cast<void (smce::FrameBuffer::*)(bool)>(&smce::FrameBuffer::needs_vertical_flip))
        .add_property("width", &smce::FrameBuffer::get_width, &smce::FrameBuffer::set_width)
        .add_property("height", &smce::FrameBuffer::get_height, &smce::FrameBuffer::set_height)
        .add_property("freq", &smce::FrameBuffer::get_freq, &smce::FrameBuffer::set_freq)
        .def(
            "read_rgb888",
            +[](smce::FrameBuffer& fb, bpy::object& pyobj) {
                return fb.read_rgb888({reinterpret_cast<std::byte*>(::PyByteArray_AsString(pyobj.ptr())),
                                       static_cast<std::size_t>(::PyByteArray_Size(pyobj.ptr()))});
            },
            bpy::args("self", "buffer"))
        .def(
            "read_rgb888",
            +[](smce::FrameBuffer& fb) {
                ::PyObject* pyptr = ::PyByteArray_FromStringAndSize(nullptr, 0);
                ::PyByteArray_Resize(pyptr, fb.get_width() * fb.get_height());
                const auto bytes_count = fb.read_rgb888({reinterpret_cast<std::byte*>(::PyByteArray_AsString(pyptr)),
                                                         static_cast<std::size_t>(::PyByteArray_Size(pyptr))});
                ::PyByteArray_Resize(pyptr, bytes_count);
                return bpy::object{bpy::handle<>(pyptr)};
            },
            bpy::arg{"self"})
        .def(
            "write_rgb888",
            +[](smce::FrameBuffer& fb, bpy::object& pyobj) {
                if (PyByteArray_Check(pyobj.ptr()))
                    return fb.write_rgb888({reinterpret_cast<const std::byte*>(::PyByteArray_AsString(pyobj.ptr())),
                                            static_cast<std::size_t>(::PyByteArray_Size(pyobj.ptr()))});
                return fb.write_rgb888({reinterpret_cast<const std::byte*>(::PyBytes_AsString(pyobj.ptr())),
                                        static_cast<std::size_t>(::PyBytes_Size(pyobj.ptr()))});
            },
            bpy::args("self", "buffer"))
        .def(
            "read_rgb444",
            +[](smce::FrameBuffer& fb, bpy::object& pyobj) {
                return fb.read_rgb444({reinterpret_cast<std::byte*>(::PyByteArray_AsString(pyobj.ptr())),
                                       static_cast<std::size_t>(::PyByteArray_Size(pyobj.ptr()))});
            },
            bpy::args("self", "buffer"))
        .def(
            "read_rgb444",
            +[](smce::FrameBuffer& fb) {
                ::PyObject* pyptr = ::PyByteArray_FromStringAndSize(nullptr, 0);
                ::PyByteArray_Resize(pyptr, fb.get_width() * fb.get_height());
                const auto bytes_count = fb.read_rgb444({reinterpret_cast<std::byte*>(::PyByteArray_AsString(pyptr)),
                                                         static_cast<std::size_t>(::PyByteArray_Size(pyptr))});
                ::PyByteArray_Resize(pyptr, bytes_count);
                return bpy::object{bpy::handle<>(pyptr)};
            },
            bpy::arg{"self"})
        .def(
            "write_rgb444",
            +[](smce::FrameBuffer& fb, bpy::object& pyobj) {
                if (PyByteArray_Check(pyobj.ptr()))
                    return fb.write_rgb444({reinterpret_cast<const std::byte*>(::PyByteArray_AsString(pyobj.ptr())),
                                            static_cast<std::size_t>(::PyByteArray_Size(pyobj.ptr()))});
                return fb.write_rgb444({reinterpret_cast<const std::byte*>(::PyBytes_AsString(pyobj.ptr())),
                                        static_cast<std::size_t>(::PyBytes_Size(pyobj.ptr()))});
            },
            bpy::args("self", "buffer"));
}

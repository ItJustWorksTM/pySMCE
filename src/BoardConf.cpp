/*
 * BoardConf.cpp
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

#include <algorithm>
#include <iterator>
#include <numeric>
#include <SMCE/BoardConf.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "pyUtils.hpp"

namespace bpy = boost::python;

void export_boardconf() {
    {
        [[maybe_unused]] bpy::scope brdcnf_scope =
            bpy::class_<smce::BoardConfig>{"BoardConfig", "Configuration for board hardware", bpy::init<>{}}
                .def("__init__",
                     bpy::make_constructor(
                         +[](const bpy::object& pins_obj, const bpy::object& gpiods_obj, const bpy::object& uartcs_obj,
                             const bpy::object& sdcs_obj, const bpy::object& fbs_obj) {
                             return new smce::BoardConfig{
                                 pysmce::pyseq_copycollect_vec<std::uint16_t>(pins_obj),
                                 pysmce::pyseq_copycollect_vec<smce::BoardConfig::GpioDrivers>(gpiods_obj),
                                 pysmce::pyseq_copycollect_vec<smce::BoardConfig::UartChannel>(uartcs_obj),
                                 pysmce::pyseq_copycollect_vec<smce::BoardConfig::SecureDigitalStorage>(sdcs_obj),
                                 pysmce::pyseq_copycollect_vec<smce::BoardConfig::FrameBuffer>(fbs_obj)};
                         },
                         bpy::default_call_policies{},
                         (bpy::arg{"pins"} = bpy::object{}, bpy::arg{"gpio_drivers"} = bpy::object{},
                          bpy::arg{"uart_channels"} = bpy::object{}, bpy::arg{"sd_cards"} = bpy::object{},
                          bpy::arg{"frame_buffers"} = bpy::object{})))
                .add_property(
                    "pins", &smce::BoardConfig::pins,
                    +[](smce::BoardConfig& bc, const bpy::object& obj) {
                        bpy::stl_input_iterator<std::uint16_t> b{obj}, e;
                        bc.pins.clear();
                        std::copy(b, e, std::back_inserter(bc.pins));
                    },
                    "GPIO pins")
                .def_readwrite("gpio_drivers", &smce::BoardConfig::gpio_drivers,
                               "GPIO drivers to apply on existing pins")
                .def_readwrite("uart_channels", &smce::BoardConfig::uart_channels, "UART channels")
                .def_readwrite("sd_cards", &smce::BoardConfig::sd_cards)
                .def_readwrite("frame_buffers", &smce::BoardConfig::frame_buffers, "Frame-buffers (cameras & screens)");

        bpy::class_<std::vector<std::uint16_t>>{"PinsVector"}
            .def(bpy::vector_indexing_suite<std::vector<std::uint16_t>>())
            .def(
                "__str__", +[](const std::vector<std::uint16_t>& vec) {
                    constexpr std::size_t max_segment_length =
                        std::numeric_limits<std::remove_cvref_t<decltype(vec)>::value_type>::digits10 + std::size(", ");
                    std::string acc;
                    acc.reserve(vec.size() * max_segment_length + sizeof '[' + sizeof ']');
                    acc += "[";
                    bool needs_comma = false;
                    for (const auto e : vec) {
                        if (needs_comma)
                            acc += ", ";
                        needs_comma = true;
                        acc += std::to_string(e);
                    }
                    acc += ']';
                    return bpy::str{acc.data(), acc.length()};
                });

        {
            using gpio_digidrv = pysmce::WrapMemVar<&smce::BoardConfig::GpioDrivers::digital_driver>;
            using gpio_anlgdrv = pysmce::WrapMemVar<&smce::BoardConfig::GpioDrivers::analog_driver>;

            [[maybe_unused]] bpy::scope gpiodrv_scope =
                bpy::class_<smce::BoardConfig::GpioDrivers>{"GpioDrivers", bpy::init<>{}}
                    .def("__init__",
                         bpy::make_constructor(
                             +[](std::uint16_t pid, const bpy::object& ddrv, const bpy::object& adrv) {
                                 return new smce::BoardConfig::GpioDrivers{
                                     pid,
                                     !adrv.is_none() ? bpy::extract<smce::BoardConfig::GpioDrivers::DigitalDriver>(ddrv)
                                                     : bpy::object{},
                                     !adrv.is_none() ? bpy::extract<smce::BoardConfig::GpioDrivers::AnalogDriver>(adrv)
                                                     : bpy::object{}};
                             },
                             bpy::default_call_policies{},
                             (bpy::arg{"pin_id"}, bpy::arg{"digital_driver"} = bpy::object{},
                              bpy::arg{"analog_driver"} = bpy::object{})))
                    .def_readwrite("pin_id", &smce::BoardConfig::GpioDrivers::pin_id)
                    .add_property("digital_driver", gpio_digidrv::opt_get(), gpio_digidrv::opt_set())
                    .add_property("analog_driver", gpio_anlgdrv::opt_get(), gpio_anlgdrv::opt_set());
            bpy::class_<smce::BoardConfig::GpioDrivers::DigitalDriver>{"DigitalDriver", bpy::init<>{}}
                .def("__init__", bpy::make_constructor(
                                     +[](bool br, bool bw) {
                                         return new smce::BoardConfig::GpioDrivers::DigitalDriver{br, bw};
                                     },
                                     bpy::default_call_policies{}, bpy::args("board_read", "board_write")))
                .def_readwrite("board_read", &smce::BoardConfig::GpioDrivers::DigitalDriver::board_read)
                .def_readwrite("board_write", &smce::BoardConfig::GpioDrivers::DigitalDriver::board_write);
            bpy::class_<smce::BoardConfig::GpioDrivers::AnalogDriver>{"AnalogDriver", bpy::init<>{}}
                .def("__init__", bpy::make_constructor(
                                     +[](bool br, bool bw) {
                                         return new smce::BoardConfig::GpioDrivers::AnalogDriver{br, bw};
                                     },
                                     bpy::default_call_policies{}, bpy::args("board_read", "board_write")))
                .def_readwrite("board_read", &smce::BoardConfig::GpioDrivers::AnalogDriver::board_read)
                .def_readwrite("board_write", &smce::BoardConfig::GpioDrivers::AnalogDriver::board_write);
        }
        bpy::class_<std::vector<smce::BoardConfig::GpioDrivers>>{"GpioDriversVector"}.def(
            bpy::vector_indexing_suite<std::vector<smce::BoardConfig::GpioDrivers>>());

        using uart_rx_pin = pysmce::WrapMemVar<&smce::BoardConfig::UartChannel::rx_pin_override>;
        using uart_tx_pin = pysmce::WrapMemVar<&smce::BoardConfig::UartChannel::tx_pin_override>;
        bpy::class_<smce::BoardConfig::UartChannel>{"UartChannel", bpy::init<>{}}
            .add_property("rx_pin_override", uart_rx_pin::opt_get(), uart_rx_pin::opt_set())
            .add_property("tx_pin_override", uart_tx_pin::opt_get(), uart_tx_pin::opt_set())
            .def_readwrite("baud_rate", &smce::BoardConfig::UartChannel::baud_rate)
            .def_readwrite("rx_buffer_length", &smce::BoardConfig::UartChannel::rx_buffer_length)
            .def_readwrite("tx_buffer_length", &smce::BoardConfig::UartChannel::tx_buffer_length)
            .def_readwrite("flushing_threshold", &smce::BoardConfig::UartChannel::flushing_threshold);
        bpy::class_<std::vector<smce::BoardConfig::UartChannel>>{"UartChannelVector"}.def(
            bpy::vector_indexing_suite<std::vector<smce::BoardConfig::UartChannel>>());

        using sds_root = pysmce::WrapMemVar<&smce::BoardConfig::SecureDigitalStorage::root_dir>;
        bpy::class_<smce::BoardConfig::SecureDigitalStorage>{"SecureDigitalStorage", bpy::init<>{}}
            .def("__init__", bpy::make_constructor(
                                 +[](std::uint16_t cspin, std::string root_dir) {
                                     return new smce::BoardConfig::SecureDigitalStorage{cspin, std::move(root_dir)};
                                 },
                                 bpy::default_call_policies{}, (bpy::arg{"cspin"} = 0, bpy::arg{"root_dir"})))
            .def_readwrite("cspin", &smce::BoardConfig::SecureDigitalStorage::cspin)
            .add_property("root_dir", sds_root::cvt_get<std::string>(), sds_root::cvt_set<std::string>());
        bpy::class_<std::vector<smce::BoardConfig::SecureDigitalStorage>>{"SecureDigitalStorageVector"}.def(
            bpy::vector_indexing_suite<std::vector<smce::BoardConfig::SecureDigitalStorage>>());

        {
            [[maybe_unused]] bpy::scope fb_scope =
                bpy::class_<smce::BoardConfig::FrameBuffer>{"FrameBuffer"}
                    .def("__init__", bpy::make_constructor(
                                         +[](std::size_t key, smce::BoardConfig::FrameBuffer::Direction dir) {
                                             return new smce::BoardConfig::FrameBuffer{key, dir};
                                         },
                                         bpy::default_call_policies{}, bpy::args("key", "direction")))
                    .def_readwrite("key", &smce::BoardConfig::FrameBuffer::key)
                    .def_readwrite("direction", &smce::BoardConfig::FrameBuffer::direction);
            bpy::enum_<smce::BoardConfig::FrameBuffer::Direction>{"Direction"}
                .value("in", smce::BoardConfig::FrameBuffer::Direction::in)
                .value("out", smce::BoardConfig::FrameBuffer::Direction::out);
        }
        bpy::class_<std::vector<smce::BoardConfig::FrameBuffer>>{"FrameBufferVector"}.def(
            bpy::vector_indexing_suite<std::vector<smce::BoardConfig::FrameBuffer>>());
    }
}

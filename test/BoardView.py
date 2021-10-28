#
#  BoardView.py
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

from pySMCE import smce

from defs import *
from time import sleep


def test_boardview_gpio(smce_root):
    tc = smce.Toolchain(smce_root)
    tc.check_suitable_environment()
    sk = smce.Sketch(SKETCHES_PATH + "pins", smce.SketchConfig(fqbn='arduino:avr:nano'))
    tc.compile(sk)
    br = smce.Board()
    assert br.configure(smce.BoardConfig(
        pins=[0, 2],
        gpio_drivers=[
            smce.BoardConfig.GpioDrivers(
                pin_id=0,
                digital_driver=smce.BoardConfig.GpioDrivers.DigitalDriver(True, False),
                analog_driver=smce.BoardConfig.GpioDrivers.AnalogDriver(True, False)
            ),
            smce.BoardConfig.GpioDrivers(
                pin_id=2,
                digital_driver=smce.BoardConfig.GpioDrivers.DigitalDriver(False, True),
                analog_driver=smce.BoardConfig.GpioDrivers.AnalogDriver(False, True)
            )
        ]
    ))
    assert br.attach_sketch(sk)
    assert br.start()
    bv = br.view()
    assert bv
    pin0 = bv.pins[0]
    assert pin0
    pin0d = pin0.digital
    assert pin0d
    assert pin0d.can_read
    assert not pin0d.can_write
    pin0a = pin0.analog
    assert pin0a
    assert pin0a.can_read
    assert not pin0a.can_write
    pin1 = bv.pins[1]
    assert not pin1
    pin2 = bv.pins[2]
    assert pin2
    pin2d = pin2.digital
    assert pin2d
    assert not pin2d.can_read
    assert pin2d.can_write
    pin2a = pin2.analog
    assert pin2a
    assert not pin2a.can_read
    assert pin2a.can_write
    sleep(0.001)
    pin0d.value = False
    assert_pin_delayable(pin2d, True, 16384)
    pin0d.value = True
    assert_pin_delayable(pin2d, False, 16384)
    assert br.stop()

#
#  Board.py
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

from asyncio import create_task, sleep, wait_for
from asyncio.futures import Future
from os import path


def test_board_contracts(smce_root):
    tc = smce.Toolchain(smce_root)
    tc.check_suitable_environment()
    sk = smce.Sketch(path.join(SKETCHES_PATH, 'noop'), smce.SketchConfig(fqbn='arduino:avr:nano'))
    tc.compile(sk)
    assert sk.is_compiled
    br = smce.Board()
    assert br.status == smce.Board.Status.clean
    assert not br.view()
    assert br.configure(smce.BoardConfig())
    assert br.status == smce.Board.Status.configured
    assert not br.view()
    assert br.attach_sketch(sk)
    assert not br.view()
    assert br.prepare()
    assert br.status == smce.Board.Status.prepared
    assert br.view()
    assert br.start()
    assert br.status == smce.Board.Status.running
    assert br.view()
    assert br.suspend()
    assert br.status == smce.Board.Status.suspended
    assert br.view()
    assert br.resume()
    assert br.status == smce.Board.Status.running
    assert br.view()
    assert br.stop()
    assert br.status == smce.Board.Status.stopped
    assert not br.view()
    assert br.reset()
    assert br.status == smce.Board.Status.clean
    assert not br.view()


@pytest.mark.asyncio
async def test_board_exit_notify(smce_root):
    tc = smce.Toolchain(smce_root)
    tc.check_suitable_environment()
    sk = smce.Sketch(path.join(SKETCHES_PATH, 'uncaught'), smce.SketchConfig(fqbn='arduino:avr:nano'))
    tc.compile(sk)
    ex = Future()

    async def board_ticker(board):
        while not ex.done():
            board.tick()
            await sleep(0.01)

    br = smce.Board(lambda ec: ex.set_result(ec))
    tick_task = create_task(board_ticker(br))

    assert br.configure(smce.BoardConfig())
    assert br.attach_sketch(sk)
    assert br.start()

    await wait_for(ex, timeout=5)
    assert ex.result() != 0
    await tick_task


def test_mixed_inocxx_sources(smce_root):
    tc = smce.Toolchain(smce_root)
    tc.check_suitable_environment()
    sk = smce.Sketch(path.join(SKETCHES_PATH, 'with_cxx'), smce.SketchConfig(fqbn='arduino:avr:nano'))
    tc.compile(sk)

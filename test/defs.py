#
#  defs.py
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

from pySMCE import smce, resources_archive_path

import pytest
from os import environ, makedirs, path
from time import sleep
from zipfile import ZipFile


@pytest.fixture
def smce_root():
    testdir_root = path.realpath(path.dirname(path.abspath(__file__)) + '/../.pySMCE-testdir')
    makedirs(testdir_root, exist_ok=True)
    ZipFile(resources_archive_path).extractall(testdir_root)
    return testdir_root


SMCE_TEST_SOURCES = path.realpath(path.dirname(path.abspath(__file__)) + '/../external/libSMCE/test')
SKETCHES_PATH = SMCE_TEST_SOURCES + '/sketches/'
PLUGINS_PATH = SMCE_TEST_SOURCES + '/plugins/'
PATCHES_PATH = SMCE_TEST_SOURCES + '/patches/'
STORAGE_PATH = SMCE_TEST_SOURCES + '/storage/'
MANIFESTS_PATH = SMCE_TEST_SOURCES + '/manifests/'


def assert_pin_delayable(pin, expected_value, ticks, tick_ms=1):
    while pin.value != expected_value:
        if ticks == 0:
            pytest.fail('Timed out pin-wait')
        sleep(tick_ms / 1000.0)
        ticks -= 1

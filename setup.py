#!/usr/bin/env python3
from skbuild import setup
from packaging.version import LegacyVersion
from skbuild.exceptions import SKBuildError
from skbuild.cmaker import get_cmake_version

setup_requires = []
try:
    if LegacyVersion(get_cmake_version()) < LegacyVersion("3.16"):
        setup_requires.append('cmake')
except SKBuildError:
    setup_requires.append('cmake')

setup(
    name='pySMCE',
    version='1.4.0',
    description='Python binding for libSMCE',
    author='ItJustWorks™',
    license="Apache-2.0",
    tests_require=['pytest', 'pytest-asyncio'],
    setup_requires=setup_requires,
    cmake_args=['-DPYSMCE_BOOST_LINKING=STATIC']
)

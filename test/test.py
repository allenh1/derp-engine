# Copyright 2018 Hunter L. Allen
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import os
import sys

from docker.errors import ContainerError
from superflore.docker import Docker
from superflore.utils import err
from superflore.utils import info
from superflore.utils import ok


repo_location="/tmp/repo"

def set_up_container():
    d = Docker()
    curr = os.getcwd()
    try:
        d.pull('allenh1', 'asw_test_env')
    except:
        d.build(os.path.realpath("../docker/Dockerfile"))
    info("Using repo in directory '%s'" % os.path.realpath(".."))
    d.map_directory(os.path.realpath(".."), repo_location)
    return d


def test_ament_uncrustify(container, auto_fix=False, verbose=False):
    container.add_bash_command('cd %s' % repo_location)
    container.add_bash_command('source /opt/ros/dashing/setup.bash')
    fix = ''
    if auto_fix:
        fix = "--reformat "
    container.add_bash_command('ament_uncrustify %ssrc include test' % fix)
    try:
        container.run(show_cmd=False)
        if verbose:
            print(container.log)
    except ContainerError:
        print(container.log)
        return 1
    container.clear_commands()
    return 0


def test_ament_copyright(container, auto_fix=False, verbose=False):
    container.add_bash_command('cd %s' % repo_location)
    container.add_bash_command('source /opt/ros/dashing/setup.bash')
    copyright_fix = ''
    if auto_fix:
        copyright_fix = '--add-missing Hunter\\ L.\\ Allen apache2 '
    container.add_bash_command('ament_copyright %ssrc include test' % copyright_fix)
    try:
        container.run(show_cmd=verbose)
        if verbose:
            print(container.log)
    except ContainerError:
        print(container.log)
        return 1
    container.clear_commands()
    return 0


def test_ament_cppcheck(container, verbose=False):
    container.add_bash_command('cd %s' % repo_location)
    container.add_bash_command('source /opt/ros/dashing/setup.bash')
    container.add_bash_command('ament_cppcheck src include test')
    try:
        container.run(show_cmd=verbose)
        if verbose:
            print(container.log)
    except ContainerError:
        print(container.log)
        return 1
    container.clear_commands()
    return 0


def test_ament_cpplint(container, verbose=False):
    container.add_bash_command('cd %s' % repo_location)
    container.add_bash_command('source /opt/ros/dashing/setup.bash')
    container.add_bash_command('ament_cpplint src include test')
    try:
        container.run(show_cmd=verbose)
        if verbose:
            print(container.log)
    except ContainerError:
        print(container.log)
        return 1
    container.clear_commands()
    return 0


def test_compilation(container, verbose=False):
    container.add_bash_command('cd %s' % repo_location)
    container.add_bash_command('mkdir build && cd build')
    container.add_bash_command('cmake ..')
    container.add_bash_command('make -j$(nproc)')
    try:
        container.run(show_cmd=verbose)
        if verbose:
            print(container.log)
    except ContainerError:
        print(container.log)
        return 1
    container.clear_commands()
    return 0


def main(argv):
    argv = argv[1:]
    parser = argparse.ArgumentParser('ASW Docker Test Runner')
    parser.add_argument(
        '--fix-style',
        help='automatically fix the code style',
        action="store_true"
    )
    parser.add_argument(
        '--add-missing-copyright',
        help='automatically fix the copyright header',
        action="store_true"
    )
    parser.add_argument(
        '--verbose',
        help="verbose output",
        action="store_true"
    )
    args = parser.parse_args(argv)
    d = set_up_container()
    score = test_ament_uncrustify(d, args.fix_style)
    score += test_ament_copyright(d, args.add_missing_copyright)
    score += test_ament_cppcheck(d)
    score += test_ament_cpplint(d)
    score += test_compilation(d)
    return score


if __name__ == '__main__':
    sys.exit(main(sys.argv))

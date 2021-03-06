#!/usr/bin/env python3

"""
compile comamnds functions

"""

import argparse
import os
import subprocess
import re
import collections
import sys
import json
import typing
import statistics
import itertools


COMPILE_COMMANDS_FILE_NAME = 'compile_commands.json'


class CompileCommand:
    def __init__(self, directory: str, command: str):
        self.directory = directory
        self.command = command

    def get_relative_includes(self) -> typing.Iterable[str]:
        # shitty comamndline parser... beware
        commands = self.command.split(' ')
        for c in commands:
            if c.startswith('-I'):
                yield c[2:]


def load_compile_commands(path: str) -> typing.Dict[str, CompileCommand]:
    with open(path, 'r') as handle:
        store = json.load(handle)

        r = {}
        for entry in store:
            r[entry['file']] = CompileCommand(entry['directory'], entry['command'])

        return r


def find_build_root(root):
    """
    Find the build folder containing the compile_commands file or None
    """
    for relative_build in ['build', 'build/debug-clang']:
        build = os.path.join(root, relative_build)
        compile_commands_json = os.path.join(build, COMPILE_COMMANDS_FILE_NAME)
        if os.path.isfile(compile_commands_json):
            return build

    return None


def add_argument(sub):
    sub.add_argument('--compile-commands', dest='cc', default=None, help="the path to compile commands")


def get_argument_or_none(args) -> typing.Optional[str]:
    if args.cc is None:
        build = find_build_root(os.getcwd())
        if build is None:
            return None
        else:
            return os.path.join(build, COMPILE_COMMANDS_FILE_NAME)
    return args.cc

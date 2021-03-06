#!/usr/bin/env python3

"""
clang-tidy and clang-format related tools for the euphoria project
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
from timeit import default_timer as timer
import compile_commands as cc


HEADER_SIZE = 65
HEADER_SPACING = 1
HEADER_START = 3

HEADER_FILES = ['.h', '.hpp', '.hxx']
SOURCE_FILES = ['.cc', '.cpp', '.cxx', '.inl']

CLANG_TIDY_WARNING_CLASS = re.compile(r'\[(\w+([-,]\w+)+)\]')


def file_exist(file: str) -> bool:
    return os.path.isfile(file)


def get_file_data(file_name, missing_file):
    if file_exist(file_name):
        with open(file_name, 'r') as f:
            return json.loads(f.read())
    else:
        return missing_file


def set_file_data(file_name, data):
    with open(file_name, 'w') as f:
        print(json.dumps(data, sort_keys=True, indent=4), file=f)



def print_header(project_name, header_character='-'):
    """
    print a "pretty" header to the terminal
    """
    project = ' ' * HEADER_SPACING + project_name + ' ' * HEADER_SPACING
    start = header_character*HEADER_START
    left = HEADER_SIZE - (len(project) + HEADER_START)
    right = header_character*(left) if left > 1 else ''
    print(header_character * HEADER_SIZE)
    print(start+project+right)
    print(header_character * HEADER_SIZE)


def list_files_in_folder(path, extensions):
    for root, directories, files in os.walk(path):
        for file in files:
            ext = os.path.splitext(file)[1]
            if extensions is None or ext in extensions:
                yield os.path.join(root, file)


def is_file_ignored(path):
    with open(path, 'r') as file_handle:
        for line in file_handle:
            return line.startswith('// clang-tidy: ignore')
    return False


def multisort(xs, specs):
    for key, reverse in reversed(specs):
        xs.sort(key=key, reverse=reverse)
    return xs


def sort_and_map_files(root, iterator_files):
    ret = {}
    get_filename = lambda x: os.path.splitext(x)[0]
    get_ext = lambda x: os.path.splitext(x)[1]
    files = multisort(list(iterator_files), ((get_filename, False), (get_ext, True)))
    for file in files:
        rel = os.path.relpath(file, root)
        # ignore external folder
        # ignore build folder
        if rel.startswith('external') or rel.startswith('build'):
            pass
        elif not is_file_ignored(file):
            cat, f = os.path.split(rel)
            if cat in ret:
                ret[cat].append(file)
            else:
                ret[cat] = [file]
    return ret


def extract_data_from_root(root, files):
    return sort_and_map_files(root, list_files_in_folder(root, files))

def clang_tidy_root(root):
    return os.path.join(root, 'clang-tidy')


def clang_tidy_lines(root):
    """
    return a iterator over the the "compiled" .clang-tidy lines
    """
    with open(clang_tidy_root(root), 'r') as clang_tidy_file:
        write = False
        checks = []
        for line in clang_tidy_file:
            if write:
                yield line.rstrip()
            else:
                stripped_line = line.strip()
                if stripped_line == '':
                    pass
                elif stripped_line[0] == '#':
                    pass
                elif stripped_line == 'END_CHECKS':
                    write = True
                    checks_value = ','.join(checks)
                    yield 'Checks: "{}"'.format(checks_value)
                else:
                    checks.append(stripped_line)


def print_clang_tidy_source(root, clang_tidy_file):
    """
    print the clang-tidy "source"
    """
    for line in clang_tidy_lines(root):
        print(line, file=clang_tidy_file)


def make_clang_tidy(root):
    """
    write the .clang-tidy from the clang-tidy "source"
    """
    with open(os.path.join(root, '.clang-tidy'), 'w') as clang_tidy_file:
        print_clang_tidy_source(root, clang_tidy_file)


def path_to_output_store(build_folder):
    return os.path.join(build_folder, 'clang-tidy-store.json')


def get_store(build_folder):
    return get_file_data(path_to_output_store(build_folder), {})


def get_last_modification(input_files: typing.List[str]):
    sourcemod = 0
    for path in input_files:
        sourcemod = max(sourcemod, os.path.getmtime(path))
    return sourcemod


def is_all_up_to_date(input_files: typing.List[str], output) -> bool:
    sourcemod = get_last_modification(input_files)

    destmod = 0
    if output is not None:
        destmod = max(destmod, output)

    return sourcemod <= destmod


def get(dictionary, key):
    if key in dictionary:
        return dictionary[key]
    return None


def get_existing_output(root, project_build_folder, source_file):
    store = get_store(project_build_folder)
    root_file = clang_tidy_root(root)
    if source_file in store:
        stored = store[source_file]
        if is_all_up_to_date([root_file, source_file], stored['time']):
            return stored['output'], get(stored, 'time_took') or 0.0

    return None, 0.0


def set_existing_output(root, project_build_folder, source_file, existing_output, time):
    store = get_store(project_build_folder)
    root_file = clang_tidy_root(root)
    data = {}
    data['time'] = get_last_modification([root_file, source_file])
    data['output'] = existing_output
    data['time_took'] = time
    store[source_file] = data
    set_file_data(path_to_output_store(project_build_folder), store)




def call_clang_tidy(root, project_build_folder, source_file, name_printer, fix):
    """
    runs clang-tidy and returns all the text output
    """
    existing_output, took = get_existing_output(root, project_build_folder, source_file)
    if existing_output is not None:
        return existing_output, took
    command = ['clang-tidy', '-p', project_build_folder]
    if fix:
        command.append('--fix')
    command.append(source_file)


    try:
        name_printer.print_name()
        start = timer()
        output = subprocess.check_output(command, universal_newlines=True,
                                    encoding='utf8', stderr=subprocess.STDOUT)
        end = timer()
        took = end - start
        set_existing_output(root, project_build_folder, source_file, output, took)
        return output, took
    except subprocess.CalledProcessError as err:
        print(err.returncode)
        if err.output is not None:
            print(err.output)
        sys.exit(err.returncode)


def total(counter):
    """
    returns the total number of items in a counter
    """
    return sum(counter.values())


class FileStatistics:
    def __init__(self):
        self.data = {}

    def add(self, file, time):
        self.data[file] = time

    def print_data(self):
        if len(self.data) != 0:
            average_value = statistics.mean(self.data.values())
            min_name = min(self.data, key=lambda key: self.data[key])
            max_name = max(self.data, key=lambda key: self.data[key])
            print(f'average: {average_value:.2f}s')
            print(f'max: {self.data[max_name]:.2f}s for {max_name}')
            print(f'min: {self.data[min_name]:.2f}s for {min_name}')
            print(f'{len(self.data)} files')


def run_clang_tidy(root, source_file, project_build_folder, stats, short, name_printer, fix, printable_file, only):
    """
    runs the clang-tidy process, printing status to terminal
    """
    output, time_taken = call_clang_tidy(root, project_build_folder, source_file, name_printer, fix)
    warnings = collections.Counter()
    classes = collections.Counter()
    if not short and len(only) == 0:
        name_printer.print_name()
        print(f'took {time_taken:.2f}s')
    stats.add(printable_file, time_taken)
    print_empty = False
    hidden = len(only) > 0
    for line in output.split('\n'):
        if 'warnings generated' in line:
            pass
        elif 'Use -header-filter=.* to display errors' in line:
            pass
        elif 'Suppressed' in line and 'NOLINT).' in line:
            pass
        elif 'Suppressed' in line and 'non-user code' in line:
            pass
        else:
            if 'warning: ' in line:
                warnings[printable_file] += 1
                tidy_class = CLANG_TIDY_WARNING_CLASS.search(line)
                if tidy_class is not None:
                    warning_classes = tidy_class.group(1)
                    for warning_class in warning_classes.split(','):
                        classes[warning_class] += 1
                        hidden = len(only) > 0
                        if warning_class in only:
                            hidden = False
            if line.strip() == '':
                if not hidden and print_empty:
                    print()
                    print_empty = False
            else:
                if not hidden:
                    print_empty = True
                    print(line)
    # print('{} warnings.'.format(total(warnings)))
    if not short and len(only) == 0:
        print_warning_counter(classes, printable_file)
        print()
    return warnings, classes


def print_warning_counter(project_counter, project):
    """
    print warning counter to the console
    """
    print('{} warnings in {}.'.format(total(project_counter), project))
    for file, count in project_counter.most_common(10):
        print('{} at {}'.format(file, count))


##############################################################################
##############################################################################

def handle_list(args):
    root = os.getcwd()

    project_build_folder = cc.find_build_root(root)
    if project_build_folder is None:
        print('unable to find build folder')
        return

    files = list_files_in_folder(root, SOURCE_FILES)

    if args.sort:
        sorted = sort_and_map_files(root, files)
        for project, source_files in sorted.items():
            print_header(project)
            for source_file in source_files:
                print(source_file)
            print()
    else:
        for file in files:
            print(file)


def handle_format(args):
    """
    callback function called when running clang.py format
    """
    root = os.getcwd()

    project_build_folder = cc.find_build_root(root)
    if project_build_folder is None:
        print('unable to find build folder')
        return

    data = extract_data_from_root(root, SOURCE_FILES + HEADER_FILES)

    for project, source_files in data.items():
        print_header(project)
        for source_file in source_files:
            print(os.path.basename(source_file), flush=True)
            if args.nop is False:
                subprocess.call(['clang-format', '-i', source_file])
        print()


def handle_make_tidy(args):
    """
    callback function called when running clang.py make
    """
    root = os.getcwd()
    if args.nop:
        print_clang_tidy_source(root, sys.stdout)
    else:
        make_clang_tidy(root)


class NamePrinter:
    def __init__(self, name):
        self.name = name
        self.printed = False

    def print_name(self):
        if not self.printed:
            print(self.name, flush=True)
            self.printed = True


def filter_out_file(filters, file):
    if filters is not None:
        return all(f not in file for f in filters)
    return False


def handle_tidy(args):
    """
    callback function called when running clang.py tidy
    """
    root = os.getcwd()
    project_build_folder = cc.find_build_root(root)
    if project_build_folder is None:
        print('unable to find build folder')
        return

    make_clang_tidy(root)

    total_counter = collections.Counter()
    total_classes = collections.Counter()
    warnings_per_file = {}

    data = extract_data_from_root(root, SOURCE_FILES + HEADER_FILES if args.headers else SOURCE_FILES)
    stats = FileStatistics()

    try:
        for project, source_files in data.items():
            first_file = True
            project_counter = collections.Counter()
            # source_files = list_source_files(root, project)
            for source_file in source_files:
                printable_file = os.path.relpath(source_file, root)
                if filter_out_file(args.filter, source_file):
                    continue
                print_name = NamePrinter(printable_file)
                if first_file:
                    if not args.short:
                        print_header(project)
                    first_file = False
                if args.nop is False:
                    warnings, classes = run_clang_tidy(root, source_file, project_build_folder, stats, args.short, print_name, args.fix, printable_file, args.only)
                    if args.short and len(warnings) > 0:
                        break
                    project_counter.update(warnings)
                    total_counter.update(warnings)
                    total_classes.update(classes)
                    for k in classes.keys():
                        if k in warnings_per_file:
                            warnings_per_file[k].append(printable_file)
                        else:
                            warnings_per_file[k] = [printable_file]
                else:
                    print_name.print_name()

            if not first_file and not args.short:
                if len(args.only) == 0:
                    print_warning_counter(project_counter, project)
                    print()
                    print()
    except KeyboardInterrupt:
        if not args.short:
            print()
        print()

    if not args.short and len(args.only) == 0:
        print_header('TIDY REPORT')
        print_warning_counter(total_counter, 'total')
        print()
        print_warning_counter(total_classes, 'classes')
        print()
        print('-' * 80)
        print()
        for k,v in warnings_per_file.items():
            print(f'{k}:')
            for f in v:
                print(f'  {f}')
            print()

        print('-' * 80)
        print()
        stats.print_data()

    if len(total_counter) > 0:
        sys.exit(-1)
    else:
        sys.exit(0)


##############################################################################


def main():
    """
    entry point function for running the clang.py script
    """
    parser = argparse.ArgumentParser(description='do clang stuff')
    sub_parsers = parser.add_subparsers(dest='command_name', title='Commands',
                                        help='', metavar='<command>')

    sub = sub_parsers.add_parser('make', help='make .clang-tidy')
    sub.add_argument('--nop', action='store_true', help="don't write anything")
    sub.set_defaults(func=handle_make_tidy)

    sub = sub_parsers.add_parser('tidy', help='do clang tidy on files')
    sub.add_argument('--nop', action='store_true', help="don't do anything")
    sub.add_argument('--fix', action='store_true', help="try to fix the source")
    sub.add_argument('filter', default=[], nargs='+')
    sub.add_argument('--short', action='store_true', help="use shorter and stop after one file")
    sub.add_argument('--list', action='store_true', help="also list files in the summary")
    sub.add_argument('--no-headers', dest='headers', action='store_false', help="don't tidy headers")
    sub.add_argument('--only', nargs='*', default=[])
    sub.set_defaults(func=handle_tidy)

    sub = sub_parsers.add_parser('format', help='do clang format on files')
    sub.add_argument('--nop', action='store_true', help="don't do anything")
    sub.set_defaults(func=handle_format)

    sub = sub_parsers.add_parser('ls', help='list files')
    sub.add_argument('--new', action='store_true', help="use new lister")
    sub.add_argument('--sort', action='store_true', help="sort listing")
    sub.set_defaults(func=handle_list)

    args = parser.parse_args()
    if args.command_name is not None:
        args.func(args)
    else:
        parser.print_help()


##############################################################################

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

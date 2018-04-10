# !/usr/bin/python3

import os
import re
import subprocess
import sys
import buildtools.core as core
import buildtools.args as args
import typing


def get_vs_root(compiler: args.Compiler):
    # warn if default value?
    # todo: determine path based on compiler
    vs_root = r'C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE'
    if core.is_windows():
        import winreg
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, 'SOFTWARE\\Microsoft\\VisualStudio\\14.0') as st:
            val = winreg.QueryValueEx(st, 'InstallDir')
            vs = val[0]
            vs_root = vs
            print('Got path from registry')
        print("This is the vs solution path...", vs_root)
        core.flush()

    return vs_root


def visual_studio_generator(compiler: args.Compiler, platform: args.Platform) -> str:
    if compiler == args.Compiler.VS2015:
        if args.is_64bit(platform):
            return 'Visual Studio 14 2015 Win64'
        else:
            return 'Visual Studio 14 2015'
    elif compiler == args.Compiler.VS2017:
        if args.is_64bit(platform):
            return 'Visual Studio 15 Win64'
        else:
            return 'Visual Studio 15'
    else:
        raise Exception('Invalid compiler')


def list_projects_in_solution(path: str) -> typing.List[str]:
    ret = []
    dir = os.path.dirname(path)
    pl = re.compile(r'Project\("[^"]+"\) = "[^"]+", "([^"]+)"')
    with open(path) as sln:
        for line in sln:
            # Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "richtext", "wx_richtext.vcxproj", "{7FB0902D-8579-5DCE-B883-DAF66A885005}"
            m = pl.match(line)
            if m:
                subfolder = m.group(1)
                if not core.is_windows():
                    subfolder = subfolder.replace('\\', '/')
                print(subfolder)
                ret.append(os.path.join(dir, subfolder))
    return ret


def add_definition_to_project(path: str, define: str):
    # <PreprocessorDefinitions>WIN32;_LIB;_CRT_SECURE_NO_DEPRECATE=1;_CRT_NON_CONFORMING_SWPRINTFS=1;_SCL_SECURE_NO_WARNINGS=1;__WXMSW__;NDEBUG;_UNICODE;WXBUILDING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    pp = re.compile(r'([ ]*<PreprocessorDefinitions>)([^<]*</PreprocessorDefinitions>)')
    lines = []
    with open(path) as project:
        for line in project:
            m = pp.match(line)
            if m:
                lines.append('{0}{1};{2}'.format(m.group(1), define, m.group(2)))
            else:
                lines.append(line.rstrip())
    with open(path, mode='w') as project:
        for line in lines:
            project.write(line + '\n')



# change from:
# <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary> to <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>
# <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary> to <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
def change_to_static_link(path: str):
    mtdebug = re.compile(r'([ ]*)<RuntimeLibrary>MultiThreadedDebugDLL')
    mtrelease = re.compile(r'([ ]*)<RuntimeLibrary>MultiThreadedDLL')
    lines = []
    with open(path) as project:
        for line in project:
            mdebug = mtdebug.match(line)
            mrelease = mtrelease.match(line)
            if mdebug:
                print('in {project} changed to static debug'.format(project=path))
                lines.append(
                    '{spaces}<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>'.format(spaces=mdebug.group(1)))
            elif mrelease:
                print('in {project} changed to static release'.format(project=path))
                lines.append('{spaces}<RuntimeLibrary>MultiThreaded</RuntimeLibrary>'.format(spaces=mrelease.group(1)))
            else:
                lines.append(line.rstrip())
    with open(path, mode='w') as project:
        for line in lines:
            project.write(line + '\n')


def change_all_projects_to_static(sln: str):
    projects = list_projects_in_solution(sln)
    for p in projects:
        change_to_static_link(p)


def add_definition_to_solution(sln: str, definition: str):
    projects = list_projects_in_solution(sln)
    for p in projects:
        add_definition_to_project(p, definition)


def make_single_project_64(project_path: str, rep: core.TextReplacer):
    if not os.path.isfile(project_path):
        print('missing ' + project_path)
        return
    lines = []
    with open(project_path) as project:
        for line in project:
            nl = rep.replace(line.rstrip())
            lines.append(nl)
    with open(project_path, 'w') as project:
        for line in lines:
            project.write(line + '\n')


def make_projects_64(sln: str):
    projects = list_projects_in_solution(sln)
    rep = core.TextReplacer()
    rep.add('Win32', 'x64')
    rep.add('<DebugInformationFormat>EditAndContinue</DebugInformationFormat>',
            '<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>')
    rep.add('<TargetMachine>MachineX86</TargetMachine>', '<TargetMachine>MachineX64</TargetMachine>')
    # protobuf specific hack since cmake looks in x64 folder
    rep.add('<OutDir>Release\</OutDir>', '<OutDir>x64\Release\</OutDir>')
    rep.add('<OutDir>Debug\</OutDir>', '<OutDir>x64\Debug\</OutDir>')
    for project in projects:
        make_single_project_64(project, rep)


def make_solution_64(sln: str):
    rep = core.TextReplacer()
    rep.add('Win32', 'x64')

    lines = []
    with open(sln) as slnlines:
        for line in slnlines:
            nl = rep.replace(line.rstrip())
            lines.append(nl)

    with open(sln, 'w') as f:
        for line in lines:
            f.write(line + '\n')


def convert_sln_to_64(sln: str):
    make_solution_64(sln)
    make_projects_64(sln)


def upgrade_sln(proto_sln: str, compiler: args.Compiler):
    devenv = os.path.join(get_vs_root(compiler), 'devenv.exe')
    print(devenv)
    print(proto_sln)
    if core.is_windows():
        core.flush()
        subprocess.check_call([devenv, proto_sln, '/upgrade'])


def msbuild(sln: str, compiler: args.Compiler, platform: args.Platform, libraries: typing.Optional[typing.List[str]]):
    msbuild_cmd = ['msbuild']
    if libraries is not None:
        msbuild_cmd.append('/t:' + ';'.join(libraries))
    msbuild_cmd.append('/p:Configuration=Release')
    # https://blogs.msdn.microsoft.com/vcblog/2016/02/24/stuck-on-an-older-toolset-version-move-to-visual-studio-2015-without-upgrading-your-toolset/
    # https://stackoverflow.com/questions/33380128/visual-studio-2015-command-line-retarget-solution
    msbuild_cmd.append('/p:PlatformToolset=' + args.get_msbuild_toolset(compiler))
    msbuild_cmd.append('/p:Platform=' + args.platform_as_string(platform))
    msbuild_cmd.append(sln)
    if core.is_windows():
        core.flush()
        subprocess.check_call(msbuild_cmd)
    else:
        print(msbuild_cmd)

# !/usr/bin/python3

import sys
import os
import subprocess
import shutil

import buildtools.cmake as cmake
import buildtools.core as core
import buildtools.visualstudio as visualstudio


def install_dependency_wx(install_dist: str, wx_root: str, build: bool):
    print('Installing dependency wxWidgets')
    wx_url = "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0.zip"
    wx_zip = os.path.join(install_dist, "wx.zip")
    wx_sln = os.path.join(wx_root, 'build', 'msw', 'wx_vc14.sln')
    core.verify_dir_exist(install_dist)
    if not core.dir_exist(wx_root):
        core.verify_dir_exist(wx_root)
        print("downloading wx...")
        core.download_file(wx_url, os.path.join(install_dist, wx_zip))
        print("extracting wx")
        core.extract_zip(wx_zip, wx_root)
        print("changing wx to static")
        visualstudio.change_all_projects_to_static(wx_sln)

        print("building wxwidgets")
        print("----------------------------------")

        if build:
            wx_msbuild_cmd = ['msbuild', '/p:Configuration=Release', '/p:Platform='+core.platform_as_string(), core.appveyor_msbuild(), wx_sln]
            if core.is_windows():
                core.flush()
                subprocess.check_call(wx_msbuild_cmd)


def install_dependency_proto(install_dist: str, proto_root: str, build: bool, vs_root: str):
    print('Installing dependency protobuf')
    proto_url = "https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.zip"
    proto_zip = os.path.join(install_dist, 'proto.zip')
    proto_sln = os.path.join(proto_root, 'vsprojects', 'protobuf.sln')
    core.verify_dir_exist(install_dist)
    if not core.dir_exist(proto_root):
        core.verify_dir_exist(proto_root)
        print("downloding proto...")
        core.download_file(proto_url, os.path.join(install_dist, proto_zip))
        print("extracting proto")
        core.extract_zip(proto_zip, proto_root)
        print('moving proto files from subfolder to root')
        proto_root_root = os.path.join(proto_root, 'protobuf-2.6.1')
        core.movefiles(proto_root_root, proto_root)
        print("upgrading protobuf")
        print("-----------------------------------")
        devenv = os.path.join(vs_root, 'devenv.exe')
        if build:
            if core.is_windows():
                core.flush()
                subprocess.check_call([devenv, proto_sln, '/upgrade'])
        visualstudio.add_definition_to_solution(proto_sln, '_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS')

        print("changing proto to static")
        visualstudio.change_all_projects_to_static(proto_sln)

        if core.is_platform_64bit():
            print('64 bit build, hacking proto to 64 bit')
            visualstudio.convert_sln_to_64(proto_sln)

        print("building protobuf")
        print("-----------------------------------")
        if build:
            proto_msbuild_cmd = ['msbuild', '/t:libprotobuf;protoc', '/p:Configuration=Release', '/p:Platform='+core.platform_as_string(), proto_sln]
            if core.is_windows():
                core.flush()
                subprocess.check_call(proto_msbuild_cmd)


def install_dependency_sdl2(deps, root, build):
    print('Installing dependency sdl2')
    url = "https://www.libsdl.org/release/SDL2-2.0.5.zip"
    zip = os.path.join(deps, 'sdl2.zip')
    if core.dir_exist(root):
        print('removing sdl2 root')
        shutil.rmtree(root)
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading sdl2')
        core.download_file(url, zip)
        core.extract_zip(zip, root)
        core.movefiles(os.path.join(root, 'SDL2-2.0.5'), root)
        project = cmake.CMake(build_folder=build, source_folder=root)
        project.config()
        project.build()

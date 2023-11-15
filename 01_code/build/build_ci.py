#!/usr/bin/env python3
# 编译前处理路径问题

import os
import sys
import argparse
import json
from lxml import etree as etree


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--workspace", help="workspace路径 默认从环境变量VITIS_WORKSPACE中获取 若此处指定 则覆盖环境变量的配置")
    parser.add_argument("--xsa", help="xsa文件位置")

    args = parser.parse_args()
    return args


def set_xsa(file, xsa):
    with open(file, "r") as f:
        data = json.load(f)
    data['platHandOff'] = xsa
    data['platIntHandOff'] = f"<platformDir>/hw/{xsa.split('/')[-1]}"
    with open(file, "w") as f:
        f.write(json.dumps(data))


def set_location(file, workspace):
    et = etree.parse(file)
    root = et.getroot()
    root.attrib['location'] = f'{workspace}/{file.rsplit("/", 2)[1]}'

    et.write(file, encoding="UTF-8")


def set_platform(file: str, workspace):
    et = etree.parse(file)
    root = et.getroot()
    root.attrib['platform'] = f'{workspace}/shrd100_plt/export/shrd100_plt/shrd100_plt.xpfm'
    et.write(file, encoding="UTF-8")


def main(location, workspace):
    # 修改platform.spr中的文件位置
    print("location:", location)
    set_xsa(f"{location}/shrd100_plt/platform.spr", args.xsa)

    # 修改shrd100_app.prj中的文件位置
    set_platform(f"{location}/shrd100_app/shrd100_app.prj", workspace)
    set_location(f"{location}/shrd100_app/shrd100_app.prj", workspace)

    set_platform(f"{location}/shrd100_app_system/shrd100_app_system.sprj", workspace)

    set_platform(f"{location}/det_alg/det_alg.prj", workspace)
    set_location(f"{location}/det_alg/det_alg.prj", workspace)


if __name__ == "__main__":
    args = parse_args()
    location = os.path.realpath(os.path.dirname(os.path.dirname(sys.argv[0])))
    if args.workspace:
        _workspace = args.workspace
    else:
        _workspace = os.environ.get("VITIS_WORKSPACE", "")
    if not args.xsa:
        print("请指定参数--xsa指定xsa文件位置")
        sys.exit(1)
    if _workspace:
        main(location, _workspace)
    else:
        print("请指定参数--workspace,或设置环境变量VITIS_WORKSPACE")
        sys.exit(1)

#/bin/python
import os
import sys
import json
import fileinput

#定义platform.sp文件路径
PLATFORM_SPR = sys.argv[1]
#定义qemuData路径
QEMUDATA_DIR = sys.argv[2]
#定义platHandOff值
XSA = sys.argv[3]
#定义platIntHandOff值
PLATINTBANDOFF = sys.argv[4]

#获取latform.spr文件中qemuData和XSA的值
def get_qemuData(platform_spr):
    qemu_data = ''
    with open(platform_spr) as f:
        json_obj = json.load(f)
        _xsa = json_obj['platHandOff']
        _platIntHandOff = json_obj['platIntHandOff']
        systems_domains = json_obj['systems'][0]['domains']
        for domain in systems_domains:
            if 'qemuArgs' in domain:
                _qemu_data = domain['qemuData']
    f.close()
    return _qemu_data,_xsa,_platIntHandOff

#将platform.spr文件中qemuArgs、qemuData、pmuQemuArgs和xsa的路径替换成本地路径
def replace_vitis_dir(platform_spr,qemuData_dir,xsa,platIntHandOff):
    qemuData_original,xsa_original, platIntHandOff_original = get_qemuData(platform_spr)
    if  os.path.exists(platform_spr):
        for line in fileinput.input(platform_spr,backup='_bak',inplace=1):
            try:
                line=line.replace(qemuData_original,qemuData_dir)
                line=line.replace(xsa_original,xsa)
                line=line.replace(platIntHandOff_original,platIntHandOff)
                print(line,end="")
            except:
                print(platform_spr+'获取失败')
                sys.exit(1)
    else:
        print(platform_spr+"文件找不到")
        sys.exit(1)
replace_vitis_dir(PLATFORM_SPR,QEMUDATA_DIR,XSA,PLATINTBANDOFF)



















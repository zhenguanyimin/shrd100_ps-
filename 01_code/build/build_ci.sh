#!/bin/bash


SRC_WORKSPACE=$(dirname $(dirname $(realpath $0)))

pushd "${SRC_WORKSPACE}" || exit

# 以下两个应由环境变量给出
# VITIS_HOME 指向vitis的安装目录
# VITIS_HOME=
# VITIS_WORKSPACE 不应该指向源代码目录，建议指向一个空白目录，且自动构建后进行清理
# VITIS_WORKSPACE=

# TODO 检查python和lxml模块是否安装
PYTHON=python3

HEADER_FILE=${SRC_WORKSPACE}/build/header_info.ini

# 删除release目录下所有文件并重新生成
rm -rf build/release*
mkdir -p build/release

COMPILE_DATE=$(date "+%Y%m%d%H%M%S")

# droneid --------------------------------------
# droneid版本号
VERSION=SHRD101R-$1-$COMPILE_DATE

rm -rf "${VITIS_WORKSPACE}"
# 指定XSA文件名
XSA_FILE=${SRC_WORKSPACE}/hw_spec/droneid_PL_0A030023/droneid_PL_0A030023.xsa
# 指定bit文件名
BIT_FILE=${SRC_WORKSPACE}/hw_spec/droneid_PL_0A030023/droneid_PL_0A030023.bit

# 编译前预处理，修改各个配置文件中目录的位置
$PYTHON build/build_ci.py --workspace="${VITIS_WORKSPACE}" --xsa="${XSA_FILE}"
rm "${SRC_WORKSPACE}"/shrd100_plt/hw/*.xsa
cp "${XSA_FILE}" "${SRC_WORKSPACE}"/shrd100_plt/hw/droneid_PL_0A030023.xsa
rm "${SRC_WORKSPACE}"/shrd100_plt/hw/*.bit
cp "${BIT_FILE}" "${SRC_WORKSPACE}"/shrd100_plt/hw/droneid_PL_0A030023.bit

BIF_FILE=build/release/shrd100_app_system.bif
echo the_ROM_image:  > ${BIF_FILE}
{
    echo "{"
    echo "    [bootloader, destination_cpu=a53-0] ${VITIS_WORKSPACE}/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf"
    echo "    [destination_device=pl] ${BIT_FILE}"
    echo "    [destination_cpu=a53-0] ${VITIS_WORKSPACE}/shrd100_app/Debug/shrd100_app.elf"
    echo "}"
} >> ${BIF_FILE}

xsct build/build_ci_droneid.tcl "$VERSION" || exit 1
bootgen -image ${BIF_FILE} -o build/release/"${VERSION}".bin -arch zynqmp -w || exit 1
$PYTHON build/build_shrd100_OTA.py "${HEADER_FILE}" build/release/"${VERSION}".bin "${VERSION}"

cp "${VITIS_WORKSPACE}"/shrd100_app/Debug/shrd100_app.elf build/release
cp "${VITIS_WORKSPACE}"/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf build/release
mv build/release build/release_droneid

# ----------------------------------------------


# spectrum -------------------------------------
# spectrum版本号
VERSION=SHRD102R-$1-$COMPILE_DATE

rm -rf "${VITIS_WORKSPACE}"
mkdir -p build/release
# 指定XSA文件名
XSA_FILE=${SRC_WORKSPACE}/hw_spec/droneid_PL_0B030014/droneid_PL_0B030014.xsa
# 指定bit文件名
BIT_FILE=${SRC_WORKSPACE}/hw_spec/droneid_PL_0B030014/droneid_PL_0B030014.bit

# 编译前预处理，修改各个配置文件中目录的位置
$PYTHON build/build_ci.py --workspace="${VITIS_WORKSPACE}" --xsa="${XSA_FILE}"
rm "${SRC_WORKSPACE}"/shrd100_plt/hw/*.xsa
cp "${XSA_FILE}" "${SRC_WORKSPACE}"/shrd100_plt/hw/droneid_PL_0B030014.xsa
rm "${SRC_WORKSPACE}"/shrd100_plt/hw/*.bit
cp "${BIT_FILE}" "${SRC_WORKSPACE}"/shrd100_plt/hw/droneid_PL_0B030014.bit

BIF_FILE=build/release/shrd100_app_system.bif
echo the_ROM_image:  > ${BIF_FILE}
{
    echo "{"
    echo "    [bootloader, destination_cpu=a53-0] ${VITIS_WORKSPACE}/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf"
    echo "    [destination_device=pl] ${BIT_FILE}"
    echo "    [destination_cpu=a53-0] ${VITIS_WORKSPACE}/shrd100_app/Debug/shrd100_app.elf"
    echo "    [destination_cpu=a53-1] ${VITIS_WORKSPACE}/det_alg/Debug/det_alg.elf"
    echo "}"
} >> ${BIF_FILE}

xsct build/build_ci_spectrum.tcl "$VERSION" || exit 1
bootgen -image ${BIF_FILE} -o build/release/"${VERSION}".bin -arch zynqmp -w || exit 1
$PYTHON build/build_shrd100_OTA.py "${HEADER_FILE}" build/release/"${VERSION}".bin "${VERSION}"

cp "${VITIS_WORKSPACE}"/shrd100_app/Debug/shrd100_app.elf build/release
cp "${VITIS_WORKSPACE}"/det_alg/Debug/det_alg.elf build/release
cp "${VITIS_WORKSPACE}"/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf build/release
mv build/release build/release_spectrum

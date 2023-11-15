#/bin/python
import os
import sys
import binascii

#定义header_info文件路径
HEADER_FILE_INI = sys.argv[1]
IMAGE_FILE = sys.argv[2]
IMAGE_VERSION = sys.argv[3]

global image_size
global image_crc
global header_info
global header_bin

def calculate_bin_crc32(file):

	with open(file, 'rb') as f:
		file_bytes = f.read()
	f.close()

	# 以bytes格式计算crc32
	crc = hex(binascii.crc32(file_bytes))

	# 计算结果不足四字节时高位需要补零，确保返回值为0xXXXXXXXX格式
	zreo_fill_len = 10 - len(crc)
	for i in range(zreo_fill_len):
		crc = crc[0:2] + '0' + crc[2:(len(crc)+ 1)]

	return crc


# 获取镜像文件长度并计算CRC校验值
def get_image_parameter(file):
	global image_size
	global image_crc

	image_size = hex(int(os.path.getsize(file)))

	if len(image_size) == 8:
		image_size = image_size[0:2] + "00" + image_size[2:9]
	elif len(image_size) == 9:
		image_size = image_size[0:2] + "0" + image_size[2:9]

	image_crc = calculate_bin_crc32(file)


# 头部信息中字符串转化为十六进制数格式, 保留长度以00填充
def str_to_bytes(str, totalLen):
	byte_str = b''

	byte_str = str.encode('utf-8')

	fillLen = totalLen - len(str)
	for i in range(fillLen):
		byte_str += b'\xFF'

	return byte_str


def hexstr_to_bytes(hexstr):
	list = b''

	for i in range(8, 0, -2):
		tmp_str = '0X0' + hexstr[i: i+2]
		tmp_int = int(tmp_str, 16)
		list += bytes([tmp_int])

	return list

# 生成头部信息bin文件
def generator_header_bin(file):
	header_bin_size = 4*1024
	write_str = b''

	# 生成一个4K大小的文件
	lfile = open(file, 'w')
	# 根据文件大小，偏移文件写入位置；位置要减掉一个字节，因为后面要写入一个字节的数据
	lfile.seek(header_bin_size - 1)
	# 然后在当前位置写入任何内容，必须要写入
	lfile.write(' ')    # lfile.write('')不会写入任何内容
	lfile.close()

	# 解析头部信息并写入bin文件
	for key in header_info:
		if key == "reserveLen" or key == "headerCrc":
			pass
		elif key == "pCorpInfo" or key == "pDevName":
			list = str_to_bytes(header_info[key], 32)
			write_str = write_str + list
		elif key == "pFwVersion":
			list = str_to_bytes(header_info[key], 64)
			write_str = write_str + list
		else:
			hex_str = header_info[key]
			list = hexstr_to_bytes(hex_str)
			write_str = write_str + list

	# '\xFF'填充头部信息reserve部分
	res_len = int(header_info["reserveLen"][8:10], 16)
	for i in range(res_len):
		write_str +=  b"\xFF"

	with open (file, 'wb') as f:
		f.write(write_str)
	f.close()


# 读取header头文件基本参数
def get_header_info(file):
	global header_info
	global header_bin
	header_bin ='header_file.bin'
	final_size = 1024*4
	header_info = {}

	# 读取header_file.ini配置文件信息
	for line in open(file):
		tmp = line.strip().split(':')
		key, value = tmp[0], tmp[1]
		header_info[key] = value

	header_info["firmwareLen"] = image_size
	header_info["firmwareCrc"] = image_crc
	header_info["pFwVersion"] = IMAGE_VERSION

	generator_header_bin(header_bin)
	# 计算header_file.bin crc32值并写入
	crc = calculate_bin_crc32(header_bin)
	b_crc = hexstr_to_bytes(crc)
	with open (header_bin, 'ab+') as f:
		f.write(b_crc)
	f.close()

	# 头文件除前256bytes有效区域外以FF填充
	final_size = 4*1024 - 256
	fill_str = b''
	for i in range(final_size):
		fill_str += b'\xFF'

	with open (header_bin, 'ab+') as f:
		f.write(fill_str)
	f.close()

# 将生成的header.bin文件和image.bin合并为一个bin文件
def combine_OTA_file(file1, file2):
	global header_bin
	bytes_str = b''
	cur_dir = os.path.abspath('.') + '/'
	
	target_file = cur_dir + 'build/release/' + IMAGE_VERSION + '_OTA.bin'
	#target_file = cur_dir + "release/" + IMAGE_VERSION + '_OTA.bin'

	with open(file1, 'rb') as f:
		bytes_str = f.read()
	f.close()

	with open(file2, 'rb') as f:
		bytes_str += f.read()
	f.close()
	
	with open(target_file, 'wb') as f:
		f.write(bytes_str)
	f.close()
	# 删除临时文件
	os.remove(header_bin)

################################# main #################################
get_image_parameter(IMAGE_FILE)
get_header_info(HEADER_FILE_INI)
combine_OTA_file('header_file.bin' ,IMAGE_FILE)

/*
 * sd_hal.h
 *
 *  Created on: 2022��12��27��
 *      Author: A22745
 */

#ifndef HAL_SD_FF_HAL_H_
#define HAL_SD_FF_HAL_H_

#include "ff.h"

FRESULT FsMount();
FRESULT FsUmount();
FRESULT FsReadFile( FIL *file , uint8_t* FsReadBuf , uint32_t ReadLen , uint32_t *bw );
FRESULT FsWriteFile( FIL *file , const uint8_t* FsWriteBuf , uint32_t WriteLen , uint32_t *bw );
FRESULT FsUnlinkFile( TCHAR *DiskPath , char* file_name );
FRESULT FsSeekFile( FIL *file , FSIZE_t ofs );
FRESULT FsOpenFile( FIL *file ,  TCHAR *DiskPath , char* file_name , BYTE mode );
FRESULT FsRenameFile(TCHAR *DiskPath , char* old_name , char* new_name );
FRESULT FsClossFile( FIL *file );
FRESULT FsOpenDir( DIR *DirInf , char *DiskPath );
FRESULT FsClossDir( DIR *DirInf );
FRESULT FsReadDir( DIR *DirInf, FILINFO *FileInf );
FRESULT FsCreateFs( TCHAR *DiskPath );

#endif /* HAL_SD_FF_HAL_H_ */

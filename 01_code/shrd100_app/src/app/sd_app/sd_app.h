/*
 * sd_app.h
 *
 *  Created on: 2022年12月30日
 *      Author: A22745
 */

#ifndef SRC_APP_SD_APP_SD_BUF_H_
#define SRC_APP_SD_APP_SD_BUF_H_

extern char *DiskPath;
void SetC2SystemTime( char *buff );
void PacketLogListData( uint8_t cmd_source_id , uint8_t msgid);
void PacketLogData( uint32_t log_id , char *name_ptr , uint8_t cmd_source_id , uint8_t msgid );
void DeleteAllLogFile();

int32_t InitSdProcess_Task(void);

#endif /* SRC_APP_SD_APP_SD_APP_H_ */

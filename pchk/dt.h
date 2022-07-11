#pragma once
#include "stdint.h"
/**
 * @file	include/lcnrtl/dt.h
 * C-Funktionen zur Verwaltung eines Datentelegrammes. Öffentlicher Teil.
 * @author	Lothar May
 * @since	2005-10-17
 * @version	$Id: dt.h 1 2013-03-21 08:03:52Z TobiasJ $
 */

#ifndef INC_LCNRTL_DT_H_
#define INC_LCNRTL_DT_H_

 /**
  * Maximale Größe eines Datentelegrammes (Binärform, in Bytes).
  */

#define DT_MAX_SIZE			20

  /**
   * Minimale Größe eines Datentelegrammes (Binärform, in Bytes).
   */

#define DT_HEADER_SIZE		6
#define DT_MIN_SIZE			DT_HEADER_SIZE

#define BIT_MIRROR(_input) (((_input & 0x80) >> 7) | ((_input & 0x40) >> 5) | ((_input & 0x20) >> 3) | ((_input & 0x10) >> 1) | ((_input & 0x08) << 1) | ((_input & 0x04) << 3) | ((_input & 0x02) << 5) | ((_input & 0x01) << 7))



/**
 * Abstraktion von einem LCN-Datentelegramm.
 */

class DataTelegram {
public:
	DataTelegram();
	//virtual ~DataTelegram();

	enum DT_ErrorCode {
		DT_VALID = 0,
		DT_MORE_BYTES_NEEDED,
		DT_INVALID_SIZE,
		DT_INVALID_CHECKSUM,
		DT_INVALID_DATA,
		DT_NULL_POINTER,
		DT_BUFFER_TOO_SMALL,
		DT_PCK_DECODE_ERROR
	};
	//bool DtGetErrorCode();
	void DtInit();
	bool DtCreate(uint8_t sendModId, bool ack, bool targetIsGroup,
		uint8_t targetSegId, uint8_t targetModId, uint8_t command,
		char *data, uint8_t dataSize);
	bool DtSetData(char *buf, unsigned int bufSize);
	int _DT_ReadSizeFromHeader(char *src, unsigned int maxSize);
	static char _DT_ReadChecksumFromHeader(char *src, unsigned int maxSize);
	char _DT_CalcChecksum();
	char* DtGetData();

private:
	unsigned char m_size;
	char m_buf[DT_MAX_SIZE];

	
};



#endif

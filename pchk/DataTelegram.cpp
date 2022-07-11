#include "dt.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include <iostream>
#include <bitset>


void DataTelegram::DtInit() {
	m_size = 0;
}

DataTelegram::DataTelegram() {
	DtInit();
}


int DataTelegram::_DT_ReadSizeFromHeader(char *src, unsigned int maxSize) {
	if (maxSize >= 2)
	{
		switch ((src[1] & 0x0c) >> 2)
		{
		case 0:		return 6;
		case 1:		return 8;
		case 2:		return 12;
		case 3:		return 20;
		default:	return -1;
		}
	}
	return 0;
}

char DataTelegram::_DT_ReadChecksumFromHeader(char *src, unsigned int maxSize) {
	if (maxSize >= 3)
		std::cout << (int)src[2] << std::endl;
		std::bitset<8> x(src[2]);
		std::cout << x << std::endl;
		return src[2];
	return 0;
}

char DataTelegram::_DT_CalcChecksum() {
	unsigned short value = 0;
	uint8_t i;
	const uint8_t size = m_size;

	for (i = 0; i < size; i++)
	{
		if (i != 2)	// Das Prüfsummenbyte geht nicht mit in die Prüfsummenberechnung ein.
		{
			value += m_buf[i];
			//scanf((const char*)(UINT8)dt->m_buf[i]);
			value = ((value & 127) << 2) | ((value & 384) >> 7);	// 9bit Zahl um 2 nach links verschieben
			if (value > 0xFF)
				value = (value & 0xFF) + 1;		// wenn bit 9 gesetzt: +1
		}
	}
	//printf((char*)value);
	// value ist eine Nummer (char) davor wandelt value in ein Zeichen um
	std::cout << (int)value << std::endl;
	return (char)value;
}

// ToDo: Vllt erstmal CreateTelegram
// *buf ist ein pointer auf das element input[256]
bool DataTelegram::DtSetData(char *buf, unsigned int bufSize) {

	int tmpSize;

	// Datentelegramm zurücksetzen.
	m_size = 0;

	if (!buf)
		return DT_NULL_POINTER;
	// Auf minimale Größe prüfen.
	if (bufSize < DT_MIN_SIZE)
		return DT_MORE_BYTES_NEEDED;
	// Tatsächliche Größe aus dem Header auslesen.
	tmpSize = _DT_ReadSizeFromHeader(buf, bufSize);
	if (tmpSize <= 0)
		return DT_INVALID_SIZE;
	// Prüfen ob genug Daten vorliegen.
	if ((unsigned)tmpSize > bufSize)
		return DT_MORE_BYTES_NEEDED;
	memcpy(m_buf, buf, tmpSize);
	m_size = (uint8_t)tmpSize;
	
	// Prüfsumme überprüfen.
	if (_DT_ReadChecksumFromHeader(buf, bufSize) != _DT_CalcChecksum())
	{
		m_size = 0;
		std::cout << "Invalid Checksum" << std::endl;
		return DT_INVALID_CHECKSUM;
	}
	
	// Semantik des Telegramms überprüfen.
	/*
	if (!_DT_SemanticCheck(dt))
	{
		dt->m_size = 0;
		return DT_INVALID_DATA;
	}
	*/

	return DT_VALID;
}

bool DataTelegram::DtCreate(uint8_t sendModId, bool ack, bool targetIsGroup,
	uint8_t targetSegId, uint8_t targetModId, uint8_t command,
	char *data, uint8_t dataSize) {
	if (dataSize > DT_MAX_SIZE - DT_HEADER_SIZE)
	{
		m_size = 0;
		return DT_INVALID_SIZE;
	}

	m_buf[0] = BIT_MIRROR((char)sendModId);

	switch (dataSize)
	{
	case  0: m_buf[1] = 0;
		break;
	case  2: m_buf[1] = 0x04;
		break;
	case  6: m_buf[1] = 0x08;
		break;
	case 14: m_buf[1] = 0x0C;
		break;
	default:
		m_size = 0;
		return DT_INVALID_SIZE;
	}

	// InfoByte
	if (ack)
		m_buf[1] |= 0x01;
	if (targetIsGroup)
		m_buf[1] |= 0x02;
	if (targetSegId != 0)
		m_buf[1] |= 0x40;

	m_size = DT_HEADER_SIZE + dataSize;

	m_buf[3] = (char)targetSegId;
	m_buf[4] = (char)targetModId;
	m_buf[5] = (char)command;
	/*
	if (dataSize)
	{
		if (data)
			memcpy(&dt->m_buf[6], data, dataSize);
		else
			memset(&dt->m_buf[6], 0xFF, dataSize);
	}
	*/
	//dt->m_buf[2] = _DT_CalcChecksum(dt);
	/*
	if (!_DT_SemanticCheck(dt))
	{
		dt->m_size = 0;
		return DT_INVALID_DATA;
	}
	*/
	return DT_VALID;
}
 char* DataTelegram::DtGetData() {
	return m_buf;
}
 /*
 bool DataTelegram::DtGetErrorCode() {
	 return ;
 }
 */

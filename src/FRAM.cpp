#include <inttypes.h>
#include <stdlib.h>

#include <Arduino.h>
#include <SPI.h>

#include "FRAM.h"

#define WPEN    128
#define BP1     8
#define BP2     4
#define WEL     2

void sendAddress(uint32_t address);

/*
 * Each module on the MB85RS2MTA is 256Kbit in size, and there are eight
 * of them. This translates to (256Kbit / 8 * 1024) = 32768.
 */
const uint32_t moduleSizeInBytes = 32768;

FRAM::FRAM(int chipSelectPin, int writeProtectPin)
{
    _csPin = chipSelectPin;
    _wpPin = writeProtectPin;
}

FRAM::~FRAM()
{
    disableWrite();
    digitalWrite(_csPin, HIGH);
    digitalWrite(_wpPin, HIGH);
}

void
FRAM::begin()
{
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);

    pinMode(_wpPin, OUTPUT);
    digitalWrite(_wpPin, HIGH);

    disableWrite();
    _size = getSizeInBytes();
}

uint8_t
FRAM::getManufacturerID()
{
    beginTransaction();

    SPI.transfer(RDID);
    uint8_t mfgr = SPI.transfer(0x00);

    /* 
     * Now we need to consume the rest of the output. 
     * - 8-bit "continuation" value;
     * - 16-bit product ID value
     */
    SPI.transfer(0x00);
    SPI.transfer16(0x00);

    endTransaction();

    return mfgr;
}

uint8_t
FRAM::getChipDensity()
{
    beginTransaction();

    SPI.transfer(RDID);
    SPI.transfer16(0x00); /* pass up the mfgr and continuation byte */
    uint8_t density = SPI.transfer(0x00);
    SPI.transfer(0x00); /* consume the last byte of output */

    endTransaction();

    return density & 0x1f;
}

uint32_t
FRAM::getSizeInBytes()
{
    return moduleSizeInBytes * (uint32_t)getChipDensity();
}

uint8_t *
FRAM::read(uint32_t address, uint8_t *b)
{
    if (address > _size)
        return NULL;

    beginTransaction();
    SPI.transfer(READ);
    sendAddress(address);
    *b = SPI.transfer(0x00);
    endTransaction();
    return b;
}

uint8_t *
FRAM::read(uint32_t address, uint8_t *buf, size_t len)
{
    if ((address + len) > _size)
        return NULL;

    beginTransaction();
    SPI.transfer(READ);
    sendAddress(address);

    for (size_t i = 0; i < len; i++)
        SPI.transfer(buf, len);

    endTransaction();
    return buf;
}

bool
FRAM::write(uint32_t address, uint8_t b)
{
    if (address > _size)
        return false;

    enableWrite();
    beginTransaction();

    SPI.transfer(WRITE);
    sendAddress(address);
    SPI.transfer(b);

    endTransaction();
    disableWrite();
    return true;
}

bool
FRAM::write(uint32_t address, const uint8_t *buf, size_t len)
{
    if ((address + len) > _size)
        return false;

    enableWrite();
    beginTransaction();
    SPI.transfer(WRITE);
    sendAddress(address);

    for (size_t i = 0; i < len; i++)
        SPI.transfer(buf[i]);

    endTransaction();
    disableWrite();
    return true;
}

uint8_t
FRAM::readStatusRegister()
{
    beginTransaction();

    sendCommand(RDSR);
    uint8_t sr = SPI.transfer(0x00);

    endTransaction();
    return sr;
}

bool
FRAM::isStatusRegisterWriteProtected()
{
    uint8_t sr = readStatusRegister();
    return ((sr & WPEN) == 0);
}

void
FRAM::enableWrite()
{
    beginTransaction();
    sendCommand(WREN);
    endTransaction();
}

void
FRAM::disableWrite()
{
    beginTransaction();
    sendCommand(WRDI);
    endTransaction();
}

bool
FRAM::isWritable()
{
    uint8_t sr = readStatusRegister();
    return ((sr & WEL) == WEL);
}

void
FRAM::sendCommand(cmd_t command)
{
    SPI.transfer(command);
}

void
FRAM::beginTransaction()
{
    digitalWrite(_csPin, LOW);
    SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE0));
}

void
FRAM::endTransaction()
{
    SPI.endTransaction();
    digitalWrite(_csPin, HIGH);
}

void sendAddress(uint32_t address)
{
    uint8_t first, second, third;
    first = (address & 0xff0000) >> 16;
    second = (address & 0x00ff00) >> 8;
    third = address & 0x0000ff;
    SPI.transfer(first);
    SPI.transfer(second);
    SPI.transfer(third);
}

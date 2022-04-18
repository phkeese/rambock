#pragma once
#include "../memory_device.hpp"
#include <Arduino.h>
#include <SPI.h>

namespace rambock {

/** Driver for Microchip 23LC1024 external SRAM
 * Website: https://www.microchip.com/en-us/product/23LC1024
 * Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/20005142C.pdf
 *
 * Only supports SPI mode
 */
class Driver_23LC1024 : public MemoryDevice {
	static const SPISettings SPI_SETTINGS;

	enum Mode {
		BYTE = 0x00,
		PAGE = 0x80,
		SEQUENTIAL = 0x40,
		RESEVED = 0xc0,
	};

	enum Command {
		READ = 0x03,
		WRITE = 0x02,
		EDIO = 0x3b,
		EQIO = 0x38,
		RSTIO = 0xff,
		RDMR = 0x05,
		WRMR = 0x01,
	};

	void reset();
	void setMode(Mode mode);
	void sendAddress(uint32_t address);

	int m_cs;

  public:
	Driver_23LC1024(int cs);

	// setup device for random access
	void begin();

	virtual uint8_t *read(uint32_t from, uint8_t *to, size_t count) override;
	virtual uint32_t write(uint32_t to, const uint8_t *from,
						   size_t count) override;
};

const SPISettings Driver_23LC1024::SPI_SETTINGS(F_CPU, MSBFIRST, SPI_MODE0);

Driver_23LC1024::Driver_23LC1024(int cs) : m_cs(cs) {
	// setup pin for use as chip select
	pinMode(m_cs, OUTPUT);
	digitalWrite(m_cs, HIGH);
}

void Driver_23LC1024::reset() {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::RSTIO);

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();
}

void Driver_23LC1024::begin() {
	reset();
	setMode(Mode::SEQUENTIAL);
}

void Driver_23LC1024::setMode(Mode mode) {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::WRMR);
	SPI.transfer(mode);

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();
}

void Driver_23LC1024::sendAddress(uint32_t address) {
	// only 24 bit address, next 7 bits also ignored
	SPI.transfer((uint8_t)(address >> 16));
	SPI.transfer((uint8_t)(address >> 8));
	SPI.transfer((uint8_t)(address >> 0));
}

uint8_t *Driver_23LC1024::read(uint32_t from, uint8_t *to, size_t count) {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::READ);
	sendAddress(from);

	for (size_t i = 0; i < count; i++) {
		to[i] = SPI.transfer(0);
	}

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();

	return to;
}

uint32_t Driver_23LC1024::write(uint32_t to, const uint8_t *from,
								size_t count) {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::WRITE);
	sendAddress(to);

	for (size_t i = 0; i < count; i++) {
		SPI.transfer(from[i]);
	}

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();

	return to;
}

} // namespace rambock

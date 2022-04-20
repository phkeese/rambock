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
	void sendAddress(Address address);

	int m_cs;

  public:
	explicit Driver_23LC1024(int cs);

	/** Address just past last addressable byte
	 * @return length of array in bytes
	 */
	static inline Size size() { return 0x20000; }

	/** Sets up the device for random access
	 * Sets the CS pin to output mode and resets the chip to SPI mode
	 */
	void begin();

	virtual void *read(Address from, void *to, Size count) override;
	virtual Address write(Address to, const void *from, Size count) override;
};

const SPISettings Driver_23LC1024::SPI_SETTINGS(F_CPU, MSBFIRST, SPI_MODE0);

Driver_23LC1024::Driver_23LC1024(int cs) : m_cs(cs) {}

void Driver_23LC1024::reset() {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::RSTIO);

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();
}

void Driver_23LC1024::begin() {
	// setup pin for use as chip select
	pinMode(m_cs, OUTPUT);
	digitalWrite(m_cs, HIGH);

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

void Driver_23LC1024::sendAddress(Address address) {
	// only 24 bit address, next 7 bits also ignored
	SPI.transfer((uint8_t)(address >> 16));
	SPI.transfer((uint8_t)(address >> 8));
	SPI.transfer((uint8_t)(address >> 0));
}

void *Driver_23LC1024::read(Address from, void *to, Size count) {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::READ);
	sendAddress(from);

	uint8_t *data = static_cast<uint8_t *>(to);
	for (Size i = 0; i < count; i++) {
		data[i] = SPI.transfer(0);
	}

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();

	return to;
}

Address Driver_23LC1024::write(Address to, const void *from, Size count) {
	SPI.beginTransaction(SPI_SETTINGS);
	digitalWrite(m_cs, LOW);

	SPI.transfer(Command::WRITE);
	sendAddress(to);

	const uint8_t *data = static_cast<const uint8_t *>(from);
	for (Size i = 0; i < count; i++) {
		SPI.transfer(data[i]);
	}

	digitalWrite(m_cs, HIGH);
	SPI.endTransaction();

	return to;
}

} // namespace rambock

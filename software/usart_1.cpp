// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "usart_hal_1.hpp"
#include "usart_1.hpp"

#include <xpcc/architecture/driver/atomic.hpp>

namespace
{
	static xpcc::atomic::Queue<char, 16> rxBuffer;
	static xpcc::atomic::Queue<char, 512> txBuffer;
	static bool isBlocking = true;
}

void
dmx512::Usart1::initializeBuffered(uint32_t interruptPriority,
											bool blocking)
{
	UsartHal1::enableInterruptVector(true, interruptPriority);
	UsartHal1::enableInterrupt(Interrupt::RxNotEmpty);
	isBlocking = blocking;
}
void
dmx512::Usart1::writeBlocking(uint8_t data)
{
	while(!UsartHal1::isTransmitRegisterEmpty());
	UsartHal1::write(data);
}

void
dmx512::Usart1::writeBlocking(const uint8_t *data, std::size_t length)
{
	while (length-- != 0) {
		writeBlocking(*data++);
	}
}

void
dmx512::Usart1::flushWriteBuffer()
{
	while(!isWriteFinished());
}

bool
dmx512::Usart1::write(uint8_t data)
{
	if(txBuffer.isEmpty() && UsartHal1::isTransmitRegisterEmpty()) {
		UsartHal1::write(data);
	} else {
		while (!txBuffer.push(data)) {
			if (!isBlocking) {	// if queue is full => block
				return false;	// if not supposed ot block return false
			}
		}
		// Disable interrupts while enabling the transmit interrupt
		xpcc::atomic::Lock lock;
		// Transmit Data Register Empty Interrupt Enable
		UsartHal1::enableInterrupt(Interrupt::TxEmpty);
	}
	return true;
}

std::size_t
dmx512::Usart1::write(const uint8_t *data, std::size_t length)
{
	uint32_t i = 0;
	for (; i < length; ++i)
	{
		if (!write(*data++)) {
			return i;
		}
	}
	return i;
}

bool
dmx512::Usart1::isWriteFinished()
{
	return txBuffer.isEmpty() && UsartHal1::isTransmitRegisterEmpty();
}

std::size_t
dmx512::Usart1::discardTransmitBuffer()
{
	std::size_t count = 0;
	// disable interrupt since buffer will be cleared
	UsartHal1::disableInterrupt(UsartHal1::Interrupt::TxEmpty);
	while(!txBuffer.isEmpty()) {
		++count;
		txBuffer.pop();
	}
	return count;
}

bool
dmx512::Usart1::read(uint8_t &data)
{
	if (rxBuffer.isEmpty()) {
		return false;
	} else {
		data = rxBuffer.get();
		rxBuffer.pop();
		return true;
	}
}

std::size_t
dmx512::Usart1::read(uint8_t *data, std::size_t length)
{
	uint32_t i = 0;
	for (; i < length; ++i)
	{
		if (rxBuffer.isEmpty()) {
			return i;
		} else {
			*data++ = rxBuffer.get();
			rxBuffer.pop();
		}
	}
	return i;
}

std::size_t
dmx512::Usart1::discardReceiveBuffer()
{
	std::size_t count = 0;
	while(!rxBuffer.isEmpty()) {
		++count;
		rxBuffer.pop();
	}
	return count;
}
extern "C" void
USART1_IRQHandler()
{
	if (dmx512::UsartHal1::isReceiveRegisterNotEmpty()) {
		// TODO: save the errors
		uint8_t data;
		dmx512::UsartHal1::read(data);
		rxBuffer.push(data);
	}
	if (dmx512::UsartHal1::isTransmitRegisterEmpty()) {
		if (txBuffer.isEmpty()) {
			// transmission finished, disable TxEmpty interrupt
			dmx512::UsartHal1::disableInterrupt(dmx512::UsartHal1::Interrupt::TxEmpty);
		}
		else {
			dmx512::UsartHal1::write(txBuffer.get());
			txBuffer.pop();
		}
	}
}

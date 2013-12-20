// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef DMX512_USART_1_HPP
#define DMX512_USART_1_HPP

#include <xpcc/architecture.hpp>

#include "usart_hal_1.hpp"

namespace dmx512
{
/**
 * Universal asynchronous receiver transmitter (USART1)
 *
 * @author		Kevin Laeufer
 * @ingroup		stm32
 */
class Usart1 : public xpcc::stm32::UartBase, public ::xpcc::Uart
{
public:
	/// TypeId used to connect GPIO pins to this peripheral's rx.
	static const xpcc::stm32::TypeId::Uart1Rx Rx;
	/// TypeId used to connect GPIO pins to this peripheral's tx.
	static const xpcc::stm32::TypeId::Uart1Tx Tx;
	
	typedef UsartHal1::StopBit StopBit;
	
public:
	template<class clockSource, uint32_t baudrate>
	static inline void
	initialize(uint32_t interruptPriority, bool blocking = true,
				Parity parity = Parity::Disabled,
				StopBit stopBit = StopBit::StopBit1)
	{
		UsartHal1::initialize<clockSource, baudrate>(parity, stopBit);
		
		initializeBuffered(interruptPriority, blocking);
		UsartHal1::setTransmitterEnable(true);
		UsartHal1::setReceiverEnable(true);
	}

	static void
	writeBlocking(uint8_t data);

	static void
	writeBlocking(const uint8_t *data, std::size_t length);

	static void
	flushWriteBuffer();

	static bool
	write(uint8_t data);

	static std::size_t
	write(const uint8_t *data, std::size_t length);

	static bool
	isWriteFinished();

	static std::size_t
	discardTransmitBuffer();

	static bool
	read(uint8_t &data);

	static std::size_t
	read(uint8_t *buffer, std::size_t length);

	static std::size_t
	discardReceiveBuffer();
	
private:
	/// Second stage initialize for buffered uart
	// that need to be implemented in the .cpp
	static void
	initializeBuffered(uint32_t interruptPriority, bool blocking);
};

}

#endif // XPCC_STM32_USART_1_HPP

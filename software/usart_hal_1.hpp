// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef DMX512_USARTHAL_1_HPP
#define DMX512_USARTHAL_1_HPP

#include <stdint.h>
#include <xpcc/architecture.hpp>

namespace dmx512
{
/**
 * Universal asynchronous receiver transmitter (Usart1)
 *
 * Not available on the low- and medium density devices.
 *
 * Very basic implementation that exposes more hardware features than
 * the regular Usart classes.
 *
 * @author		Kevin Laeufer
 * @ingroup		stm32
 */
class UsartHal1 : public xpcc::stm32::UartBase
{
private:
	/**
	 * Disable Parity or Enable Odd/Even Parity
	 *
	 * This method assumes 8 databits + 1 parity bit
	 * Remember to enable the clock but not the UART peripheral
	 * before setting the party.
	 */
	static inline void
	setParity(const Parity parity);

	/// Calculate settings for the UBR register
	template<class clockSource, uint32_t baudrate, OversamplingMode>
	static inline uint16_t
	getBaudrateSettings();

public:
	/// TypeId used to connect GPIO pins to this peripheral's rx.
	static const xpcc::stm32::TypeId::UsartHal1Rx Rx;
	/// TypeId used to connect GPIO pins to this peripheral's tx.
	static const xpcc::stm32::TypeId::UsartHal1Tx Tx;
	
	enum class StopBit : uint32_t
	{
		StopBit0_5 	= USART_CR2_STOP_0,
		StopBit1 	= 0,
		StopBit1_5 	= USART_CR2_STOP_1 | USART_CR2_STOP_0,
		StopBit2  	= USART_CR2_STOP_1,
	};

public:
	/// Enables the clock, resets the hardware and sets the UE bit
	static inline void
	enable();

	/// Disables the hw module (by disabling its clock line)
	static inline void
	disable();

	/**
	 * Initialize Uart HAL Peripheral
	 *
	 * Enables clocks, the UART peripheral (but neither TX nor RX)
	 * Sets baudrate and parity.
	 */
	template<	class clockSource, uint32_t baudrate,
				OversamplingMode oversample = OversamplingMode::By16>
	static inline void
	initialize(Parity parity = Parity::Disabled,
	           StopBit stopBit = StopBit::StopBit1);

	/// Choose if you want to oversample by 16 (_default_) or by 8
	static inline void
	setOversamplingMode(OversamplingMode mode);
	// Methods needed to use this Usart Peripheral for SPI
	static inline void
	setSpiClock(SpiClock clk);

	static inline void
	setSpiDataMode(SpiDataMode mode);

	static inline void
	setLastBitClockPulse(LastBitClockPulse pulse);
	/**
	 * \brief	Write a single byte to the transmit register
	 *
	 * @warning 	This method does NOT do any sanity checks!!
	 *				It is your responsibility to check if the register
	 *				is empty!
	 */
	static inline void
	write(uint8_t data);

	/**
	 * Saves the value of the receive register to data
	 *
	 * @warning 	This method does NOT do any sanity checks!!
	 *				It is your responsibility to check if the register
	 *				contains something useful!
	 */
	static inline void
	read(uint8_t &data);

	/// Enable/Disable Transmitter
	static inline void
	setTransmitterEnable(const bool enable);

	/// Enable/Disable Receiver
	static inline void
	setReceiverEnable(bool enable);

	/// Returns true if data has been received
	static inline bool
	isReceiveRegisterNotEmpty();

	/// Returns true if data can be written
	static inline bool
	isTransmitRegisterEmpty();

	static inline void
	enableInterruptVector(bool enable, uint32_t priority);

	static inline void
	enableInterrupt(Interrupt interrupt);

	static inline void
	disableInterrupt(Interrupt interrupt);

	static inline InterruptFlag
	getInterruptFlags();

	/**
	 * Returns the value of the receive register
	 *
	 * @warning 	Not all InterruptFlags can be cleared this way.
	 */
	static inline void
	resetInterruptFlags(InterruptFlag flags);
};

}

#include "usart_hal_1_impl.hpp"

#endif // DMX512_USARTHAL_1_HPP

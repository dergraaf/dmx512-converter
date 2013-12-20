
#include <xpcc/architecture.hpp>
#include "usart_1.hpp"

using namespace xpcc::stm32;

typedef GpioInputC13 Button;
typedef GpioOutputA8 Led;
typedef GpioOutputB5 Rs485Enable;

namespace dmx512
{
	uint8_t channels[512] = {};
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Systemclock
	typedef Pll<ExternalClock<MHz25>, MHz168, MHz48> ClockSource;
	typedef SystemClock<ClockSource> Clock;
	
	Clock::enable();
	
	// Breakout board
	Led::setOutput(xpcc::Gpio::High);
	
	// Debug UART
	GpioOutputC12::connect(Uart5::Tx);
	GpioInputD2::connect(Uart5::Rx);
	
	Uart5::initialize<Clock, 115200>(10);
	
	// RS485/DMX512
	Rs485Enable::configure(Gpio::OutputType::OpenDrain);
	Rs485Enable::setOutput(xpcc::Gpio::Low);
	
	GpioOutputB6::connect(dmx512::Usart1::Tx, Gpio::OutputType::OpenDrain);
	GpioInputB7::connect(dmx512::Usart1::Rx);
	
	dmx512::Usart1::initialize<Clock, 250000>(3,
	                                          true,
	                                          dmx512::Usart1::Parity::Disabled,
	                                          dmx512::Usart1::StopBit::StopBit2);
	
	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(500);
		
		uint8_t data;
		if (Uart5::read(data)) {
			Uart5::writeBlocking(data);
		}
		
		while (!dmx512::Usart1::isWriteFinished()) {
			// wait till previous transmission is finished
		}
		
		// disable transmitter
		Rs485Enable::reset();
		
		xpcc::delay_us(200);
		
		
		
		// Enable transmitter
		Rs485Enable::set();
		
		// send break
		GpioOutputB6::setOutput(xpcc::Gpio::Low);
		
		// min 88µs
		xpcc::delay_us(100);
		
		GpioOutputB6::set();
		GpioOutputB6::connect(dmx512::Usart1::Tx, Gpio::OutputType::OpenDrain);
		
		xpcc::delay_us(12);
		
		dmx512::Usart1::writeBlocking(0);
		dmx512::Usart1::writeBlocking(dmx512::channels, 10);
	}
}

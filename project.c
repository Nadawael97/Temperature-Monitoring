


//*****************************************************************************
//
// temperature_sensor.c - Example demonstrating the internal ADC temperature
//                        sensor.
//
// Copyright (c) 2010-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 2.1.4.178 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "LCDHeader.h"
#include "tm4c123gh.h"

//*****************************************************************************
//
//! \addtogroup adc_examples_list
//! <h1>ADC Temperature Sensor (temperature_sensor)</h1>
//!
//! This example shows how to setup ADC0 to read the internal temperature
//! sensor.
//!
//! NOTE: The internal temperature sensor is not calibrated.  This example
//! just takes the raw temperature sensor sample and converts it using the
//! equation found in the LM3S9B96 datasheet.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC0 peripheral
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of the
//! ADC.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//
//*****************************************************************************

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************


//*****************************************************************************
//
// Configure ADC0 for the temperature sensor input with a single sample.  Once
// the sample is done, an interrupt flag will be set, and the data will be
// read then displayed on the console via UART0.
//
//*****************************************************************************
	void SystemInit () {}
int
main(void)
{
		
	
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
			GPIOPinConfigure(GPIO_PA1_U0TX);
	    GPIOPinConfigure(GPIO_PA0_U0RX);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
			UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
			GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			UARTStdioConfig(0, 9600, 16000000);
				
			UARTIntClear(UART0_BASE,UART_INT_9BIT | UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE | UART_INT_FE | UART_INT_TX | UART_INT_RX | UART_INT_DSR | UART_INT_DCD| UART_INT_CTS| UART_INT_RI);

		
	
    //
    // This array is used for storing the data read from the ADC FIFO. It
    // must be as large as the FIFO for the sequencer in use.  This example
    // uses sequence 3 which has a FIFO depth of 1.  If another sequence
    // was used with a deeper FIFO, then the array size must be changed.
    //
    uint32_t pui32ADC0Value[1];
	
    //
    // These variables are used to store the temperature conversions for
    // Celsius and Fahrenheit.
    //
    uint32_t ui32TempValueC;
    uint32_t ui32TempValueF;

    //
    // Set the clocking to run at 20 MHz (200 MHz / 10) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock
    // source.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //




    //
    // Set up the serial console to use for displaying messages.  This is just
    // for this example program and is not needed for ADC operation.
    //
    



    //
    // Display the setup on the console.
    //
    UARTprintf("ADC ->\n");
    UARTprintf("  Type: Internal Temperature Sensor\n");
    UARTprintf("  Samples: One\n");
    UARTprintf("  Update Rate: 250ms\n");
    UARTprintf("  Input Pin: Internal temperature sensor\n\n");



    //
    // The ADC0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a singal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.  This example is arbitrarily using sequence 3.
    //
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample the temperature sensor
    // (ADC_CTL_TS) and configure the interrupt flag (ADC_CTL_IE) to be set
    // when the sample is done.  Tell the ADC logic that this is the last
    // conversion on sequence 3 (ADC_CTL_END).  Sequence 3 has only one
    // programmable step.  Sequence 1 and 2 have 4 steps, and sequence 0 has
    // 8 programmable steps.  Since we are only doing a single conversion using
    // sequence 3 we will only configure step 0.  For more information on the
    // ADC sequences and steps, reference the datasheet.
    //
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 3);

    //
    // Sample the temperature sensor forever.  Display the value on the
    // console.
    //
    while(1)
    {
			
				ADCIntClearEx(ADC0_BASE, ADC_INT_SS0 | ADC_INT_SS1 | ADC_INT_SS2 | ADC_INT_SS3 | ADC_INT_DMA_SS0 | ADC_INT_DMA_SS1 | ADC_INT_DMA_SS2 | ADC_INT_DMA_SS3
	 | ADC_INT_DCON_SS0 | ADC_INT_DCON_SS1 | ADC_INT_DCON_SS2 | ADC_INT_DCON_SS3);
			
			
				UARTIntClear(UART0_BASE,UART_INT_9BIT | UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE | UART_INT_FE | UART_INT_TX | UART_INT_RX | UART_INT_DSR | UART_INT_DCD| UART_INT_CTS| UART_INT_RI);
				//UARTprintf("Fuck You\n");
        //
        // Trigger the ADC conversion.
        //
        ADCProcessorTrigger(ADC0_BASE, 3);

        //
        // Wait for conversion to be completed.
        //
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }

        //
        // Clear the ADC interrupt flag.
        //
        ADCIntClear(ADC0_BASE, 3);
			
        //
        // Read ADC Value.
        //
        ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

        //
        // Use non-calibrated conversion provided in the data sheet.  Make
        // sure you divide last to avoid dropout.
        //
        ui32TempValueC = 147.5 - ((75 * (3.3 - 0) * pui32ADC0Value[0]) / 4096);

        //
        // Get Fahrenheit value.  Make sure you divide last to avoid dropout.
        //
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

        //
        // Display the temperature value on the console.
        //
		
				UARTprintf("Temperature %3d *C\r", ui32TempValueC);

        //
        // This function provides a means of generating a constant length
        // delay.  The function delay (in cycles) = 3 * parameter.  Delay
        // 250ms arbitrarily.
        //

        SysCtlDelay(SysCtlClockGet() / 12);

	
    }
		
}

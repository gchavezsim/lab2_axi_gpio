/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"

#define LED_DELAY (99990000)

int main()
{
    XGpio Gpio1, Gpio2, Gpio3;
    int Status;
    volatile int delay;
    u32 switch_values;
    u32 binary_count = 0;
    u32 ring_count = 1;



    init_platform();

    Status = XGpio_Initialize(&Gpio1, XPAR_AXI_GPIO_0_BASEADDR);

    if (Status != XST_SUCCESS)
    {
        xil_printf("GPIO Intialization failed.\r\n");
        return XST_FAILURE;        
    }

    xil_printf("GPIO has intitialized successfully.\r\n");

    Status = XGpio_Initialize(&Gpio2, XPAR_AXI_GPIO_1_BASEADDR);

    if (Status != XST_SUCCESS)
    {
        xil_printf("GPIO Intialization failed.\r\n");
        return XST_FAILURE;        
    }

    xil_printf("GPIO has intitialized successfully.\r\n");

    Status = XGpio_Initialize(&Gpio3, XPAR_AXI_GPIO_2_BASEADDR);

    if (Status != XST_SUCCESS)
    {
        xil_printf("GPIO Intialization failed.\r\n");
        return XST_FAILURE;        
    }

    xil_printf("GPIO has intitialized successfully.\r\n");

    XGpio_SetDataDirection(&Gpio1, 1, 0);
    XGpio_SetDataDirection(&Gpio2, 1, 0);
    XGpio_SetDataDirection(&Gpio3, 1, 1);

    while (1)
    {
        switch_values = XGpio_DiscreteRead(&Gpio3, 1);

        switch(switch_values)
        {
            case 0x01:
                xil_printf("%X\r\n", switch_values);
                XGpio_DiscreteWrite(&Gpio1, 1, switch_values);
                //XGpio_DiscreteWrite(&Gpio2, 1, switch_values);
                XGpio_DiscreteWrite(&Gpio2, 1, 0x04);
                break;

            case 0x02:
                xil_printf("%X\r\n", switch_values);
                XGpio_DiscreteWrite(&Gpio1, 1, switch_values);
                //XGpio_DiscreteWrite(&Gpio2, 1, switch_values);
                XGpio_DiscreteWrite(&Gpio2, 1, 0x02);
                break;

            case 0x04:
                xil_printf("%X\r\n", switch_values);
                XGpio_DiscreteWrite(&Gpio1, 1, switch_values);
                //XGpio_DiscreteWrite(&Gpio2, 1, switch_values);
                XGpio_DiscreteWrite(&Gpio2, 1, 0x01);
                break;

            case 0x08:
                xil_printf("%X\r\n", switch_values);
                XGpio_DiscreteWrite(&Gpio1, 1, switch_values);
                XGpio_DiscreteWrite(&Gpio2, 1, 0x07);
                break;

            case 0x03:
                
                XGpio_DiscreteWrite(&Gpio1, 1, binary_count);                    
                XGpio_DiscreteClear(&Gpio2, 1, 0xF);

                if (binary_count == 15)
                {
                    binary_count = 0;
                } else {
                    binary_count += 1;
                }

                break;

            case 0x0C:
                XGpio_DiscreteWrite(&Gpio1, 1, ring_count);                    
                XGpio_DiscreteClear(&Gpio2, 1, 0xF);

                if (ring_count == 8) {
                    ring_count = 1;
                } else {
                    ring_count = ring_count << 1;
                }

                break;

            default:
            // leds and RGB off
                XGpio_DiscreteClear(&Gpio1, 1, 0xF);
                XGpio_DiscreteClear(&Gpio2, 1, 0xF);
                break;
        }

        // switch sampling rate
        for (delay = 0; delay < LED_DELAY; delay++);
    }
}

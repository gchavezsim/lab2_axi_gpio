## CSUN ECE 520 - Lab 2: AXI GPIO

### Overview

In this lab, I created a Zynq-7000 System-on-Chip (SoC) design on the Zybo Z7-10 development board using the Vivado IP Integrator. The design utilized three AXI GPIO peripherals connected between the Processing System (PS) and Programmable Logic (PL). Software written in C and executed on the ARM Cortex-A9 processor controlled the PL peripherals through the AXI interconnect. The completed system reads switch inputs, drives the four user LEDs, controls the RGB LED, and implements both binary and ring counter operating modes using software running on the PS.

### Design Summary

The architecture consists of a Zynq Processing System connected to three AXI GPIO peripherals through the AXI General Purpose master interface and AXI interconnect. The hardware design was implemented in Vivado and exported as an XSA platform for software development in Vitis.

- **AXI GPIO #1 (LED Output Peripheral)**: Configured as a 4-bit output peripheral connected to LED0 through LED3 on the Zybo board. The software writes LED patterns directly to this GPIO peripheral.

- **AXI GPIO #2 (RGB LED Peripheral)**: Configured as a 3-bit output peripheral connected to the RGB LED. Bit 0 controls the Red channel, Bit 1 controls the Green channel, and Bit 2 controls the Blue channel. Multiple colors can be produced by asserting more than one bit simultaneously.

- **AXI GPIO #3 (Switch Input Peripheral)**: Configured as a 4-bit input peripheral connected to switches SW0-SW3. The application continuously polls this GPIO peripheral to determine the desired operating mode.

The software initializes all GPIO peripherals using the Xilinx GPIO driver library. GPIO1 and GPIO2 are configured as outputs while GPIO3 is configured as an input. A switch-case structure is then used to decode the switch inputs and perform the required LED and RGB LED control functions.

The implemented switch mappings are summarized below:

| Switch State | LED Output | RGB LED Output |
|-------------|------------|---------------|
| `0x01` | LED0 ON | Red (`0x04`) |
| `0x02` | LED1 ON | Green (`0x02`) |
| `0x04` | LED2 ON | Blue (`0x01`) |
| `0x08` | LED3 ON | White (`0x07`) |
| `0x03` | Binary Counter | OFF |
| `0x0C` | Ring Counter | OFF |
| Any Other Value | LEDs OFF | OFF |

#### Binary Counter Mode

When switches SW0 and SW1 are enabled simultaneously (`0x03`), the software displays a 4-bit up-counter on LED0-LED3. The variable `binary_count` increments from `0x0` to `0xF` before wrapping back to `0x0`. During this mode, the RGB LED is disabled.

#### Ring Counter Mode

When switches SW2 and SW3 are enabled simultaneously (`0x0C`), the software displays a ring counter pattern on LED0-LED3. A single active LED shifts left through the sequence:

```text
0001 → 0010 → 0100 → 1000 → 0001
```

The active LED wraps back to LED0 after reaching LED3. The RGB LED remains disabled during this mode.

A software delay loop (`LED_DELAY`) is used to slow the counter update rate and make the counting patterns visible on the hardware.

### Verification and Results

Verification was performed directly on the Zybo Z7-10 hardware through observation of the LEDs, RGB LED, and UART debug messages printed to the serial terminal.

- **Test Case 1 (Red Mode)**  
  Evaluated with SW0 enabled (`0x01`). LED0 illuminated and the RGB LED displayed red. The UART terminal correctly printed the switch value.

- **Test Case 2 (Green Mode)**  
  Evaluated with SW1 enabled (`0x02`). LED1 illuminated and the RGB LED displayed green. GPIO operation matched the design requirements.

- **Test Case 3 (Blue Mode)**  
  Evaluated with SW2 enabled (`0x04`). LED2 illuminated and the RGB LED displayed blue. Correct operation was observed.

- **Test Case 4 (White Mode)**  
  Evaluated with SW3 enabled (`0x08`). LED3 illuminated and the RGB LED displayed white through simultaneous activation of all RGB channels.

- **Test Case 5 (Binary Counter Mode)**  
  Evaluated with SW0 and SW1 enabled simultaneously (`0x03`). The LEDs incremented through the sequence `0000` to `1111` before wrapping back to `0000`. The RGB LED remained off throughout operation.

- **Test Case 6 (Ring Counter Mode)**  
  Evaluated with SW2 and SW3 enabled simultaneously (`0x0C`). The active LED shifted from LED0 to LED3 before wrapping back to LED0. The RGB LED remained disabled.

- **Test Case 7 (Invalid Switch Combinations)**  
  Evaluated using switch combinations not explicitly defined in the lab specification. All user LEDs and RGB LED were turned off as required.

The UART terminal successfully displayed GPIO initialization messages and switch values, confirming proper communication between the Processing System and AXI GPIO peripherals.

### Known Issues or Limitations

- **Polling-Based Implementation**: The software continuously polls the switch GPIO peripheral rather than using interrupts. While simple to implement, this approach utilizes processor resources continuously.

- **Software-Based Timing**: Counter speed is controlled using a software delay loop. Changes to processor clock frequency would affect the visible counter update rate.

- **Switch Bounce**: Mechanical switch transitions are not debounced. As a result, brief unintended intermediate states may occur while toggling switches.

- **Blocking Delay Loop**: The delay loop prevents the processor from performing other useful work while waiting. A timer-based implementation would provide a more efficient solution.

- **Limited Counter Functionality**: Only one counter mode can be active at a time. Additional software logic would be required to support programmable count rates or alternative counting patterns.
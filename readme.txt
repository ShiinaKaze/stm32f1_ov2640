# Project Structure
According to book: Embedded Systems Architecture: A Comprehensive Guide for Engineers and Programmers
## Cortex-M
- Core
    - main.c
    - stm32f10x_conf.h
    - stm32f10x_it.c
    - stm32f10x_it.h
## MCU System
- System
    - delay.h
    - delay.c
## MCU Peripheral
- Peripheral
	- i2c.h
	- i2c.c
## Other Module
Such as OLED/LCD, Wi-Fi, Bluetooth
- Module
    - oled.h
    - oled.c
## CMSIS Library
- CMSIS
    - Core
        - core_cm3.c
        - core_cm3.h
    - Device
        - startup
            - startup_stm32f10x_md.s
        - stm32f10x.h
        - system_stm32f10x.c
        - system_stm32f10x.h
## Standard Peripherals Library
- SPL
    - inc
        - stm32f10x_ppp.h
    - src
        - stm32f10x_ppp.c

# uVision IDE
## Project items
Project Targets: Target 1
Groups:
	- Core
	- System
	- Peripheral
	- CMSIS/Core
	- CMSIS/Device
	- CMSIS/Device/startup
	- SPL/inc
	- SPL/src

## Compiler Setting
Compiler: ARM Compiler version 5
Include path:
    .\Core
    .\System
    .\Peripheral
    .\CMSIS\Core
    .\CMSIS\Device
    .\CMSIS\Device\startup
    .\SPL\inc
Preprocessor: STM32F10X_MD,USE_STDPERIPH_DRIVER
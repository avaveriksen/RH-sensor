/**
  ******************************************************************************
  * @file      startup_stm32f303x8.s
  * @author    MCD Application Team
  * @brief     STM32F303x6/STM32F303x8 devices vector table for GCC toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address,
  *                - Configure the clock system  
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

  .syntax unified
	.cpu cortex-m4
	.fpu softvfp
	.thumb

.global	g_pfnVectors
.global	Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word	_sidata
/* start address for the .data section. defined in linker script */
.word	_sdata
/* end address for the .data section. defined in linker script */
.word	_edata
/* start address for the .bss section. defined in linker script */
.word	_sbss
/* end address for the .bss section. defined in linker script */
.word	_ebss

.equ  BootRAM,        0xF1E0F85F
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

    .section	.text.Reset_Handler
	.weak	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
                                                    ldr   sp, =_estack    /* Atollic update: set stack pointer */
  
/* Call the clock system initialization function.*/
    bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit
  
/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
	bl	main

LoopForever:
    b LoopForever
    
.size	Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
    .section	.text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
	b	Infinite_Loop
	.size	Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex-M4.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
 	.section	.isr_vector,"a",%progbits
	.type	g_pfnVectors, %object
	.size	g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
	.word	_estack
	.word	Reset_Handler
	.word	NMI_Handler
	.word	HardFault_Handler
	.word	MemManage_Handler
	.word	BusFault_Handler
	.word	UsageFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler
	.word	DebugMon_Handler
	.word	0
	.word	PendSV_Handler
	.word	SysTick_Handler
	.word	WWDG_IRQHandler
	.word	PVD_IRQHandler
	.word	TAMP_STAMP_IRQHandler
	.word	RTC_WKUP_IRQHandler
	.word	FLASH_IRQHandler
	.word	RCC_IRQHandler
	.word	EXTI0_IRQHandler
	.word	EXTI1_IRQHandler
	.word	EXTI2_TSC_IRQHandler
	.word	EXTI3_IRQHandler
	.word	EXTI4_IRQHandler
	.word	DMA1_Channel1_IRQHandler
	.word	DMA1_Channel2_IRQHandler
	.word	DMA1_Channel3_IRQHandler
	.word	DMA1_Channel4_IRQHandler
	.word	DMA1_Channel5_IRQHandler
	.word	DMA1_Channel6_IRQHandler
	.word	DMA1_Channel7_IRQHandler
	.word	ADC1_2_IRQHandler
	.word	CAN_TX_IRQHandler
	.word	CAN_RX0_IRQHandler
	.word	CAN_RX1_IRQHandler
	.word	CAN_SCE_IRQHandler
	.word	EXTI9_5_IRQHandler
	.word	TIM1_BRK_TIM15_IRQHandler
	.word	TIM1_UP_TIM16_IRQHandler
	.word	TIM1_TRG_COM_TIM17_IRQHandler
	.word	TIM1_CC_IRQHandler
	.word	TIM2_IRQHandler
	.word	TIM3_IRQHandler
	.word	0
	.word	I2C1_EV_IRQHandler
	.word	I2C1_ER_IRQHandler
	.word	0
	.word	0
	.word	SPI1_IRQHandler
	.word	0
	.word	USART1_IRQHandler
	.word	USART2_IRQHandler
	.word	USART3_IRQHandler
	.word	EXTI15_10_IRQHandler
	.word	RTC_Alarm_IRQHandler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	TIM6_DAC1_IRQHandler
	.word	TIM7_DAC2_IRQHandler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	COMP2_IRQHandler
	.word	COMP4_6_IRQHandler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	FPU_IRQHandler

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak	NMI_Handler
	.thumb_set NMI_Handler,Default_Handler

  .weak	HardFault_Handler
	.thumb_set HardFault_Handler,Default_Handler

  .weak	MemManage_Handler
	.thumb_set MemManage_Handler,Default_Handler

  .weak	BusFault_Handler
	.thumb_set BusFault_Handler,Default_Handler

	.weak	UsageFault_Handler
	.thumb_set UsageFault_Handler,Default_Handler

	.weak	SVC_Handler
	.thumb_set SVC_Handler,Default_Handler

	.weak	DebugMon_Handler
	.thumb_set DebugMon_Handler,Default_Handler

	.weak	PendSV_Handler
	.thumb_set PendSV_Handler,Default_Handler

	.weak	SysTick_Handler
	.thumb_set SysTick_Handler,Default_Handler

	.weak	WWDG_IRQHandler
	.thumb_set WWDG_IRQHandler,Default_Handler

	.weak	PVD_IRQHandler
	.thumb_set PVD_IRQHandler,Default_Handler

	.weak	TAMP_STAMP_IRQHandler
	.thumb_set TAMP_STAMP_IRQHandler,Default_Handler

	.weak	RTC_WKUP_IRQHandler
	.thumb_set RTC_WKUP_IRQHandler,Default_Handler

	.weak	FLASH_IRQHandler
	.thumb_set FLASH_IRQHandler,Default_Handler

	.weak	RCC_IRQHandler
	.thumb_set RCC_IRQHandler,Default_Handler

	.weak	EXTI0_IRQHandler
	.thumb_set EXTI0_IRQHandler,Default_Handler

	.weak	EXTI1_IRQHandler
	.thumb_set EXTI1_IRQHandler,Default_Handler

	.weak	EXTI2_TSC_IRQHandler
	.thumb_set EXTI2_TSC_IRQHandler,Default_Handler

	.weak	EXTI3_IRQHandler
	.thumb_set EXTI3_IRQHandler,Default_Handler

	.weak	EXTI4_IRQHandler
	.thumb_set EXTI4_IRQHandler,Default_Handler

	.weak	DMA1_Channel1_IRQHandler
	.thumb_set DMA1_Channel1_IRQHandler,Default_Handler

	.weak	DMA1_Channel2_IRQHandler
	.thumb_set DMA1_Channel2_IRQHandler,Default_Handler

	.weak	DMA1_Channel3_IRQHandler
	.thumb_set DMA1_Channel3_IRQHandler,Default_Handler

	.weak	DMA1_Channel4_IRQHandler
	.thumb_set DMA1_Channel4_IRQHandler,Default_Handler

	.weak	DMA1_Channel5_IRQHandler
	.thumb_set DMA1_Channel5_IRQHandler,Default_Handler

	.weak	DMA1_Channel6_IRQHandler
	.thumb_set DMA1_Channel6_IRQHandler,Default_Handler

	.weak	DMA1_Channel7_IRQHandler
	.thumb_set DMA1_Channel7_IRQHandler,Default_Handler

	.weak	ADC1_2_IRQHandler
	.thumb_set ADC1_2_IRQHandler,Default_Handler

	.weak	CAN_TX_IRQHandler
	.thumb_set CAN_TX_IRQHandler,Default_Handler

	.weak	CAN_RX0_IRQHandler
	.thumb_set CAN_RX0_IRQHandler,Default_Handler

	.weak	CAN_RX1_IRQHandler
	.thumb_set CAN_RX1_IRQHandler,Default_Handler

	.weak	CAN_SCE_IRQHandler
	.thumb_set CAN_SCE_IRQHandler,Default_Handler

	.weak	EXTI9_5_IRQHandler
	.thumb_set EXTI9_5_IRQHandler,Default_Handler

	.weak	TIM1_BRK_TIM15_IRQHandler
	.thumb_set TIM1_BRK_TIM15_IRQHandler,Default_Handler

	.weak	TIM1_UP_TIM16_IRQHandler
	.thumb_set TIM1_UP_TIM16_IRQHandler,Default_Handler

	.weak	TIM1_TRG_COM_TIM17_IRQHandler
	.thumb_set TIM1_TRG_COM_TIM17_IRQHandler,Default_Handler

	.weak	TIM1_CC_IRQHandler
	.thumb_set TIM1_CC_IRQHandler,Default_Handler

	.weak	TIM2_IRQHandler
	.thumb_set TIM2_IRQHandler,Default_Handler

	.weak	TIM3_IRQHandler
	.thumb_set TIM3_IRQHandler,Default_Handler

	.weak	I2C1_EV_IRQHandler
	.thumb_set I2C1_EV_IRQHandler,Default_Handler

	.weak	I2C1_ER_IRQHandler
	.thumb_set I2C1_ER_IRQHandler,Default_Handler

	.weak	SPI1_IRQHandler
	.thumb_set SPI1_IRQHandler,Default_Handler

	.weak	USART1_IRQHandler
	.thumb_set USART1_IRQHandler,Default_Handler

	.weak	USART2_IRQHandler
	.thumb_set USART2_IRQHandler,Default_Handler

	.weak	USART3_IRQHandler
	.thumb_set USART3_IRQHandler,Default_Handler

	.weak	EXTI15_10_IRQHandler
	.thumb_set EXTI15_10_IRQHandler,Default_Handler

	.weak	RTC_Alarm_IRQHandler
	.thumb_set RTC_Alarm_IRQHandler,Default_Handler

	.weak	TIM6_DAC1_IRQHandler
	.thumb_set TIM6_DAC1_IRQHandler,Default_Handler

	.weak	TIM7_DAC2_IRQHandler
	.thumb_set TIM7_DAC2_IRQHandler,Default_Handler
	
	.weak	COMP2_IRQHandler
	.thumb_set COMP2_IRQHandler,Default_Handler
	
	.weak	COMP4_6_IRQHandler
	.thumb_set COMP4_6_IRQHandler,Default_Handler
	
	.weak	FPU_IRQHandler
	.thumb_set FPU_IRQHandler,Default_Handler
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

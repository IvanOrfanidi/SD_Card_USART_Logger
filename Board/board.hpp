/**
 ******************************************************************************
 * @file    boadr.hpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    28-March-2018
 * @brief   This file contains all the methods prototypes for the RTC
 *          firmware library.
 ******************************************************************************
 * @attention
 *
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_HPP
#define __BOARD_HPP

/* Standart lib */
#include <stdint.h>

/* StdPeriphLib */
#include "stm32f10x_gpio.h"

#ifdef __HW_VER_1__    // Hardware version

/// USART
#define UART_DATA_SOURCE USART1

/// INPUT STATUS SD CARD
#define WD_PIN GPIO_Pin_2
#define PORT_WD GPIOA
#define PORT_WD_CLK RCC_APB2Periph_GPIOA

#define CD_PIN GPIO_Pin_3
#define PORT_CD GPIOA
#define PORT_CD_CLK RCC_APB2Periph_GPIOA

/// LIGHT LED
#define LED_RUN_PIN GPIO_Pin_12
#define PORT_RUN_LED GPIOB
#define PORT_RUN_LED_CLK RCC_APB2Periph_GPIOB

#define LED_ERR_PIN GPIO_Pin_13
#define PORT_ERR_LED GPIOB
#define PORT_ERR_LED_CLK RCC_APB2Periph_GPIOB

/// USB CONNECT
#define USB_CONNECT_PIN GPIO_Pin_9
#define PORT_USB_CONNECT GPIOB
#define PORT_USB_CONNECT_CLK RCC_APB2Periph_GPIOB

#endif    ///HW_VER_1

#ifdef __cplusplus

/*
 * @brief Class Board
 */
class Board {
  public:
    static void InitGPIO();    ///< Initialisation Led

    static void InitBKP();    ///< Initialisation Backup

    static void InitIWDG();    ///< Initialisation Watchdog timer

    static void LedErrOn();    ///< Enable Err Led

    static void LedErrOff();    ///< Disable Err Led

    static void LedRunOn();    ///< Enable Run Led

    static void LedRunOff();    ///< Disable Run Led

    static bool GetWriteDaisable();    ///< Get state pin WD SD card

    static bool GetCardDetect();    ///< Get state SD card detect

    static bool GetUsbConnect();    ///< Get state PC and USB
};

/*
 * @brief Extern interrupt
 */
extern "C" {
}
#endif    //__cplusplus

#endif
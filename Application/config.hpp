/**
 ******************************************************************************
 * @file    config.cpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    04-2019
 * @brief   This is file config.
 ******************************************************************************
 * @attention
 *
 *
 *
 ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_HPP
#define __CONFIG_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f10x_usart.h"

#ifdef __cplusplus

/* Build Date --------------------------------------------------------------*/
#pragma location = "BuildDate"
__root constexpr char __BUILD__DATE__[] = __DATE__;

// The name of the configuration file
constexpr char NAME_CONFIG_FILE[] = "config.ini";

/**
 * @brief Class Config
 */
class Config {
  public:
    enum Usart_t {
        PREEMPTION_PRIORITY = 0,
        SUB_PRIORITY = 0,

        TX_BUFFER_SIZE = 1536,
        RX_BUFFER_SIZE = 1536,

    };

    enum {
        AUTO_BAUDRATE = 0,
        MAX_BAUDRATE = 256000,
        MIN_SIZE_FILE = 1,
        MAX_SIZE_FILE = 102400,

        NUM_PARAMS = 6
    };

    /// Config for USART
    struct Config_t {
        uint32_t Baudrate;
        uint16_t WordLength;
        uint16_t Parity;
        uint16_t StopBits;
        uint32_t FileSize;    ///< File size in KB
        bool AddTime;

        enum ConfigDefault_t {
            DEF_BAUDRATE = AUTO_BAUDRATE,
            DEF_WORDLEN = USART_WordLength_8b,
            DEF_PARITY = USART_Parity_No,
            DEF_STOPBITS = USART_StopBits_1,
            DEF_FILE_SIZE = 1024,
            DEF_ADDTIME = false
        };

        Config_t()
        {
            Baudrate = DEF_BAUDRATE;
            WordLength = DEF_WORDLEN;
            Parity = DEF_PARITY;
            StopBits = DEF_STOPBITS;
            FileSize = DEF_FILE_SIZE;
            AddTime = DEF_ADDTIME;
        }
    };

    /// Read configuration from file.
    static bool ReadFile(Config_t*);

    /// Create config file
    static bool CreateFile(Config_t&);

    static void ManualSetting(
        USART_InitTypeDef*);    /// Set manual setting for USART

    static void AutoSetting(
        USART_InitTypeDef*);    /// Set automatic setting for USART
};

extern "C" {
}

#endif    //__cplusplus

#endif
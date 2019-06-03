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
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
/* Standart lib */
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

/* User lib */
#include "config.hpp"
#include "main.hpp"
#include "board.hpp"

#include "ff.h"
#include "diskio.h"

/**
 * @brief  Read configuration from file.
 * @retval true - success, false - fail
 */
bool Config::ReadFile(Config_t* config)
{
    // Open file configurations for USART
    FIL file;
    if(f_open(&file, NAME_CONFIG_FILE, FA_OPEN_EXISTING | FA_READ) == FR_OK) {
        /* 
            The configuration entry must be in the form:
            0
            8-N-1
            1024
            1

            where:
            0 - Baud rate UART(0 - Auto selected from 600 to 115200)

            8-N-1   8 - Data bits(8,9),
                    N - Parity(N - none, E - even, O - odd),
                    1 - Stop bits(0.5, 1, 1.5, 2)
            1024 - Maximum log file size in KB(from 1 to 102400)
            1 - Record time in the log(1 - set time, 0 - no)
        */

        std::string buff;
        buff.resize(FF_MAX_SS);

        uint32_t len = 0;
        f_read(&file,
            &buff[0],
            buff.capacity(),
            &len);    //Read File in BufferSdCard
        f_close(&file);

        /* Parse config text */
        if(0 == len) {
            return false;
        }

        const char* const delimiter[] = {
            "\r\n", "-", "-", "\r\n", "\r\n", "\r\n"
        };

        std::string params[NUM_PARAMS];
        size_t pos = 0;
        std::string token;
        for(size_t i = 0; i < NUM_PARAMS; ++i) {
            if((pos = buff.find(delimiter[i])) != std::string::npos) {
                token = buff.substr(0, buff.find(delimiter[i]));
                params[i] = token;
                buff.erase(0, pos + strlen(delimiter[i]));
            }
            else {
                // None delimiter
                return false;
            }
        }

        // Checking configurations
        uint32_t baudrate = atoi(params[0].c_str());
        if(baudrate > MAX_BAUDRATE) {
            return false;
        }

        if(("8" != params[1]) && ("9" != params[1])) {
            return false;
        }

        if(("N" != params[2]) && ("O" != params[2]) && ("E" != params[2])) {
            return false;
        }

        if(("0.5" != params[3]) && ("1.5" != params[3]) && ("2" != params[3]) &&
            ("1" != params[3])) {
            return false;
        }

        // Size File
        uint32_t size = atoi(params[4].c_str());
        if(size > MAX_SIZE_FILE || size < MIN_SIZE_FILE) {
            return false;
        }

        // Add Time
        if(("0" != params[5]) && ("1" != params[5])) {
            return false;
        }

        /* Assigning variables */

        // Baud Rate
        config->Baudrate = baudrate;

        // Word Length
        if("9" == params[1]) {
            config->WordLength = USART_WordLength_9b;
        }
        else {
            config->WordLength = USART_WordLength_8b;
        }

        // Parity
        if("O" == params[2]) {
            config->Parity = USART_Parity_Odd;
        }
        else if("E" == params[2]) {
            config->Parity = USART_Parity_Even;
        }
        else {
            config->Parity = USART_Parity_No;
        }

        // Stop Bits
        if("0.5" == params[3]) {
            config->StopBits = USART_StopBits_0_5;
        }
        else if("1.5" == params[3]) {
            config->StopBits = USART_StopBits_1_5;
        }
        else if("2" == params[3]) {
            config->StopBits = USART_StopBits_2;
        }
        else {
            config->StopBits = USART_StopBits_1;
        }

        // File of size
        config->FileSize = size;

        // Add Time
        config->AddTime = static_cast<bool>(atoi(params[5].c_str()));
        return true;
    }
    return false;
}

/**
 * @brief  Create config file
 * @param  config - configuration for file 
 * @retval true - success, false - fail
 */
bool Config::CreateFile(Config_t& config)
{
    DIR dirs;    // Dir
    FRESULT res = f_opendir(&dirs, "/");
    if(res == FR_OK) {
        // Create config file
        FIL file;
        res = f_open(&file, NAME_CONFIG_FILE, FA_OPEN_ALWAYS | FA_WRITE);
        if(res == FR_OK) {
            // string with config
            std::string conf;

            // Baud Rate
            conf += std::to_string(config.Baudrate) + "\r\n";

            // Word Length
            if(USART_WordLength_9b == config.WordLength) {
                conf += "9-";
            }
            else {
                conf += "8-";
            }

            // Parity
            if(USART_Parity_Odd == config.Parity) {
                conf += "O-";
            }
            else if(USART_Parity_Even == config.Parity) {
                conf += "E-";
            }
            else {
                conf += "N-";
            }

            // Stop Bits
            switch(config.StopBits) {
                case USART_StopBits_0_5:
                    conf += "0.5\r\n";
                    break;

                case USART_StopBits_1_5:
                    conf += "1.5\r\n";
                    break;

                case USART_StopBits_2:
                    conf += "2\r\n";
                    break;

                default:
                    conf += "1\r\n";
            };

            // File Size in KB
            conf += std::to_string(config.FileSize) + "\r\n";

            // Time
            if(config.AddTime) {
                conf += "1";
            }
            else {
                conf += "0";
            }
            conf += "\r\n\r\n\r\n";

            // Add build date
            conf += "Build Date:\r\n";
            conf += __BUILD__DATE__;

            uint32_t len;
            res = f_write(&file, conf.c_str(), conf.length(), &len);

            f_close(&file);

            // Check write size
            if(len != conf.length()) {
                return false;
            }
        }
    }
    return (res == FR_OK);
}

/**
 * @brief  Set automatic setting for USART
 * @param  init - configuration from user file 
 * @retval None
 */
void Config::AutoSetting(USART_InitTypeDef* init)
{
    // Supported interface baud rate
    std::vector<uint32_t> baudrate{
        600,
        1200,
        2400,
        4800,
        9600,
        14400,
        19200,
        28800,
        38400,
        56000,
        57600,
        115200,
    };

    // Descending sort validation to prevent errors
    std::sort(baudrate.begin(), baudrate.end());
    std::reverse(baudrate.begin(), baudrate.end());

    // Create iterator
    auto itBaudRate = baudrate.cbegin();
    init->USART_BaudRate = *itBaudRate;

    // Create and initialisation class UART
    Main::VPortUart =
        new Uart(UART_DATA_SOURCE, init, TX_BUFFER_SIZE, RX_BUFFER_SIZE);

    uint_fast8_t success = 0;
    constexpr uint_fast8_t attempt = 5;
    while(success < attempt) {
        IWDG_ReloadCounter();
        constexpr time_t SCAN_USART = 10;
        System::DelayMS(SCAN_USART);
        if(static_cast<Uart*>(Main::VPortUart)->IsError()) {
            if(++itBaudRate >= baudrate.cend()) {
                itBaudRate = baudrate.cbegin();
            }
            init->USART_BaudRate = *itBaudRate;

            static_cast<Uart*>(Main::VPortUart)->InitUart(init);
            Main::VPortUart->CleanReceive();
            success = 0;
        }
        else {
            if(Main::VPortUart->GetLen()) {
                success++;
                Main::VPortUart->CleanReceive();
                System::DelayMS(SCAN_USART);
            }
        }
    }
}

/**
 * @brief  Set manual setting for USART
 * @param  init - configuration from user file 
 * @retval None
 */
void Config::ManualSetting(USART_InitTypeDef* init)
{
    Main::VPortUart =
        new Uart(UART_DATA_SOURCE, init, TX_BUFFER_SIZE, RX_BUFFER_SIZE);
}
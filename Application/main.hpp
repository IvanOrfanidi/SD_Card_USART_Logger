/**
 ******************************************************************************
 * @file    main.hpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    28-March-2018
 * @brief   This file main programs
 ******************************************************************************
 * @attention
 *
 *
 *
 ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_HPP
#define __MAIN_HPP

/* Includes ------------------------------------------------------------------*/
/* User */
#include "file.hpp"
#include "disk.hpp"
#include "config.hpp"

/* Divers */
#include "system.hpp"
#include "board.hpp"
#include "uart.hpp"
#include "exti.hpp"
#include "virtual_port.hpp"
#include "rtc.hpp"
#include "nvic.hpp"
#include "soft_timer.hpp"

/* SD Card */
#include "sdcard.h"
#include "ff.h"
#include "diskio.h"

/* USB */
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "usb_istr.h"

#ifdef __cplusplus

/* Standart */
#include <stdint.h>
#include <time.h>
#include <vector>

// Filename extension
constexpr auto EXTENSION_LOG_FILE = ".txt";

/**
 * @brief Class Main
 */
class Main {
  public:
    /// Default value
    enum Default_t {
        NUM_DRV = 0,

        MAX_TIME_TO_SCAN_SDCARD = 10,    ///< Maximum time to scan sd card
    };

    /// All delays in msec
    enum Delay_t : time_t {
        BEFORE_REBOOT_DEVICE = 500,
        SCAN_SDCARD = 100,
        ON_ERROR = 250
    };

    /// Status
    enum Status_t {
        NO_ERROR,
        ERR_WRITE_PROTECTED,
        ERR_MOUNT_DISK,
        ERR_UNMOUNT_DISK,
        ERR_SD_CARD,
        ERR_CREATE_CONFIG_FILE,
        ERR_CREATE_FILE,
        ERR_OPEN_FILE,
        ERR_MOVE_POINTER_TO_END,
        ERR_WRITE_TO_FILE
    };

    /// File configuration
    struct File_t {
        std::string Name;
        size_t MaxSize;
    };

    Main();

    static void SdTask();    /// Interrupt Handler SDIO

    static void UsbCtrHp();    /// Interrupt Handler USB

    static void UsbIstr();    /// Interrupt Handler USB

    size_t GetSizeFile() const;

    static class VirtualPort* VPortUart;    // Virtual port interface

    SD_CardInfo SDCardInfo;    // SD Card Information

  private:
    void CreateNameFileLog(std::string&);

    void Echo(const std::vector<uint8_t>&);    // Transmit echo to USART

    Status_t WriteLog(File_t&, const std::vector<uint8_t>&);

    void Exception(Status_t);

    uint8_t _Drv;    // Number driver

    class File _Log;

    class Disk _Disk;

    Rtc* _Rtc;
};

extern "C" {
}

#endif    //__cplusplus

#endif
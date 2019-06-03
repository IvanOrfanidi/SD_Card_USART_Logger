/**
 ******************************************************************************
 * @file    file.hpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    28-March-2018
 * @brief   This file file programs
 ******************************************************************************
 * @attention
 *
 *
 *
 ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISK_HPP
#define __DISK_HPP

/* Includes ------------------------------------------------------------------*/
/* Standart lib */
#include <stdio.h>
#include <stdint.h>

/* SD Card */
#include "ff.h"

#ifdef __cplusplus

class Disk {
  public:
    Disk();                           /// Ñonstructor
    Disk(const Disk& a) = delete;     /// Ñonstructor copy is delete
    Disk(const Disk&& a) = delete;    /// Ñonstructor move is delete

    Disk& operator=(const Disk&) = delete;    /// Operator copy is delete
    Disk& operator=(Disk&&) = delete;         /// Operator move is delete

    Disk(uint8_t);    /// Ñonstructor

    ~Disk();

    bool Mount();    /// Mount disk

    bool Unmount();    /// Unmount disk

    bool IsMount() const;

    bool Formatt() const;

  private:
    FATFS _Fs;    // File System

    uint8_t _Drv;    // Number driver

    bool _Mount;
};

extern "C" {
}

#endif    //__cplusplus

#endif
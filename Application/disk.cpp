/**
 ******************************************************************************
 * @file    disk.cpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    04-2019
 * @brief   This is file disk.
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
/* User lib */
#include "disk.hpp"

/* SD Card */
#include "diskio.h"

Disk::Disk() : _Drv(0), _Mount(false) {}

Disk::Disk(uint8_t drv) : _Drv(drv), _Mount(false) {}

Disk::~Disk()
{
    Unmount();
}

/**
 * @brief  Mount disk
 * @retval true - success, false - fail
 */
bool Disk::Mount()
{
    if(!_Mount) {
        const auto res = f_mount(&_Fs, "", NULL);
        if(FR_OK == res) {
            _Mount = true;
        }
        return (FR_OK == res);
    }
    return true;
}

/**
 * @brief  Unmount disk
 * @retval true - success, false - fail
 */
bool Disk::Unmount()
{
    if(_Mount) {
        // Sync Drv
        while(disk_ioctl(_Drv, CTRL_SYNC, (void*)NULL) != RES_OK) {
        }

        // Unmount disk
        const auto res = f_mount(NULL, "", NULL);
        if(FR_OK == res) {
            _Mount = false;
        }
        return (FR_OK == res);
    }
    return true;
}

/**
 * @brief  Unmount disk
 * @retval true - success, false - fail
 */
bool Disk::Formatt() const
{
    const auto res = f_mkfs("", FM_ANY, 0, 0, 0);
    return (FR_OK == res);
}

/**
 * @brief  Get status disk
 * @retval true - mount, false - unmount
 */
bool Disk::IsMount() const
{
    return _Mount;
}
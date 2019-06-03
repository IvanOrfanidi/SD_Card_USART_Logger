/**
 ******************************************************************************
 * @file    file.cpp
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
/* Standart lib */
#include <stdio.h>

/* User lib */
#include "diskio.h"
#include "file.hpp"

//
File::File() : _Access(0), _Drv(0) {}
File::File(uint8_t drv) : _Drv(drv), _Access(0) {}

/**
 * @brief Ñonstructor
 */
File::File(const char* name, uint8_t drv) : _Drv(drv), _Access(0)
{
    // Create new file
    Create(name);
}

/**
 * @brief Ñonstructor
 */
File::File(const char* name, uint8_t drv, Access_t access) : _Drv(drv)
{
    AssigningAccess(access);

    // Create new file
    Create(name);
}

/**
 * @brief Destructor
 */
File::~File()
{
    Close();
}

void File::SetNumberDriver(uint8_t drv)
{
    _Drv = drv;
}

bool File::CreateAlways(const char* name, Access_t access)
{
    _NameFile = name;
    if(NO_ACCESS != access) {
        AssigningAccess(access);
    }

    // Create always new file
    do {
        SyncDrv();
        _Result = f_open(&_FileObject, _NameFile.c_str(), FA_CREATE_ALWAYS);

    } while(FR_OK != _Result);

    Close();

    return (FR_OK == _Result);
}

bool File::Create(const char* name, Access_t access)
{
    _NameFile = name;
    if(NO_ACCESS != access) {
        AssigningAccess(access);
    }

    SyncDrv();
    _Result = f_open(&_FileObject, _NameFile.c_str(), FA_OPEN_EXISTING);
    if(FR_OK != _Result) {
        return false;
    }

    // Create new file
    _Result = f_open(&_FileObject, _NameFile.c_str(), FA_CREATE_NEW);

    if(FR_OK == _Result) {
        Close();
    }

    return (FR_OK == _Result);
}

FRESULT File::GetResult() const
{
    return _Result;
}

/**
 * @brief  Get size file
 * @retval size file
 */
size_t File::GetSize() const
{
    return f_size(&_FileObject);
}

/**
 * @brief  Close file
 * @retval true - success, false - fail
 */
bool File::Close()
{
    SyncDrv();
    _Result = f_close(&_FileObject);
    return (FR_OK == _Result);
}

/**
 * @brief 
 * @retval true - success, false - fail
 */
void File::SyncDrv() const
{
    //    while(disk_ioctl(_Drv, CTRL_SYNC, (void*)NULL) != RES_OK) {
    //    }
}

/**
 * @brief  Write data to end file
 * @retval true - success, false - fail
 */
bool File::Write(const uint8_t* buff, size_t rd_len)
{
    int attempts = MAX_ATTEMPTS_TO_WRITE_FILE;
    uint32_t wr_len = 0;
    while(rd_len > wr_len) {
        SyncDrv();
        _Result = f_write(&_FileObject, &buff[wr_len], rd_len, &wr_len);
        if((0 == --attempts) || (FR_OK != _Result)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief  Move pointer to end file
 * @retval true - success, false - fail
 */
bool File::MovePointerToEnd()
{
    do {
        SyncDrv();
        _Result = f_lseek(&_FileObject, f_size(&_FileObject));
    } while(FR_OK != _Result);

    return (FR_OK == _Result);
}

/**
 * @brief  Open file
 * @brief access - file access
 * @retval true - success, false - fail
 */
bool File::Open(Access_t access)
{
    if(!AssigningAccess(access)) {
        return false;
    }

    SyncDrv();
    _Result =
        f_open(&_FileObject, _NameFile.c_str(), FA_OPEN_EXISTING | _Access);

    return (FR_OK == _Result);
}

/**
 * @brief  Open file
 * @brief access - file access
 * @retval true - success, false - fail
 */
bool File::Open()
{
    if(NO_ACCESS == _Access) {
        return false;
    }

    SyncDrv();
    _Result =
        f_open(&_FileObject, _NameFile.c_str(), FA_OPEN_EXISTING | _Access);

    return (FR_OK == _Result);
}

/**
 * @brief Always open files
 * @brief access - file access
 * @retval true - success, false - fail
 */
bool File::OpenAlways(Access_t access)
{
    if(!AssigningAccess(access)) {
        return false;
    }

    do {
        SyncDrv();
        _Result =
            f_open(&_FileObject, _NameFile.c_str(), FA_OPEN_ALWAYS | _Access);
    } while(FR_OK != _Result);

    return (FR_OK == _Result);
}

/**
 * @brief Always open files
 * @brief access - file access
 * @retval true - success, false - fail
 */
bool File::OpenAlways()
{
    if(NO_ACCESS == _Access) {
        return false;
    }

    do {
        SyncDrv();
        _Result =
            f_open(&_FileObject, _NameFile.c_str(), FA_OPEN_ALWAYS | _Access);
    } while(FR_OK != _Result);

    return (FR_OK == _Result);
}

bool File::AssigningAccess(Access_t access)
{
    if(WRITE == access) {
        _Access = FA_WRITE;
    }
    else if(READ == access) {
        _Access = FA_READ;
    }
    else if(WRITE_AND_READ == access) {
        _Access = FA_WRITE | FA_READ;
    }
    else {
        _Access = 0;
        return false;
    }
    return true;
}
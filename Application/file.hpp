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
#ifndef __FILE_HPP
#define __FILE_HPP

/* Includes ------------------------------------------------------------------*/
/* SD Card */
#include "ff.h"

#ifdef __cplusplus
//
#include <string>

/**
 * @brief Class File
 */
class File {
  public:
    enum Access_t {
        NO_ACCESS,        ///< No access
        WRITE,            ///< Only write
        READ,             ///< Only read
        WRITE_AND_READ    ///< Write and read
    };

    File();
    File(const File& a) = delete;     /// Ñonstructor copy is delete
    File(const File&& a) = delete;    /// Ñonstructor move is delete

    File& operator=(const File&) = delete;    /// Operator copy is delete
    File& operator=(File&&) = delete;         /// Operator move is delete

    File(uint8_t);
    File(const char*, uint8_t);
    File(const char*, uint8_t, Access_t);

    ~File();

    void SetNumberDriver(uint8_t);

    size_t GetSize() const;

    bool Create(const char*, Access_t access = NO_ACCESS);

    bool CreateAlways(const char*, Access_t access = NO_ACCESS);

    bool Open(Access_t);

    bool Open();

    bool OpenAlways(Access_t);

    bool OpenAlways();

    bool Close();

    bool Write(const uint8_t*, size_t);    /// Write data to end file

    bool MovePointerToEnd();    /// Move pointer to end file

    FRESULT GetResult() const;

  private:
    enum {
        MAX_ATTEMPTS_TO_WRITE_FILE = 5    ///< Maximum attempts to write file
    };

    /// Funct is called to control device specific features and miscellaneous
    void SyncDrv() const;

    bool AssigningAccess(Access_t);

    FIL _FileObject;    // File Object

    uint8_t _Drv;    // Number driver

    uint8_t _Access;

    FRESULT _Result;

    std::string _NameFile;
};

extern "C" {
}

#endif    //__cplusplus

#endif

/* Standart lib */
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/* User lib */
#include "main.hpp"

/* Global --------------------------------------------------------------------*/
uint32_t Mass_Memory_Size = 0;
uint32_t Mass_Block_Size = 0;
uint32_t Mass_Block_Count = 0;

/**
* Global class Virtual Port for <cout>
*/
VirtualPort* Main::VPortUart = nullptr;

// Initialisation params SD Card
static void InitSdCardCharacteristics(SD_CardInfo& info);

/**
 * @brief General functions main
 */
int main()
{
    Main();
}

/**
 * @brief General functions main in class
 */
Main::Main() : _Drv(NUM_DRV), _Log(_Drv), _Disk(_Drv)
{
    // Update System clock Core
    System::ClockUpdate();

    // Set NVIC Priority Group (4 bits for preemption priority, 0 bits for subpriority)
    System::SetNvicPriorityGroup(NVIC_PriorityGroup_4);

    // Initialisation System Timer
    System::InitSysTick(1000);


    // Initialisation Led
    Board::InitGPIO();

    // Initialisation RTC
    _Rtc = Rtc::GetInstance();
    //_Rtc->SetTime(1554638723 + 10800);

    // Initialisation Watchdog Timer
    Board::InitIWDG();

    /* Check the SD card in the tray */
    bool connect = false;
    while(!Board::GetCardDetect()) {
        if(Board::GetUsbConnect()) {
            connect = true;
            break;
        }
        // Card is not, wait SD Card
        Board::LedErrOn();
        IWDG_ReloadCounter();
        System::DelayMS(ON_ERROR);
    }

    /* Attention! Если есть подключение к USB, то преходим без проверки карты */
    if(!Board::GetUsbConnect()) {
        /* Check that SD card is allowed for recording */
        for(auto i = 0; i < MAX_TIME_TO_SCAN_SDCARD; i++) {
            IWDG_ReloadCounter();
            System::DelayMS(SCAN_SDCARD);
            if(!Board::GetWriteDaisable()) {
                IWDG_ReloadCounter();
                System::DelayMS(SCAN_SDCARD);
                break;
            }
        }
    }

    if(!connect && Board::GetWriteDaisable()) {
        // Write protection for card
        Exception(ERR_WRITE_PROTECTED);
    }

    // Ennable interrupt SDIO
    Nvic* SdIrg = new Nvic(SDIO_IRQn, 0, 0, Main::SdTask);
    IWDG_ReloadCounter();

    // Initialisation SD Card interface
    if(InitSdioFats(&SDCardInfo) != SD_OK) {
        Exception(ERR_SD_CARD);
    }

    /* Check the connection to the USB */
    if(Board::GetUsbConnect()) {
        Board::LedRunOn();
        Board::LedErrOn();

        // Initialisation the microSD card size
        InitSdCardCharacteristics(SDCardInfo);

        // Initialisation USB Clock
        System::SetClockUSB(RCC_USBCLKSource_PLLCLK_1Div5);

        /* Configures the USB interrupts */
        Nvic* istr = new Nvic(USB_LP_CAN1_RX0_IRQn, 0, 0, Main::UsbIstr);
        Nvic* ctr = new Nvic(USB_HP_CAN1_TX_IRQn, 0, 0, Main::UsbCtrHp);

        // Initialisation USB Device
        InitUSB();

        while(Board::GetUsbConnect() && Board::GetCardDetect()) {
            IWDG_ReloadCounter();
        }

        while(!Board::GetCardDetect()) {
            IWDG_ReloadCounter();
        }

        System::DelayMS(BEFORE_REBOOT_DEVICE);
        // Reboot Device
        NVIC_SystemReset();
    }

    Board::LedRunOn();
    Board::LedErrOff();

    bool res = _Disk.Mount();
    if(!res) {
        // Error File
        Exception(ERR_MOUNT_DISK);
    }

    /* Read configurations from file*/
    Config::Config_t config;    // Configuration
    if(Config::ReadFile(&config) == false) {
        // If there is no configuration file, we will create it.
        do {
            res = Config::CreateFile(config);
            if(!res) {
                // Failed to create the file, try to format the card
                //_Disk.Formatt();
                Exception(ERR_CREATE_CONFIG_FILE);
            }
        } while(!res);

        /* Remount the drive */
        res = _Disk.Unmount();
        if(!res) {
            Exception(ERR_UNMOUNT_DISK);
        }
        res = _Disk.Mount();
        if(!res) {
            Exception(ERR_MOUNT_DISK);
        }
    }

    /* Configure File Log */
    File_t file;
    file.MaxSize = config.FileSize * 1024;

    /* Configure USART */
    USART_InitTypeDef init;
    init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    init.USART_WordLength = config.WordLength;
    init.USART_StopBits = config.StopBits;
    init.USART_Parity = config.Parity;
    init.USART_BaudRate = config.Baudrate;

    /* Initialisation USART/UART */
    if(Config::AUTO_BAUDRATE == config.Baudrate) {
        // Auto
        Config::AutoSetting(&init);
    }
    else {
        // Manual
        Config::ManualSetting(&init);
    }
    VPortUart->CleanReceive();

    // Create data buffer
    std::vector<uint8_t> buffer(FF_MAX_SS);
    auto it = buffer.cbegin();

    /* General loop */
    while(true) {
        IWDG_ReloadCounter();

        /* Check the connection to the USB */
        if(Board::GetUsbConnect()) {
            // Reboot device for USB/PC connect
            NVIC_SystemReset();
        }

        /* Проверяем UART на приход данных кратное размеру сектора */
        const size_t len = VPortUart->GetLen();
        if((!(len % FF_MAX_SS)) && (0 != len)) {
            // There is data
            do {
                IWDG_ReloadCounter();
                Board::LedRunOff();

                const uint32_t rd_len = (len > FF_MAX_SS) ? FF_MAX_SS : len;
                VPortUart->Receive(&buffer[0], rd_len);

                // Transmit echo to USART
                Echo(buffer);

                // Write Log to file
                const Status_t status = WriteLog(file, buffer);

                // Сheck write status
                if(NO_ERROR != status) {
                    Exception(status);
                }

                // So Good
                Board::LedRunOn();

            } while(len > FF_MAX_SS);
        }

        /* Unmount Disk */
        if(!Board::GetCardDetect()) {
            Board::LedRunOff();
            Board::LedErrOn();
            while(!Board::GetCardDetect()) {
                IWDG_ReloadCounter();
            }
            // Reboot Device
            NVIC_SystemReset();
        }
    }
}

/**
 * @brief  Create name new File Log
 * @param [out] name - name new file
 * @retval None
 */
void Main::CreateNameFileLog(std::string& name)
{
    // Get date
    RTC_t date;
    _Rtc->GetTime(&date);

    // New name file
    sprintf(&name[0],
        "%02d.%02d %02d.%02d.%02d%s",
        date.Hour,
        date.Min,
        date.Mday,
        date.Month,
        date.Year,
        EXTENSION_LOG_FILE);
}

/**
 * @brief  Transmit echo to USART
 * @param [in] buffer - buffer vector data
 * @retval None
 */
void Main::Echo(const std::vector<uint8_t>& buffer)
{
    for(auto echo : buffer) {
        std::cout << echo;
    }
}

/**
 * @brief  Transmit echo to USART
 * @param [in/out] file - file info, name and max size
 * @param [in] buffer - buffer data
 * @param [out] error - error
 * @retval None
 */
Main::Status_t Main::WriteLog(File_t& file, const std::vector<uint8_t>& buffer)
{
    Status_t status = NO_ERROR;
    bool res = _Disk.Mount();
    if(res) {
        // Create a file at the start of the program and data
        if(file.Name.length() == 0) {
            /* Create new file and close it */
            CreateNameFileLog(file.Name);
            _Log.CreateAlways(file.Name.c_str());
        }

        // Open the file to record the log !critical section!
        res = _Log.OpenAlways(File::WRITE_AND_READ);
        if(res) {
            // Check the file size and if it is exceeded, then create a new one.
            const auto size = _Log.GetSize();
            if(size >= file.MaxSize) {
                // Close current file
                _Log.Close();

                // Create new file
                CreateNameFileLog(file.Name);
                _Log.CreateAlways(file.Name.c_str());

                // Open it
                res = _Log.OpenAlways(File::WRITE_AND_READ);
            }

            // Write data to file
            if(res) {
                // Move the record pointer to the end of the file
                res = _Log.MovePointerToEnd();
                if(res) {
                    // Write data in multiples of the sector size in the end file
                    res = _Log.Write(&buffer[0], buffer.size());
                    if(!res) {
                        status = ERR_WRITE_TO_FILE;
                    }
                }
                else {
                    status = ERR_MOVE_POINTER_TO_END;
                }

                // Close file
                _Log.Close();
                _Disk.Unmount();
            }
            else {
                status = ERR_CREATE_FILE;
            }
        }
        else {
            status = ERR_OPEN_FILE;
        }
    }
    else {
        status = ERR_MOUNT_DISK;
    }

    return status;
}

/**
 * @brief  Exception Handler for Errors
 * @retval None
*/
void Main::Exception(const Status_t error)
{
    if(NO_ERROR == error) {
        return;
    }

    bool reset = false;
    Board::LedRunOff();
    while(true) {
        Board::LedErrOn();
        IWDG_ReloadCounter();
        System::DelayMS(ON_ERROR);

        if(Board::GetCardDetect()) {
            Board::LedErrOff();
            IWDG_ReloadCounter();
            System::DelayMS(ON_ERROR);
        }
        else {
            reset = true;
        }

        // Reboot Device
        if(Board::GetCardDetect() && reset) {
            IWDG_ReloadCounter();
            System::DelayMS(BEFORE_REBOOT_DEVICE);
            NVIC_SystemReset();
        }
    }
}

/**
 * @brief  Initialisation the microSD card size
 * @retval None
 */
void InitSdCardCharacteristics(SD_CardInfo& info)
{
    auto a = info.SD_csd.DeviceSize + 1;
    auto b = 1 << (info.SD_csd.DeviceSizeMul + 2);

    Mass_Block_Count = a * b;
    Mass_Block_Size = 1 << info.SD_csd.RdBlockLen;
    Mass_Memory_Size = (Mass_Block_Count * Mass_Block_Size);
}

/**
 * @brief  Interrupt Handler SDIO 
 * @retval None
 */
void Main::SdTask()
{
    /* Process All SDIO Interrupt Sources */
    SD_ProcessIRQSrc();
}

/**
 * @brief  Interrupt Handler USB
 * @retval None
 */
void Main::UsbCtrHp()
{
    CTR_HP();
}

/**
 * @brief  Interrupt Handler USB
 * @retval None
 */
void Main::UsbIstr()
{
    USB_Istr();
}

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE* f)
#endif /* __GNUC__ */

/* Переопределение функции putchar  */
PUTCHAR_PROTOTYPE
{
    Main::VPortUart->Transmit((const uint8_t*)&ch, sizeof(uint8_t));
    return ch;
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval : None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
      ex: ("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while(1) {
    }
}
#endif
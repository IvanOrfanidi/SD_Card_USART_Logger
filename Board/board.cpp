/**
 ******************************************************************************
 * @file    board.cpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    04-2019
 * @brief   This is file realise driver board.
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.hpp"
#include "stm32f10x_conf.h"

/**
 * @brief  Initialisation Led.
 * @retval None.
 */
void Board::InitGPIO()
{
    // Init Clock
    RCC_APB2PeriphClockCmd(PORT_WD_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(PORT_CD_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(PORT_RUN_LED_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(PORT_ERR_LED_CLK, ENABLE);

    // Init Status Pin SD Card
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_InitStructure.GPIO_Pin = WD_PIN;
    GPIO_Init(PORT_WD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CD_PIN;
    GPIO_Init(PORT_CD, &GPIO_InitStructure);

    // Init USB Pin Connect
    GPIO_InitStructure.GPIO_Pin = USB_CONNECT_PIN;
    GPIO_Init(PORT_USB_CONNECT, &GPIO_InitStructure);

    // Init Led Light Pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Pin = LED_RUN_PIN;
    GPIO_Init(PORT_RUN_LED, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_ERR_PIN;
    GPIO_Init(PORT_ERR_LED, &GPIO_InitStructure);

    GPIO_SetBits(PORT_RUN_LED, LED_RUN_PIN);
    GPIO_SetBits(PORT_ERR_LED, LED_ERR_PIN);
}

/**
 * @brief  Enable Led.
 * @retval None.
 */
void Board::LedErrOn()
{
    GPIO_ResetBits(PORT_ERR_LED, LED_ERR_PIN);
}

/**
 * @brief  Disable Led.
 * @retval None.
 */
void Board::LedErrOff()
{
    GPIO_SetBits(PORT_ERR_LED, LED_ERR_PIN);
}

/**
 * @brief  Enable Led.
 * @retval None.
 */
void Board::LedRunOn()
{
    GPIO_ResetBits(PORT_RUN_LED, LED_RUN_PIN);
}

/**
 * @brief  Disable Led.
 * @retval None.
 */
void Board::LedRunOff()
{
    GPIO_SetBits(PORT_RUN_LED, LED_RUN_PIN);
}

bool Board::GetWriteDaisable()
{
    return (GPIO_ReadInputDataBit(PORT_WD, WD_PIN) == Bit_SET);
}

bool Board::GetCardDetect()
{
    return (GPIO_ReadInputDataBit(PORT_CD, CD_PIN) == Bit_RESET);
}

bool Board::GetUsbConnect()
{
    return (
        GPIO_ReadInputDataBit(PORT_USB_CONNECT, USB_CONNECT_PIN) == Bit_SET);
}

/**
 * @brief  Initialisation Backup.
 * @retval None.
 */
void Board::InitBKP()
{
    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);

    /* Enable write access to Backup domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Clear Tamper pin Event(TE) pending flag */
    BKP_ClearFlag();
}

/**
 * @brief  Initialisation Watchdog timer.
 * @retval None.
 */
void Board::InitIWDG()
{
    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
    }

    /* IWDG timeout equal to 2000 ms (the timeout may varies due to LSI
    frequency dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_256);

/* Set counter reload value to obtain 250ms IWDG TimeOut.
   Counter Reload Value = 250ms/IWDG counter clock period
                        = 250ms / (LSI/32)
                        = 0.25s / (LsiFreq/32)
                        = LsiFreq/(32 * 4)
                        = LsiFreq/128
 */
#define LSI_FREQ 40000
    IWDG_SetReload(LSI_FREQ / 64);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}

/**
 ******************************************************************************
 * @file    Audio_playback_and_record/src/usbh_usr.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    28-October-2011
 * @brief   This file includes the usb host user callbacks
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_usr.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"

/** @addtogroup STM32F4-Discovery_Audio_Player_Recorder
 * @{
 */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t Command_index = 0;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
FATFS fatfs;
FIL fileR;
DIR dir;
FILINFO fno;

extern volatile int enum_done, usb_stan;

USBH_Usr_cb_TypeDef USR_Callbacks =
{
		USBH_USR_Init,
		USBH_USR_DeInit,
		USBH_USR_DeviceAttached,
		USBH_USR_ResetDevice,
		USBH_USR_DeviceDisconnected,
		USBH_USR_OverCurrentDetected,
		USBH_USR_DeviceSpeedDetected,
		USBH_USR_Device_DescAvailable,
		USBH_USR_DeviceAddressAssigned,
		USBH_USR_Configuration_DescAvailable,
		USBH_USR_Manufacturer_String,
		USBH_USR_Product_String,
		USBH_USR_SerialNum_String,
		USBH_USR_EnumerationDone,
		USBH_USR_UserInput,
		USBH_USR_MSC_Application,
		USBH_USR_DeviceNotSupported,
		USBH_USR_UnrecoveredError
};

/**
 * @brief  USBH_USR_Init
 * @param  None
 * @retval None
 */
void USBH_USR_Init(void)
{
	usb_stan=1;
}

/**
 * @brief  USBH_USR_DeviceAttached
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAttached(void)
{
	usb_stan=2;
	//	GPIO_SetBits(GPIOD, GPIO_Pin_14);
}

/**
 * @brief  USBH_USR_UnrecoveredError
 * @param  None
 * @retval None
 */
void USBH_USR_UnrecoveredError (void)
{
	usb_stan=3;
}

/**
 * @brief  USBH_DisconnectEvent
 *         Device disconnect event
 * @param  None
 * @retval Staus
 */
void USBH_USR_DeviceDisconnected (void)
{
	usb_stan=4;
//	GPIO_ResetBits(GPIOD, GPIO_Pin_14);
//	GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	enum_done = 0;
}

/**
 * @brief  USBH_USR_ResetUSBDevice
 * @param  None
 * @retval None
 */
void USBH_USR_ResetDevice(void)
{
	usb_stan=5;
	/* callback for USB-Reset */
}


/**
 * @brief  USBH_USR_DeviceSpeedDetected
 *         Displays the message on LCD for device speed
 * @param  Device speed:
 * @retval None
 */
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	usb_stan=6;
}

/**
 * @brief  USBH_USR_Device_DescAvailable
 * @param  device descriptor
 * @retval None
 */
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
	usb_stan=7;
	/* callback for device descriptor */
}

/**
 * @brief  USBH_USR_DeviceAddressAssigned
 *         USB device is successfully assigned the Address
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAddressAssigned(void)
{
	usb_stan=8;
	/* callback for device successfully assigned the Address */
}

/**
 * @brief  USBH_USR_Conf_Desc
 * @param  Configuration descriptor
 * @retval None
 */
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
		USBH_InterfaceDesc_TypeDef *itfDesc,
		USBH_EpDesc_TypeDef *epDesc)
{
	usb_stan=9;
	/* callback for configuration descriptor */
}

/**
 * @brief  USBH_USR_Manufacturer_String
 * @param  Manufacturer String
 * @retval None
 */
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	usb_stan=10;
	/* callback for  Manufacturer String */
}

/**
 * @brief  USBH_USR_Product_String
 * @param  Product String
 * @retval None
 */
void USBH_USR_Product_String(void *ProductString)
{
	usb_stan=11;
	/* callback for Product String */
}

/**
 * @brief  USBH_USR_SerialNum_String
 * @param  SerialNum_String
 * @retval None
 */
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	usb_stan=12;
	/* callback for SerialNum_String */
}

/**
 * @brief  EnumerationDone
 *         User response request is displayed to ask application jump to class
 * @param  None
 * @retval None
 */
void USBH_USR_EnumerationDone(void)
{
	usb_stan=13;
	/* 0.5 seconds delay */
	USB_OTG_BSP_mDelay(500);
	USBH_USR_MSC_Application();
} 

/**
 * @brief  USBH_USR_DeviceNotSupported
 *         Device is not supported
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceNotSupported(void)
{
	usb_stan=14;
}


/**
 * @brief  USBH_USR_UserInput
 *         User Action for application state entry
 * @param  None
 * @retval USBH_USR_Status : User response for key button
 */
USBH_USR_Status USBH_USR_UserInput(void)
{
	usb_stan=15;
	/* callback for Key botton: set by software in this case */
	return USBH_USR_RESP_OK;
}

/**
 * @brief  USBH_USR_OverCurrentDetected
 *         Over Current Detected on VBUS
 * @param  None
 * @retval None
 */
void USBH_USR_OverCurrentDetected (void)
{
	usb_stan=16;
}

/**
 * @brief  USBH_USR_MSC_Application
 * @param  None
 * @retval Staus
 */
int USBH_USR_MSC_Application(void)
{
	usb_stan=17;
	if (f_mount( 0, &fatfs ) != FR_OK )
	{
		/* efs initialisation fails*/
		return(-1);
	}

	enum_done++;
//	GPIO_SetBits(GPIOD, GPIO_Pin_12);
	return(0);
}

/**
 * @brief  USBH_USR_DeInit
 *         Deint User state and associated variables
 * @param  None
 * @retval None
 */
void USBH_USR_DeInit(void)
{
	usb_stan=18;
}

/**
 * @}
 */



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

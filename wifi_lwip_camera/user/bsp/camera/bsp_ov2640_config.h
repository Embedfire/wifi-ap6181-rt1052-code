/*
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FSL_OV5640_H_
#define _FSL_OV5640_H_

#include "fsl_common.h"
#include "fsl_sccb.h"
#include "fsl_camera_device.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 
//#define OV2640_SCCB_ADDR 0x60U		//OV2640
//#define OV2640_SCCB_ADDR_Read 0x60U		//OV2640
#define OV2640_SCCB_ADDR 0x30U		//OV2640
#define OV2640_SCCB_ADDR_Read 0x30U		//OV2640
#define OV2640_WriteReg(i2c, reg, val) SCCB_WriteReg(i2c, OV2640_SCCB_ADDR, kSCCB_RegAddr8Bit, (reg), (val))
#define OV2640_ReadReg(i2c, reg, val) SCCB_ReadReg(i2c, OV2640_SCCB_ADDR_Read, kSCCB_RegAddr8Bit, (reg), (val))

/*!
 * @brief OV2640 resource.
 *
 * Before initialize the OV5640, the resource must be initialized that the SCCB
 * I2C could start to work.
 */
typedef struct _ov2640_resource
{
    sccb_i2c_t sccbI2C;                    /*!< I2C for SCCB transfer. */
    void (*pullResetPin)(bool pullUp);     /*!< Function to pull reset pin high or low. */
    void (*pullPowerDownPin)(bool pullUp); /*!< Function to pull the power down pin high or low. */
    uint32_t inputClockFreq_Hz;            /*!< Input clock frequency. */
} ov2640_resource_t;

extern uint32_t activeFrameAddr;
extern uint32_t inactiveFrameAddr;

/*! @brief OV2640 operation functions. */
extern const camera_device_operations_t ov2640_ops;
extern void CSI_DriverIRQHandler(void);
/* Image Sizes enumeration */
//typedef enum   
//{
//  BMP_QQVGA             =   0x00,	    /* BMP Image QQVGA 160x120 Size */
//  BMP_QVGA              =   0x01,      /* BMP Image VGA 800*480 Size */
//  JPEG_160x120          =   0x02,	    /* JPEG Image 160x120 Size */
//  JPEG_176x144          =   0x03,	    /* JPEG Image 176x144 Size */
//  JPEG_320x240          =   0x04,	    /* JPEG Image 320x240 Size */
//  JPEG_352x288          =   0x05	    /* JPEG Image 352x288 Size */
//}ImageFormat_TypeDef;


/* Image Sizes enumeration */
typedef enum
{
    BMP_QQVGA             =   0x00,	    /* BMP Image QQVGA 160x120 Size */
    BMP_QVGA              =   0x01,      /* BMP Image VGA 800*480 Size */
    JPEG_160x120          =   0x02,	    /* JPEG Image 160x120 Size */
    JPEG_176x144          =   0x03,	    /* JPEG Image 176x144 Size */
    JPEG_320x240          =   0x04,	    /* JPEG Image 320x240 Size */
    JPEG_352x288          =   0x05,	    /* JPEG Image 352x288 Size */
    JPEG_640x480          =   0x06,
    JPEG_800x600          =   0x07,     /* JPEG Image 800x600 Size */
    JPEG_1024x768         =   0x08     	/* JPEG Image 1024x768 Size */
}ImageFormat_TypeDef;

#define JPEG_IMAGE_FORMAT  JPEG_320x240
//#define JPEG_IMAGE_FORMAT  JPEG_640x480
//#define JPEG_IMAGE_FORMAT  JPEG_800x600
//#define JPEG_IMAGE_FORMAT  JPEG_1024x768


#define FRAME_RATE_30FPS	0 //30֡
#define FRAME_RATE_15FPS	1 //15֡

/*����ͷ���ýṹ��*/
typedef struct
{	
	uint16_t cam_out_width;//���ͼ��ֱ��ʣ���
	uint16_t cam_out_height;//���ͼ��ֱ��ʣ���
	
	int lcd_sx;//ͼ����ʾ��Һ������X��ʼλ��
	int lcd_sy;//ͼ����ʾ��Һ������Y��ʼλ��
	uint8_t lcd_scan;//Һ������ɨ��ģʽ��0-7��
		
	uint8_t illuminance;//���ն�
	uint8_t contrast;//�Աȶ�
	uint8_t light_mode;//����ģʽ
	uint8_t effect;	//����Ч��


}OV2640_MODE_PARAM;
extern OV2640_MODE_PARAM Set_Cam_mode(int temp);
extern OV2640_MODE_PARAM cam_mode;
extern OV2640_MODE_PARAM cam_mode_240_320;
extern OV2640_MODE_PARAM cam_mode_640_480;
extern OV2640_MODE_PARAM cam_mode_800_480;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_OV5640_H_ */
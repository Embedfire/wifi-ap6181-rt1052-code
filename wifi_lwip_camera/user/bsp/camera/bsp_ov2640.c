/**
  ******************************************************************
  * @file    bsp_ov2640.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   OV2640����
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */

#include "bsp_ov2640.h"
#include "bsp_ov2640_config.h"
#include "./lcd/bsp_lcd.h" 
#include "./key/bsp_key.h" 
#include "fsl_debug_console.h"
#include "string.h"

/*������ر���*/
uint32_t activeFrameAddr;
uint32_t inactiveFrameAddr;
static csi_private_data_t csiPrivateData;

OV2640_MODE_PARAM cam_temp_mode;//ȫ�ֽṹ��
/* OV2640���ӵ�CSI */
static csi_resource_t csiResource = {
    .csiBase = CSI,
};

camera_receiver_handle_t cameraReceiver = {
    .resource = &csiResource, .ops = &csi_ops, .privateData = &csiPrivateData,
};
/**
  * @brief  ����ͷ�жϴ�������
  * @param  ��
  * @retval ��
  */
//extern uint8_t fps;
void CSI_IRQHandler(void)
{
    CSI_DriverIRQHandler();
//		fps++; //֡�ʼ���
}


/**
  * @brief  ����ͷ��λ���ſ��ƺ���
  * @param  pullUp��1���ߵ�ƽ��0���͵�ƽ
  * @retval ��
  */
static void BOARD_PullCameraResetPin(bool pullUp)
{
    if (pullUp)
    {
        GPIO_PinWrite(CAMERA_RST_GPIO, CAMERA_RST_GPIO_PIN, 1);
    }
    else
    {
        GPIO_PinWrite(CAMERA_RST_GPIO, CAMERA_RST_GPIO_PIN, 0);
    }
}
/**
  * @brief  ����ͷ��Դ���ſ��ƺ���
  * @param  pullUp��1���ߵ�ƽ��0���͵�ƽ
  * @retval ��
  */
static void BOARD_PullCameraPowerDownPin(bool pullUp)
{
    if (pullUp)
    {
        GPIO_PinWrite(CAMERA_PWR_GPIO, CAMERA_PWR_GPIO_PIN, 1);
    }
    else
    {
        GPIO_PinWrite(CAMERA_PWR_GPIO, CAMERA_PWR_GPIO_PIN, 0);
    }
}
/* ����ͷ��Դ�ṹ��*/
static ov2640_resource_t ov2640Resource = {
    .sccbI2C = OV2640_I2C,
    .pullResetPin = BOARD_PullCameraResetPin,
    .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
    .inputClockFreq_Hz = 24000000,
};
/*����ͷ�豸���*/
camera_device_handle_t cameraDevice = {
    .resource = &ov2640Resource, .ops = &ov2640_ops,
};
/**
* @brief  ��ʼ����������ͷ��Դ	
  * @param  ��
  * @retval ��
  */
void BOARD_InitCameraResource(void)
{
    lpi2c_master_config_t masterConfig;
    uint32_t sourceClock;

    LPI2C_MasterGetDefaultConfig(&masterConfig);
		/*����I2Cʱ��Ϊ400KHz*/
    masterConfig.baudRate_Hz = 400000;
    masterConfig.debugEnable = true;
    masterConfig.ignoreAck = true;

    /*LPI2Cʱ������ */
    /*
     * LPI2C ʱ��Դѡ��:
     *  0: pll3_60m
     *  1: OSC clock
     */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 1);
    /*
     * LPI2C ��Ƶϵ��.
     *  0b000000: Divide by 1
     *  0b111111: Divide by 2^6
     */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 0);

    /* LPI2C ʱ��ԴΪ OSC */
    sourceClock = CLOCK_GetOscFreq();

    LPI2C_MasterInit(OV2640_I2C, &masterConfig, sourceClock);
		/* ��ʼ������ͷ��PDN��RST���� */
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalOutput, 1,
    };
    GPIO_PinInit(CAMERA_PWR_GPIO, CAMERA_PWR_GPIO_PIN, &pinConfig);
		GPIO_PinInit(CAMERA_RST_GPIO, CAMERA_RST_GPIO_PIN, &pinConfig);
}


/**
  * @brief  ����OV2640
  * @param  None
  * @retval None
  */
void Camera_Init(void) 
{
		/* ��ʼ������ͷ���� */
		BOARD_InitCSIPins();
		/* ��ʼ������ͷ��I2C���������� */
		BOARD_InitCameraResource();
    /* ��������ͷģʽ */
		Set_Cam_mode(1);

    /* ��������ͷ���� */
    const camera_config_t cameraConfig = {
        .pixelFormat = kVIDEO_PixelFormatRGB565,/*������ظ�ʽ*/
        .bytesPerPixel = APP_BPP,
				.resolution = FSL_VIDEO_RESOLUTION(cam_temp_mode.cam_out_width, cam_temp_mode.cam_out_height),//��Ƶ�ֱ���
        .frameBufferLinePitch_Bytes = APP_IMG_WIDTH * APP_BPP,
        .interface = kCAMERA_InterfaceGatedClock,
        .controlFlags = APP_CAMERA_CONTROL_FLAGS,
        .framePerSec = 30,
    };
    memset(s_frameBuffer, 0, sizeof(s_frameBuffer));

    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, NULL, NULL);

    CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);

    CAMERA_DEVICE_Start(&cameraDevice);

    /* ����֡�������ύ������������. */
    for (uint32_t i = 0; i < APP_FRAME_BUFFER_COUNT; i++)
    {
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(s_frameBuffer[i]));
    }

    CAMERA_RECEIVER_Start(&cameraReceiver);

//    /*
//     * LCDIF �л�������ͷǻ������, ������ڴ˴���ȡ����������.
//     */
//    /* �ȴ���ȡ����֡����������ʾ */
//    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &activeFrameAddr))
//    {
//    }

//    /* �ȴ���ȡ����֡����������ʾ */
//    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &inactiveFrameAddr))
//    {
//    }


}


#include "bsp_camera_show_rtos.h"
#define CSI_W               188          
#define CSI_H               120 

extern uint8_t image_csi1[CSI_H][CSI_W];
extern uint8_t image_csi2[CSI_H][CSI_W];
extern uint8_t (*user_image)[CSI_W];

uint32_t fullCameraBufferAddr;    //�ɼ���ɵĻ�������ַ    �û��������


//uint8_t csi_get_full_buffer(csi_handle_t *handle, uint32_t *buffaddr)
uint8_t csi_get_full_buffer()
{
   // if(kStatus_Success == CSI_TransferGetFullBuffer(CSI,handle,(uint32_t *)buffaddr))
	 if(kStatus_Success == CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver,(uint32_t *)fullCameraBufferAddr))
    {
        return 1;//��ȡ���ɼ���ɵ�BUFFER
    }
    return 0;    //δ�ɼ����
}

void csi_add_empty_buffer(csi_handle_t *handle, uint8_t *buff)
{
    CSI_TransferSubmitEmptyBuffer(CSI,handle,(uint32_t)buff);
}

void csi_isr(CSI_Type *base, csi_handle_t *handle, status_t status, void *userData)
{

    if(csi_get_full_buffer())
    {
			
   			CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint8_t )fullCameraBufferAddr);
			
        //csi_add_empty_buffer(&csi_handle,(uint8_t *)fullCameraBufferAddr);
        if(fullCameraBufferAddr == (uint32_t)image_csi1[0])
        {
            user_image = image_csi1;//image_csi1�ɼ����
        }
        else if(fullCameraBufferAddr == (uint8_t)image_csi2[0])
        {
            user_image = image_csi2;//image_csi2�ɼ����
        }
        //mt9v03x_csi_finish_flag = 1;//�ɼ���ɱ�־λ��һ
    }
}

/**
  * @brief  ����ͷ����ѡ��
  * @param  None
  * @retval None
  */
int index_num=1;
void Cam_Config_Switch()
{
//		/* ���WAUP���� */
//		if(Key_Scan(CORE_BOARD_WAUP_KEY_GPIO, CORE_BOARD_WAUP_KEY_GPIO_PIN) == KEY_ON )
//		{
//			index_num++;
//			if(index_num>3)
//			{
//				index_num=1;
//			}
//      /*	��������ͷģʽ */
//			Set_Cam_mode(index_num);
//      /* ��������ͷ���� */
//			const camera_config_t cameraConfig = {
//					.pixelFormat = kVIDEO_PixelFormatRGB565,
//					.bytesPerPixel = APP_BPP,
//					.resolution = FSL_VIDEO_RESOLUTION(cam_temp_mode.cam_out_width, cam_temp_mode.cam_out_height),//��Ƶ�ֱ��ʵ� ���Ⱥ͸߶�
//					.frameBufferLinePitch_Bytes = APP_IMG_WIDTH * APP_BPP,
//					.interface = kCAMERA_InterfaceGatedClock,
//					.controlFlags = APP_CAMERA_CONTROL_FLAGS,
//					.framePerSec = 30,
//			};
//			/*	�ر�����ͷ���� */
//			CAMERA_DEVICE_Stop(&cameraDevice);			
//			memset(s_frameBuffer, 0, sizeof(s_frameBuffer));
//			/* ��ʼ������ͷ���� */
//			CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, NULL, NULL);
//      /* ��ʼ������ͷ�豸 */
//			CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
//      /* ����ͷ�豸��ʼ */
//			CAMERA_DEVICE_Start(&cameraDevice);
//			/* ����֡�������ύ������������. */
//			for (uint32_t i = 0; i < APP_FRAME_BUFFER_COUNT; i++)
//			{
//					CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(s_frameBuffer[i]));
//			}
//			CAMERA_RECEIVER_Start(&cameraReceiver);	
//			
//			memset(s_frameBuffer, 0, sizeof(s_frameBuffer));//������Ӱ
//		}    
}
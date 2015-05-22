/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : nibp_api.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/6
  Last Modified :
  Description   : api for nibp
  Function List :
  History       :
  1.Date        : 2015/3/6
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include "includes.h"
#include "driver_nibp.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern InternalADCResult *pgInternalADCResult;

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define NIBP_MAX_PRESS                  310//mmHg
#define NIBP_PRESS_DET_STABLE_TIME_MS   2000 //2s

#define Ctrl_Idle                   (u8)(0x00)
#define Ctrl_Busy                   (u8)(0x01)

#define SW_OFF                      (u8)(0x00)
#define SW_ON                       (u8)(0x01)

#define PRESS_RELE_DISABLE          (u8)(0x00)
#define PRESS_RELE_ENABLE           (u8)(0x01)

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
typedef enum
{ 
    ADULT     = 1,     
    CHILD     = 2,
    NEWBORN   = 3,
} PATIENT_TypeDef;

typedef struct
{
    u16 u16Eeprom0mmHg_mV;
    u16 u16Eeprom162mmHg_mV;
    u16 u16Eeprom315mmHg_mV;
    u8 u8DspPatientType;
    u8 u8VerifyedFlag;
} Nibp2Dsp_TypeDef;

static Nibp2Dsp_TypeDef gNibp2Dsp;
static Nibp2Dsp_TypeDef *pgNibp2Dsp = &gNibp2Dsp;

static u8 u8FastReleState;
static u16 u16PressLimit_mV;
static u8 u8ProtectStatus;
static u8 u8VerifyingState;
/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

void nibp_api_setPressRele(u8 state)
{
    if (PRESS_RELE_DISABLE == state)
    {
        u8FastReleState = SW_OFF;
        nibp_dev_setPressRelePin(1);
    }
    else
    {
        u8FastReleState = SW_ON;
        nibp_dev_setPressRelePin(0);
    }
}

u8 nibp_api_getPressReleState(void)
{
    return u8FastReleState;
}

static void nibp_api_resetSafePress(void)
{
    u16PressLimit_mV = 7.31 * NIBP_MAX_PRESS + 265;
    pgNibp2Dsp->u8VerifyedFlag = 0;
    pgNibp2Dsp->u8DspPatientType = ADULT;
}

void nibp_api_init(void)
{
    u8ProtectStatus = Ctrl_Idle;
    u8VerifyingState = SW_OFF;
    
    nibp_dev_gpio_init();
    nibp_api_setPressRele(PRESS_RELE_DISABLE);
    nibp_api_resetSafePress();
}

void nibp_api_checkPressRele(void)
{
    static u16 u16PressOld_mV;
    if (Ctrl_Idle == u8ProtectStatus)
    {
        if (pgInternalADCResult->BBP > u16PressLimit_mV)
        {
            u8ProtectStatus = Ctrl_Busy;
            u16PressOld_mV = pgInternalADCResult->BBP;
            nibp_api_setPressRele(PRESS_RELE_ENABLE);
            printf("\r\n[NIBP]OverPress=%d, max=%d",
                pgInternalADCResult->BBP,u16PressLimit_mV);
        }
    }
    else if (Ctrl_Busy == u8ProtectStatus)
    {
        vTaskDelay(NIBP_PRESS_DET_STABLE_TIME_MS / portTICK_PERIOD_MS);
        if ((u16PressOld_mV - pgInternalADCResult->BBP) < 3)
        {
            u8ProtectStatus = Ctrl_Idle;
            nibp_api_setPressRele(PRESS_RELE_DISABLE);
            printf("\r\n[NIBP]OverPress Action Finished!");
        }
        u16PressOld_mV = pgInternalADCResult->BBP; 
    }
}

void nibp_api_printf(void)
{
    printf("\r\n[NIBP]Cur=%d, max=%d", pgInternalADCResult->BBP,u16PressLimit_mV);
}

u8 nibp_api_isVerifying(void)
{
    return u8VerifyingState;
}

int nibp_api_setSafePress(u8 *pBuf, u8 nLen)
{
    //D[0]=flag,D[1~2]=0mmHg-adc,D[3~4]=162mmHg-adc,D[5~6]=315mmHg-adc,D[7]=patientype
    if (8 != nLen) return -1;
    
    pgNibp2Dsp->u8VerifyedFlag = pBuf[0];
    pgNibp2Dsp->u16Eeprom0mmHg_mV = (pBuf[1]<<8) | pBuf[2];
    pgNibp2Dsp->u16Eeprom162mmHg_mV = (pBuf[3]<<8) | pBuf[4];
    pgNibp2Dsp->u16Eeprom315mmHg_mV = (pBuf[5]<<8) | pBuf[6];
    pgNibp2Dsp->u8DspPatientType = pBuf[7];

    if (NEWBORN == pgNibp2Dsp->u8DspPatientType)
    {
        u16PressLimit_mV = pgNibp2Dsp->u16Eeprom162mmHg_mV;
    }
    else
    {
        u16PressLimit_mV = pgNibp2Dsp->u16Eeprom315mmHg_mV;
    }
	return 0;
}

int nibp_api_getSafePress(u8 *pBuf, u8 nLen)
{
    //D[0]=flag,D[1~2]=0mmHg-adc,D[3~4]=162mmHg-adc,D[5~6]=315mmHg-adc,D[7]=patientype
    pBuf[0] = pgNibp2Dsp->u8VerifyedFlag;
    pBuf[1] = (pgNibp2Dsp->u16Eeprom0mmHg_mV>> 8) & 0xFF;//MSB
    pBuf[2] = pgNibp2Dsp->u16Eeprom0mmHg_mV & 0xFF;       //LSB 
    pBuf[3] = (pgNibp2Dsp->u16Eeprom162mmHg_mV>> 8) & 0xFF;//MSB
    pBuf[4] = pgNibp2Dsp->u16Eeprom162mmHg_mV & 0xFF;       //LSB
    pBuf[5] = (pgNibp2Dsp->u16Eeprom315mmHg_mV>> 8) & 0xFF;//MSB
    pBuf[6] = pgNibp2Dsp->u16Eeprom315mmHg_mV & 0xFF;       //LSB
    pBuf[7] = pgNibp2Dsp->u8DspPatientType;//LSB   
    
	return 0;
}

int nibp_api_setVerify(u8 state)
{
    if (!state)
    {
        u8VerifyingState = SW_OFF;
    }
    else
    {
        u8VerifyingState = SW_ON;
    }
	return 0;
}


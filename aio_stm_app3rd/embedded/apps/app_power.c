/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_power.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/15
  Last Modified :
  Description   : power detect
  Function List :
  History       :
  1.Date        : 2015/4/15
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
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
#define PWR_RESET_VAL               (u8)(0x00)
#define PWR_BIT_D3V3_MASK           (u8)(0x01<<0)
#define PWR_BIT_VDDINT_MASK         (u8)(0x01<<1)
#define PWR_BIT_D5V_N_MASK          (u8)(0x01<<2)
#define PWR_BIT_D5V_P_MASK          (u8)(0x01<<3)

#define PWR_DET_RANGE               10
#define PWR_D3V3_Typ                (u16)(1650) //mV
#define PWR_VDDINT_Typ              (u16)(1200) //mV
#define PWR_D5V_N_Typ               (u16)(1940) //mV
#define PWR_D5V_P_Typ               (u16)(1670) //mV

#define PWR_D3V3_Max        (u16)(PWR_D3V3_Typ * (100 + PWR_DET_RANGE) / 100)
#define PWR_D3V3_Min        (u16)(PWR_D3V3_Typ * (100 - PWR_DET_RANGE) / 100)

#define PWR_VDDINT_Max      (u16)(PWR_VDDINT_Typ * (100 + PWR_DET_RANGE) / 100)
#define PWR_VDDINT_Min      (u16)(PWR_VDDINT_Typ * (100 - PWR_DET_RANGE) / 100)

#define PWR_D5V_N_Max       (u16)(PWR_D5V_N_Typ * (100 + PWR_DET_RANGE) / 100)
#define PWR_D5V_N_Min       (u16)(PWR_D5V_N_Typ * (100 - PWR_DET_RANGE) / 100)

#define PWR_D5V_P_Max       (u16)(PWR_D5V_P_Typ * (100 + PWR_DET_RANGE) / 100)
#define PWR_D5V_P_Min       (u16)(PWR_D5V_P_Typ * (100 - PWR_DET_RANGE) / 100)

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static u8 u8PwrStatus = 0;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static void power_refreshStatus(void)
{
    u8 status = PWR_RESET_VAL;
    if((pgInternalADCResult->D3V3 < PWR_D3V3_Min) \
        || (pgInternalADCResult->D3V3 > PWR_D3V3_Max))
    {
        status |= PWR_BIT_D3V3_MASK;
    }
    
    if((pgInternalADCResult->VDDINT < PWR_VDDINT_Min) \
        || (pgInternalADCResult->VDDINT > PWR_VDDINT_Max))
    {
        status |= PWR_BIT_VDDINT_MASK;
    }
    
    if((pgInternalADCResult->D5V_N < PWR_D5V_N_Min) \
        || (pgInternalADCResult->D5V_N > PWR_D5V_N_Max))
    {
        status |= PWR_BIT_D5V_N_MASK;
    }
    
    if((pgInternalADCResult->D5V_P < PWR_D5V_P_Min) \
        || (pgInternalADCResult->D5V_P > PWR_D5V_P_Max))
    {
        status |= PWR_BIT_D5V_P_MASK;
    }
    
    u8PwrStatus = status;
}

u8 power_getStatus(void)
{
    return u8PwrStatus;
}

static void power_sample_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    for (;;)
    {
        power_refreshStatus();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }    
}

int app_power_init(void)
{
    u8PwrStatus = PWR_RESET_VAL;
    return 0;
}

int app_power_start(void)
{
    xTaskCreate(power_sample_task,\
                "power_sample_task",\
                20,\
                NULL,\
                POWER_TASK_PRIORITY,\
                NULL);
    return 0;
}

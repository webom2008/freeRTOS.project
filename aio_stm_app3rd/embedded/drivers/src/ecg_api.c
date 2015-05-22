/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : ecg_api.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/6
  Last Modified :
  Description   : api for ecg
  Function List :
  History       :
  1.Date        : 2015/3/6
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "driver_ecg.h"
#include "ecg_api.h"

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
const u8 ECG_PROBE_RESET_VAL     = (u8)(0x00);
const u8 ECG_PROBE_LA_MASK       = (u8)(0x01 << 0);
const u8 ECG_PROBE_RA_MASK       = (u8)(0x01 << 1);
const u8 ECG_PROBE_LL_MASK       = (u8)(0x01 << 2);
const u8 ECG_PROBE_RL_MASK       = (u8)(0x01 << 3);
const u8 ECG_PROBE_V1_MASK       = (u8)(0x01 << 4);
const u8 ECG_PROBE_ECG1_MASK     = (u8)(0x01 << 5);
const u8 ECG_PROBE_ECG2_MASK     = (u8)(0x01 << 6);
const u8 ECG_PROBE_ECGV_MASK     = (u8)(0x01 << 7);

const u8 STM_ECG_MODE_5PROBE        = 0x00;
const u8 STM_ECG_MODE_3PROBE_I      = 0x01;
const u8 STM_ECG_MODE_3PROBE_II     = 0x02;
const u8 STM_ECG_MODE_3PROBE_III    = 0x03;

const u16 ECG_OFF_703MV_MAX             = 843;  //+20%
const u16 ECG_OFF_703MV_MIN             = 562;  //-20%

const u16 ECG_5P_RL_OFF_1742MV_MAX      = 1829; //+5%
const u16 ECG_5P_RL_OFF_1742MV_MIN      = 1724; //-1%
const u16 ECG_5P_RL_ON_1631MV_MAX       = 1712; //+5%
const u16 ECG_5P_RL_ON_1631MV_MIN       = 1549; //-5%
const u16 ECG_5P_RL_ON_1174MV_MAX       = 1408; //+20%
const u16 ECG_5P_RL_ON_1174MV_MIN       = 1138; //-3%
const u16 ECG_5P_RL_ON_1042MV_MAX       = 1094; //+5%
const u16 ECG_5P_RL_ON_1042MV_MIN       = 990;  //-5%

const u16 ECG_3P_DRIVER_OFF_1792MV_MAX   = 2060; //+15%
const u16 ECG_3P_DRIVER_OFF_1792MV_MIN   = 1523; //-15%
const u16 ECG_3P_DRIVER_ON_1346MV_MAX    = 1480; //+10%
const u16 ECG_3P_DRIVER_ON_1346MV_MIN    = 1211; //-10%
const u16 ECG_3P_DRIVER_ON_1042MV_MAX    = 1146; //+10%
const u16 ECG_3P_DRIVER_ON_1042MV_MIN    = 937;  //-10%

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/

typedef struct
{
    u16 RA_OFF_Val;
    u16 RL_OFF_Val;
    u16 LA_OFF_Val;
    u16 LL_OFF_Val;
    u16 V1_OFF_Val;
//    u16 ECG1_OVERLOAD_Val;
//    u16 ECG2_OVERLOAD_Val;
//    u16 ECGV_OVERLOAD_Val;
} ECG_ResultTypeDef;

typedef struct
{
    u8 u8PorbeStatus;
    u8 u8PorbeStatusPrev;
    u8 u8PorbeMode;
} ECG_DevTypeDef;

static ECG_SelectTypeDef g_ecg_target;
static ECG_ResultTypeDef g_ecg_result;
static ECG_ResultTypeDef *pECGResult = &g_ecg_result;
static ECG_DevTypeDef gEcgDev;
static ECG_DevTypeDef *pgEcgDev = &gEcgDev;
/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static u8 ecg_api_setChannel(ECG_SelectTypeDef channel)
{
    
    if (ecg_dev_setChannel(channel))
    {
        g_ecg_target = channel;
        return 1;
    }
    else
    {
        return 0;
    }
}

void ecg_api_init(void)
{
    pgEcgDev->u8PorbeStatus = ECG_PROBE_RESET_VAL;
    pgEcgDev->u8PorbeMode = STM_ECG_MODE_5PROBE;
    ecg_dev_gpio_init();
    ecg_api_setChannel(LA_OFF);
}


u8 ecg_api_setNextChannel(void)
{
    u8 result = 0;
    ECG_SelectTypeDef curType = g_ecg_target;
    if (curType == V1_OFF)
    {
        result = ecg_api_setChannel(LA_OFF);
    }
    else
    {
        curType++;
        result = ecg_api_setChannel(curType);
    }
    return result;
}

u8 ecg_api_saveDetectData(void)
{
    u16 curValue = pgInternalADCResult->PROBE;
    
    switch(g_ecg_target)
    {
        case RA_OFF:
        {
            pECGResult->RA_OFF_Val = curValue;
            break;
        }
        case RL_OFF:
        {
            pECGResult->RL_OFF_Val = curValue;
            break;
        }
        case LA_OFF:
        {
            pECGResult->LA_OFF_Val = curValue;
            break;
        }
        case LL_OFF:
        {
            pECGResult->LL_OFF_Val = curValue;
            break;
        }
        case V1_OFF:
        {
            pECGResult->V1_OFF_Val= curValue;
            break;
        }
//        case ECG1_OVERLOAD:
//        {
//            pECGResult->ECG1_OVERLOAD_Val= curValue;
//            break;
//        }
//        case ECG2_OVERLOAD:
//        {
//            pECGResult->ECG2_OVERLOAD_Val= curValue;
//            break;
//        }
//        case ECGV_OVERLOAD:
//        {
//            pECGResult->ECGV_OVERLOAD_Val= curValue;
//            break;
//        }
        default:
            printf("\r\nECG_DetectDataSave error = %d",g_ecg_target);
            return 0;
    }
    return 1;
}
static u8 ecg_api_isProbeOFF(u16 mV)
{
    if ((mV > ECG_OFF_703MV_MIN) && (mV < ECG_OFF_703MV_MAX))
    {
        return (u8)0x01;
    }
    else
    {
        return (u8)0x00;
    }
}

static u8 ecg_api_get5ProbeModeStatus(void)
{
    u8 u8status = ECG_PROBE_RESET_VAL;
    if (pECGResult->RL_OFF_Val > ECG_5P_RL_OFF_1742MV_MIN)
    {
        if (pECGResult->V1_OFF_Val > ECG_5P_RL_OFF_1742MV_MIN)
        {
            u8status = ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK | ECG_PROBE_LL_MASK;
        }
        else
        {
            u8status = ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK | ECG_PROBE_LL_MASK \
                        | ECG_PROBE_RL_MASK | ECG_PROBE_V1_MASK;
        }
    }
    else if ((pECGResult->RL_OFF_Val > ECG_5P_RL_ON_1631MV_MIN) \
            && (pECGResult->RL_OFF_Val < ECG_5P_RL_ON_1631MV_MAX))
    {
        if (ecg_api_isProbeOFF(pECGResult->LA_OFF_Val))
        {
            u8status |= ECG_PROBE_LA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->RA_OFF_Val))
        {
            u8status |= ECG_PROBE_RA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->LL_OFF_Val))
        {
            u8status |= ECG_PROBE_LL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else if ((pECGResult->RL_OFF_Val > ECG_5P_RL_ON_1174MV_MIN) \
            && (pECGResult->RL_OFF_Val < ECG_5P_RL_ON_1174MV_MAX))
    {
        if (ecg_api_isProbeOFF(pECGResult->LA_OFF_Val))
        {
            u8status |= ECG_PROBE_LA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->RA_OFF_Val))
        {
            u8status |= ECG_PROBE_RA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->LL_OFF_Val))
        {
            u8status |= ECG_PROBE_LL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else if (pECGResult->RL_OFF_Val < ECG_5P_RL_ON_1042MV_MAX)
    {
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else
    {
        u8status = pgEcgDev->u8PorbeStatusPrev;
        printf("\r\necg_api_get5ProbeModeStatus not match %d!",pECGResult->RL_OFF_Val);
    }
    pgEcgDev->u8PorbeStatusPrev = u8status;
    return u8status;
}

static u8 ecg_api_get3ProbeModeIStatus(void)
{
    u8 u8status = ECG_PROBE_RESET_VAL;
    if (pECGResult->LL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN)
    {
        if ((pECGResult->RL_OFF_Val < ECG_3P_DRIVER_OFF_1792MV_MIN) \
            && (pECGResult->V1_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_RL_MASK | ECG_PROBE_RA_MASK);
        }
        else if ((pECGResult->V1_OFF_Val < ECG_3P_DRIVER_OFF_1792MV_MIN) \
                && (pECGResult->RL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_V1_MASK | ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK);
        }
        else if ((pECGResult->V1_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN) \
                && (pECGResult->RL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK);
        }
        else
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK | ECG_PROBE_LL_MASK \
                        | ECG_PROBE_RL_MASK | ECG_PROBE_V1_MASK);
        }
    }
    else if ((pECGResult->LL_OFF_Val > ECG_3P_DRIVER_ON_1346MV_MIN) \
            && (pECGResult->LL_OFF_Val < ECG_3P_DRIVER_ON_1346MV_MAX))
    {
        if (ecg_api_isProbeOFF(pECGResult->RA_OFF_Val))
        {
            u8status |= ECG_PROBE_RA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->RL_OFF_Val))
        {
            u8status |= ECG_PROBE_RL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->LA_OFF_Val))
        {
            u8status |= ECG_PROBE_LA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else if (pECGResult->LL_OFF_Val < ECG_3P_DRIVER_ON_1042MV_MAX)
    {
        if (ecg_api_isProbeOFF(pECGResult->RL_OFF_Val))
        {
            u8status |= ECG_PROBE_RL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else
    {
        u8status = pgEcgDev->u8PorbeStatusPrev;
        printf("\r\necg_api_get3ProbeModeStatus I not match %d!",pECGResult->RL_OFF_Val);
    }
    pgEcgDev->u8PorbeStatusPrev = u8status;
    return u8status;
}

static u8 ecg_api_get3ProbeModeIIStatus(void)
{
    u8 u8status = ECG_PROBE_RESET_VAL;
    if (pECGResult->LA_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN)
    {
        if ((pECGResult->RL_OFF_Val < ECG_3P_DRIVER_OFF_1792MV_MIN) \
            && (pECGResult->V1_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_LL_MASK | ECG_PROBE_RL_MASK | ECG_PROBE_RA_MASK);
        }
        else if ((pECGResult->V1_OFF_Val < ECG_3P_DRIVER_OFF_1792MV_MIN) \
                && (pECGResult->RL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_V1_MASK | ECG_PROBE_LL_MASK | ECG_PROBE_RA_MASK);
        }
        else if ((pECGResult->V1_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN) \
                && (pECGResult->RL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_LL_MASK | ECG_PROBE_RA_MASK);
        }
        else
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK | ECG_PROBE_LL_MASK \
                        | ECG_PROBE_RL_MASK | ECG_PROBE_V1_MASK);
        }
    }
    else if ((pECGResult->LA_OFF_Val > ECG_3P_DRIVER_ON_1346MV_MIN) \
            && (pECGResult->LA_OFF_Val < ECG_3P_DRIVER_ON_1346MV_MAX))
    {
        if (ecg_api_isProbeOFF(pECGResult->RA_OFF_Val))
        {
            u8status |= ECG_PROBE_RA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->RL_OFF_Val))
        {
            u8status |= ECG_PROBE_RL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->LL_OFF_Val))
        {
            u8status |= ECG_PROBE_LL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else if (pECGResult->LA_OFF_Val < ECG_3P_DRIVER_ON_1042MV_MAX)
    {
        if (ecg_api_isProbeOFF(pECGResult->RL_OFF_Val))
        {
            u8status |= ECG_PROBE_RL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else
    {
        u8status = pgEcgDev->u8PorbeStatusPrev;
        printf("\r\necg_api_get3ProbeModeStatus II not match %d!",pECGResult->RL_OFF_Val);
    }
    pgEcgDev->u8PorbeStatusPrev = u8status;
    return u8status;
}

static u8 ecg_api_get3ProbeModeIIIStatus(void)
{
    u8 u8status = ECG_PROBE_RESET_VAL;
    if (pECGResult->RA_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN)
    {
        if ((pECGResult->RL_OFF_Val < ECG_3P_DRIVER_OFF_1792MV_MIN) \
            && (pECGResult->V1_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_RL_MASK | ECG_PROBE_LL_MASK);
        }
        else if ((pECGResult->V1_OFF_Val < ECG_3P_DRIVER_OFF_1792MV_MIN) \
                && (pECGResult->RL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_V1_MASK | ECG_PROBE_LA_MASK | ECG_PROBE_LL_MASK);
        }
        else if ((pECGResult->V1_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN) \
                && (pECGResult->RL_OFF_Val > ECG_3P_DRIVER_OFF_1792MV_MIN))
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_LL_MASK);
        }
        else
        {
            u8status = (ECG_PROBE_LA_MASK | ECG_PROBE_RA_MASK | ECG_PROBE_LL_MASK \
                        | ECG_PROBE_RL_MASK | ECG_PROBE_V1_MASK);
        }
    }
    else if ((pECGResult->RA_OFF_Val > ECG_3P_DRIVER_ON_1346MV_MIN) \
            && (pECGResult->RA_OFF_Val < ECG_3P_DRIVER_ON_1346MV_MAX))
    {
        if (ecg_api_isProbeOFF(pECGResult->LL_OFF_Val))
        {
            u8status |= ECG_PROBE_LL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->RL_OFF_Val))
        {
            u8status |= ECG_PROBE_RL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->LA_OFF_Val))
        {
            u8status |= ECG_PROBE_LA_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else if (pECGResult->RA_OFF_Val < ECG_3P_DRIVER_ON_1042MV_MAX)
    {
        if (ecg_api_isProbeOFF(pECGResult->RL_OFF_Val))
        {
            u8status |= ECG_PROBE_RL_MASK;
        }
        if (ecg_api_isProbeOFF(pECGResult->V1_OFF_Val))
        {
            u8status |= ECG_PROBE_V1_MASK;
        }
    }
    else
    {
        u8status = pgEcgDev->u8PorbeStatusPrev;
        printf("\r\necg_api_get3ProbeModeStatus III not match %d!",pECGResult->RL_OFF_Val);
    }
    pgEcgDev->u8PorbeStatusPrev = u8status;

    return u8status;
}

void ecg_api_updateProbeStatus(void)
{
    static char message[20];
    switch (pgEcgDev->u8PorbeMode)
    {
    case STM_ECG_MODE_5PROBE:
        {
            pgEcgDev->u8PorbeStatus = ecg_api_get5ProbeModeStatus();
        }
        break;
    case STM_ECG_MODE_3PROBE_I:
        {
            pgEcgDev->u8PorbeStatus = ecg_api_get3ProbeModeIStatus();
        }
        break;
    case STM_ECG_MODE_3PROBE_II:
        {
            pgEcgDev->u8PorbeStatus = ecg_api_get3ProbeModeIIStatus();
        }
        break;
    case STM_ECG_MODE_3PROBE_III:
        {
            pgEcgDev->u8PorbeStatus = ecg_api_get3ProbeModeIIIStatus();
        }
        break;
    default:
        break;
    }
    sprintf(message, "\r\n>>%d",pgEcgDev->u8PorbeStatus);
    uart_dev_sendBuf(message, strlen(message));
}

void ecg_api_printf(void)
{
    printf("\r\nRA=%dmV,",pECGResult->RA_OFF_Val);
    vTaskDelay( (TickType_t)10);
    printf("RL=%dmV,",pECGResult->RL_OFF_Val);
    vTaskDelay( (TickType_t)10);
    printf("LA=%dmV,",pECGResult->LA_OFF_Val);
    vTaskDelay( (TickType_t)10);
    printf("LL=%dmV,",pECGResult->LL_OFF_Val);
    vTaskDelay( (TickType_t)10);
    printf("V1=%dmV,",pECGResult->V1_OFF_Val);
    vTaskDelay( (TickType_t)10);
    printf("Status=0X%02X,",pgEcgDev->u8PorbeStatus);
    vTaskDelay( (TickType_t)10);
    printf("Mode=%d",pgEcgDev->u8PorbeMode);
    vTaskDelay( (TickType_t)10);
}

int ecg_api_getProbe(u8 *pVal)
{
    *pVal = pgEcgDev->u8PorbeStatus;
    return 0;
}

u8 ecg_api_getProbeMode(void)
{
    return pgEcgDev->u8PorbeMode;
}
void ecg_api_setProbeMode(u8 mode)
{
    pgEcgDev->u8PorbeMode = mode;
}

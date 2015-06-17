/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_cli.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/9
  Last Modified :
  Description   : cli server
  Function List :
  History       :
  1.Date        : 2015/4/9
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

#include "app_cli.h"
#include "UartCommandSever.h"
#include "CLI_Commands.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int app_cli_init(void)
{
    UartCommandSever_init();
    return 0;
}

int app_cli_start(void)
{
    UartCommandSever_start();
    
    /* Register commands with the FreeRTOS+CLI command interpreter. */
	vRegisterCLICommands();
    return 0;
}

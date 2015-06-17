/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : CLI_Commands.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/9
  Last Modified :
  Description   : cli commands
  Function List :
  History       :
  1.Date        : 2015/4/9
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "FreeRTOS_CLI.h"
#include "CLI_Commands.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern char APP_VERSION[];

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

static BaseType_t prvVersionCommand( char *pcWriteBuffer,
                                        size_t xWriteBufferLen,
                                        const char *pcCommandString )
{
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;

    sprintf( pcWriteBuffer, "Version:%s\r\n", APP_VERSION );
    
	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

static BaseType_t prvLEDControlCommand( char *pcWriteBuffer,
                                        size_t xWriteBufferLen,
                                        const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t lParameterStringLength, xReturn;
    static BaseType_t lParameterNumber = 0;
    int param;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( lParameterNumber == 0 )
	{
		sprintf( pcWriteBuffer, "Action were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							lParameterNumber,		/* Return the next parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );
        
		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( lParameterNumber == 2L )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			xReturn = pdFALSE;
			lParameterNumber = 0L;
            param = atoi(pcParameter);
            app_led_ctl(LED_CTRL_PERIOD, param);
		    sprintf( pcWriteBuffer, "LED_CTRL_PERIOD\r\n" );
		}
		else
		{
			/* There are more parameters to return after this one. */
            param = atoi(pcParameter);
            if (0 == param)
            {
                app_led_ctl(LED_CTRL_OFF, param);
		        sprintf( pcWriteBuffer, "LED_CTRL_OFF\r\n" );
    			xReturn = pdFALSE;
    			lParameterNumber = 0L;
            }
            else if (1 == param)
            {
                app_led_ctl(LED_CTRL_ON, param);
		        sprintf( pcWriteBuffer, "LED_CTRL_ON\r\n" );
    			xReturn = pdFALSE;
    			lParameterNumber = 0L;
            }
            else
            {
    			xReturn = pdTRUE;
    			lParameterNumber++;
            }
		}
	}

	return xReturn;
}

static BaseType_t prvSoftWareResetCommand(  char *pcWriteBuffer,
                                            size_t xWriteBufferLen,
                                            const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Obtain the parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						1,						/* Return the first parameter. */
						&lParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );

	/* There are only two valid parameter values. */
	if( strncmp( pcParameter, "update", strlen( "update" ) ) == 0 )
	{
        BKP_WriteBackupRegister(BKP_DR3, 0xAA);
        NVIC_SystemReset(); //reboot to bootloader
		sprintf( pcWriteBuffer, "reset and wait to update app.\r\n" );
	}
	else
	{
        NVIC_SystemReset(); //reboot to bootloader
		sprintf( pcWriteBuffer, "only reset.\r\n" );
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}


static BaseType_t prvParameterEchoCommand(  char *pcWriteBuffer, 
                                            size_t xWriteBufferLen,
                                            const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t lParameterStringLength, xReturn;
    static BaseType_t lParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( lParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( pcWriteBuffer, "The parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							lParameterNumber,		/* Return the next parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);

		if( pcParameter != NULL )
		{
			/* Return the parameter string. */
			memset( pcWriteBuffer, 0x00, xWriteBufferLen );
			sprintf( pcWriteBuffer, "%d: ", lParameterNumber );
			strncat( pcWriteBuffer, pcParameter, lParameterStringLength );
			strncat( pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

			/* There might be more parameters to return after this one. */
			xReturn = pdTRUE;
			lParameterNumber++;
		}
		else
		{
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[ 0 ] = 0x00;

			/* No more data to return. */
			xReturn = pdFALSE;

			/* Start over the next time this command is executed. */
			lParameterNumber = 0;
		}
	}

	return xReturn;
}


static const CLI_Command_Definition_t CLICommands[] =
{
{
	"ver", /* The command string to type. */
	"\r\nver           :Display version infomation, no params need"\
	"\r\n",
	prvVersionCommand, /* The function to run. */
	0 /* No parameters are expected. */
},

{
	"sf_reset",
	"\r\nsf_reset      :SoftWare Reset, 1 params need"\
	"\r\n               param1:update - update app action; reset - only reset"\
	"\r\n",
	prvSoftWareResetCommand, /* The function to run. */
	1 /* one parameters are expected, which can take any value. */
},

{
	"led_ctl", /* The command string to type. */
	"\r\nled_ctl       :Control LED, 2 params need"\
	"\r\n               param1:0(off), 1(on), 2(period) param2:period(ms)"\
	"\r\n",
	prvLEDControlCommand, /* The function to run. */
	2 /* two parameters are expected, which can take any value. */
},

{
	"echo_params",
	"\r\necho_params   :Take variable number of parameters, echos each in turn"\
	"\r\n               param<...>"\
	"\r\n",
	prvParameterEchoCommand, /* The function to run. */
	-1 /* The user can enter any number of commands. */
},

//END OF TABLE
{
    NULL,
    NULL,
    NULL,
    0
},
};

/*-----------------------------------------------------------*/

void vRegisterCLICommands( void )
{
    int i = 0;
	/* Register all the command line commands defined immediately above. */
    for (i = 0; NULL != CLICommands[i].pxCommandInterpreter; i++)
    {
        FreeRTOS_CLIRegisterCommand(&CLICommands[i]);
    }
}
/*-----------------------------------------------------------*/

/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Examples 5,4,1 02-08-2010 (biosEx-j14)
 */
/*
 *  ======== swi.c =======
 *
 */

//#include <std.h>
//
//#include <log.h>
//#include <swi.h>
//#include <sys.h>
//
//#include "swicfg.h"
//
//Void swiFxn0(Void);
//Void swiFxn1(Void);
//
///*
// *  ======== main ========
// */
//Void main(Int argc, Char *argv[])
//{
//    LOG_printf(&trace,"swi example started!\n");
//    LOG_printf(&trace,"Main posts SWI0\n");
//    SWI_post(&SWI0);
//        LOG_printf(&trace,"Main done!\n");
//}
//
///*
// *  ======== swiFxn0 ========
// */
//Void swiFxn0(Void)
//{
//    LOG_printf(&trace,"swiFxn0 posts SWI1\n");
//        SWI_post(&SWI1);
//        LOG_printf(&trace,"SWI0 done!\n");
//}
//
///*
// *  ======== swiFxn1 ========
// */
//Void swiFxn1(Void)
//{
//        LOG_printf(&trace,"SWI1 done!\n");
//}

#include <std.h>

#include <log.h>
#include <clk.h>
#include <tsk.h>
#include <gbl.h>

#include "bioscfg.h"
#include "c55.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "ezdsp5502_i2cgpio.h"
#include "csl_mcbsp.h"
#include "stdio.h"
#include "string.h"
#include "graphics.h"


extern void audioProcessingInit(void);
extern void ToggleMute();
extern uint16_t GetMute();
extern void SetFilter(Uint8 type);

volatile int counter = 0;

Uint8 sw1State = 0;       // SW1 state
Uint8 sw2State = 0;       // SW2 state
Uint8 filterType = 0;

void main(void)

{
    /* Initialize BSL */
    EZDSP5502_init( );

    LOG_enable(&trace);

    // configure the Codec chip
    setup_aic3204();

    /* Initialize I2S */
    EZDSP5502_MCBSP_init();

    screen_start();
    select_screen(0);
    screen_string("FILTER TYPE: NONE");
    select_screen(1);
    screen_string("MUTE: OFF ");

    /* enable the interrupt with BIOS call */
    C55_enableInt(7); // reference technical manual, I2S2 tx interrupt
    C55_enableInt(6); // reference technical manual, I2S2 rx interrupt

    audioProcessingInit();

    // after main() exits the DSP/BIOS scheduler starts
    LOG_printf(&trace, "Finished Main");
}


Void FXN_IDL_SWITCH(void)
{

//    while(1)
    {
        if(!(EZDSP5502_I2CGPIO_readLine(SW0))) // Is SW1 pressed?
           {
               if(sw1State)          // Was previous state not pressed?
               {
                   filterType++;
                   if(filterType >= 3)
                       filterType = 0;
                   SetFilter(filterType);     // Change filter
                   select_screen(0);
                   switch(filterType)
                   {
                       case 0:
                           screen_string("NONE");
                           break;
                       case 1:
                           screen_string("HPF ");
                           break;
                       case 2:
                           screen_string("LPF ");
                           break;
                       default:
                           screen_string("NONE");
                           break;
                   }
                   sw1State = 0;     // Set state to 0 to allow only single press
               }
           }
           else                      // SW1 not pressed
               sw1State = 1;         // Set state to 1 to allow timer change

           /* Check SW2 */
           if(!(EZDSP5502_I2CGPIO_readLine(SW1))) // Is SW2 pressed?
           {
               if(sw2State)          // Was previous state not pressed?
               {
                   ToggleMute();     // Mute
                   select_screen(1);
                   switch(GetMute())
                   {
                       case 0:
                           screen_string("OFF ");
                           break;
                       case 1:
                           screen_string("ON  ");
                           break;
                       default:
                           screen_string("UNK ");
                           break;
                   }

                   sw2State = 0;     // Set state to 0 to allow only single press
               }
           }
           else                      // SW2 not pressed
               sw2State = 1;         // Set state to 1 to allow tone change
    }
}







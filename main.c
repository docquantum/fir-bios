/*
 * Daniel Shchur
 * FIR-BIOS Main
 */

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
#include "graphics.h"
#include "led.h"

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

    /* Initialize LEDS */
    InitLeds();
    TurnOnLed(0);

    /* Setup I2C GPIOs for Switches */
    EZDSP5502_I2CGPIO_configLine(SW0, IN);
    EZDSP5502_I2CGPIO_configLine(SW1, IN);

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
                       TurnOnLed(0);
                       TurnOffLed(1);
                       TurnOffLed(2);
                       screen_string("NONE");
                       break;
                   case 1:
                       TurnOffLed(0);
                       TurnOnLed(1);
                       TurnOffLed(2);
                       screen_string("HPF ");
                       break;
                   case 2:
                       TurnOffLed(0);
                       TurnOffLed(1);
                       TurnOnLed(2);
                       screen_string("LPF ");
                       break;
                   default:
                       TurnOnLed(0);
                       TurnOffLed(1);
                       TurnOffLed(2);
                       screen_string("UNK ");
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
                       TurnOffLed(3);
                       screen_string("OFF ");
                       break;
                   case 1:
                       TurnOnLed(3);
                       screen_string("ON  ");
                       break;
                   default:
                       TurnOffLed(3);
                       screen_string("UNK ");
                       break;
               }

               sw2State = 0;     // Set state to 0 to allow only single press
           }
       }
       else                      // SW2 not pressed
           sw2State = 1;         // Set state to 1 to allow tone change
}







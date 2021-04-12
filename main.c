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
#include "custom_types.h"

void TSK_DisplayManager(void);

extern void audioProcessingInit(void);
extern uint16_t GetMute();

void main(void)

{
    /* Initialize BSL */
    EZDSP5502_init( );

    /* Setup I2C GPIOs for Switches */
    EZDSP5502_I2CGPIO_configLine(SW0, IN);
    EZDSP5502_I2CGPIO_configLine(SW1, IN);

    LOG_enable(&trace);

    // configure the Codec chip
    setup_aic3204();

    /* Initialize I2S */
    EZDSP5502_MCBSP_init();

    /* enable the interrupt with BIOS call */
    C55_enableInt(7); // reference technical manual, I2S2 tx interrupt
    C55_enableInt(6); // reference technical manual, I2S2 rx interrupt

    audioProcessingInit();

    /* Initialize LEDS */
    InitLeds();
    TurnOnLed(0);
    /* Initialize Screen */
    screen_start();

    // after main() exits the DSP/BIOS scheduler starts
    LOG_printf(&trace, "Finished Main");
}

void TSK_UserInterface(void)
{
    Uint8 sw1State = 0;
    Uint8 sw2State = 0;
    Uint8 filterType = 1;
    Uint8 muteState = 0;
    while(1)
    {
        TSK_sleep(80);
        /* Check SW1 */
        LCK_pend(&i2c_lock, SYS_FOREVER);
        if(!(EZDSP5502_I2CGPIO_readLine(SW0)))
        {
            if(sw1State)
            {
                filterType++;
                if(filterType >= 4)
                    filterType = 1;
                MBX_post(&mbx_command, &filterType, SYS_FOREVER);
                switch(filterType)
                {
                    case FILTER_NONE:
                        TurnOnLed(0);
                        TurnOffLed(1);
                        TurnOffLed(2);
                        break;
                    case FILTER_HPF:
                        TurnOffLed(0);
                        TurnOnLed(1);
                        TurnOffLed(2);
                        break;
                    case FILTER_LPF:
                        TurnOffLed(0);
                        TurnOffLed(1);
                        TurnOnLed(2);
                        break;
                    default:
                        TurnOnLed(0);
                        TurnOffLed(1);
                        TurnOffLed(2);
                        break;
                }
                sw1State = 0;
            }
        }
        else
            sw1State = 1;

        LCK_post(&i2c_lock);

        /* Check SW2 */
        LCK_pend(&i2c_lock, SYS_FOREVER);
        if(!(EZDSP5502_I2CGPIO_readLine(SW1)))
        {
            if(sw2State)
            {
                sw2State = 0;
                MBX_post(&mbx_command, &sw2State, SYS_FOREVER);
                muteState = muteState ? 0 : 1;
                switch(muteState)
                {
                    case 0:
                        TurnOffLed(3);
                        break;
                    case 1:
                        TurnOnLed(3);
                        break;
                    default:
                        TurnOffLed(3);
                        break;
                }
            }
        }
        else
            sw2State = 1;
        LCK_post(&i2c_lock);
    }
}

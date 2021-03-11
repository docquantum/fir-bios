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
//#include "clkcfg.h"

#include "bioscfg.h"
#include "c55.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "stdio.h"
#include "string.h"


extern void audioProcessingInit(void);

volatile int counter = 0;

void main(void)
{
    /* Initialize BSL */
    EZDSP5502_init( );

    LOG_enable(&trace);

    // configure the Codec chip
    setup_aic3204();

    /* Initialize I2S */
    EZDSP5502_MCBSP_init();

    /* enable the interrupt with BIOS call */
    C55_enableInt(7); // reference technical manual, I2S2 tx interrupt
    C55_enableInt(6); // reference technical manual, I2S2 rx interrupt

    audioProcessingInit();

    // after main() exits the DSP/BIOS scheduler starts
    LOG_printf(&trace, "Finished Main");
}

Void taskFxn(Arg value_arg)
{
    LgUns prevHtime, currHtime;
    uint32_t delta;
    float ncycles;

    /* get cpu cycles per htime count */
    ncycles = CLK_cpuCyclesPerHtime();

    while(1)
    {
        TSK_sleep(1);
        LOG_printf(&trace, "task running! Time is: %d ticks", (Int)TSK_time());

        prevHtime = currHtime;
        currHtime = CLK_gethtime();

        delta = (currHtime - prevHtime) * ncycles;
        LOG_printf(&trace, "CPU cycles = 0x%x %x", (uint16_t)(delta >> 16), (uint16_t)(delta));

    }
}

void myIDLThread(void)
{
    counter++;
}

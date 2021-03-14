/*
 * led.c
 *
 *  Created on: Mar 14, 2021
 *      Author: docquantum
 */

#include "ezdsp5502_i2cgpio.h"
#include "led.h"

void InitLeds(void)
{
    /* Setup I2C GPIO directions for LEDs */
    EZDSP5502_I2CGPIO_configLine(LED0, OUT);
    EZDSP5502_I2CGPIO_configLine(LED1, OUT);
    EZDSP5502_I2CGPIO_configLine(LED2, OUT);
    EZDSP5502_I2CGPIO_configLine(LED3, OUT);

    /* Turn off all LEDs */
    EZDSP5502_I2CGPIO_writeLine(LED0, HIGH);
    EZDSP5502_I2CGPIO_writeLine(LED1, HIGH);
    EZDSP5502_I2CGPIO_writeLine(LED2, HIGH);
    EZDSP5502_I2CGPIO_writeLine(LED3, HIGH);
}

void TurnOnLed(Uint8 led)
{
    switch(led)
    {
        case 0:
            led = LED0;
            break;
        case 1:
            led = LED1;
            break;
        case 2:
            led = LED2;
            break;
        case 3:
            led = LED3;
            break;
        default:
            return;
    }
    EZDSP5502_I2CGPIO_writeLine(led, LOW);
}

void TurnOffLed(Uint8 led)
{
    switch(led)
    {
        case 0:
            led = LED0;
            break;
        case 1:
            led = LED1;
            break;
        case 2:
            led = LED2;
            break;
        case 3:
            led = LED3;
            break;
        default:
            return;
    }
    EZDSP5502_I2CGPIO_writeLine(led, HIGH);
}

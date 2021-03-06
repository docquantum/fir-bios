/*
 * graphics.c
 *
 *  Created on: Mar 11, 2021
 *      Author: docquantum
 */

#include "graphics.h"
#include "ezdsp5502.h"
#include "lcd.h"
#include "bioscfg.h"

/**
 * Manages the display, writing frames to the LCD
 */
void TSK_DisplayManager(void)
{
    Uint16 fb[96];
    while(1)
    {
        MBX_pend(&mbx_frameBuffer, fb, SYS_FOREVER);
        LCK_pend(&i2c_lock, SYS_FOREVER);
        osd9616_frameBuffer(fb);
        LCK_post(&i2c_lock);
    }
}

void screen_start()
{

    /* Initialize  Display */
    osd9616_init();

    clear_screen();

    return;
}

void clear_screen()
{
    // Set pointer back to start
    // TODO: Figure out how to reset the pointer to start of memory
    // Fill buffer with 0s
    Uint16 i, fb[96];
    for (i=0; i < 96; i++)
    {
        fb[i] = 0x0000;
    }
    // Send buffer to screen
    osd9616_frameBuffer(fb);
}

// TODO: switch to the new frame buffer system, and reverse characters
// TODO: Finish creating missing characters
uint16_t char_screen(char c)
{
    switch(c)
    {
        case 'A':
            printLetter(0x7C, 0x09, 0x0A, 0x7C);
            break;
        case 'B':
            printLetter(0x36, 0x49, 0x49, 0x7F);
            break;
        case 'C':
            printLetter(0x22, 0x41, 0x41, 0x3E);
            break;
        case 'D':
            printLetter(0x3E, 0x41, 0x41, 0x7F);
            break;
        case 'E':
            printLetter(0x41, 0x49, 0x49, 0x7F);
            break;
        case 'F':
            printLetter(0x01, 0x09, 0x09, 0x7F);
            break;
        case 'G':
        case 'H':
            printLetter(0x7F, 0x08, 0x08, 0x7F);
            break;
        case 'I':
            printLetter(0x00, 0x7F, 0x00, 0x00);
            break;
        case 'J':
        case 'K':
        case 'L':
            printLetter(0x40, 0x40, 0x40, 0x7F);
            break;
        case 'M':
            printLetter(0x7F, 0x06, 0x06, 0x7F);
            break;
        case 'N':
            printLetter(0x7F, 0x30, 0x0E, 0x7F);
            break;
        case 'O':
            printLetter(0x3E, 0x41, 0x41, 0x3E);
            break;
        case 'P':
            printLetter(0x06, 0x09, 0x09, 0x7F);
            break;
        case 'Q':
        case 'R':
            printLetter(0x46, 0x29, 0x19, 0x7F);
            break;
        case 'S':
            printLetter(0x32, 0x49, 0x49, 0x26);
            break;
        case 'T':
            printLetter(0x01, 0x7F, 0x01, 0x01);
            break;
        case 'U':
            printLetter(0x3F, 0x40, 0x40, 0x3F);
            break;
        case 'V':
        case 'W':
            printLetter(0x7F, 0x30, 0x30, 0x7F);
            break;
        case 'X':
            printLetter(0x63, 0x1C, 0x1C, 0x63);
            break;
        case 'Y':
            printLetter(0x07, 0x78, 0x08, 0x07);
            break;
        case 'Z':
            printLetter(0x43, 0x4D, 0x51, 0x61);
            break;
        case ' ':
            printLetter(0x00, 0x00, 0x00, 0x00);
            break;
        case '0':
            printLetter(0x3E, 0x49, 0x45, 0x3E);
            break;
        case '1':
            printLetter(0x40, 0x7F, 0x42, 0x00);
            break;
        case '2':
            printLetter(0x47, 0x49, 0x51, 0x62);
            break;
        case '3':
        case '4':
        case '5':
            printLetter(0x31, 0x49, 0x49, 0x2F);
            break;
        case '6':
        case '7':
        case '8':
        case '9':
        case '!':
            printLetter(0x00, 0x00, 0x5F, 0x00);
            break;
        case '?':
        case '.':
            printLetter(0x00, 0x00, 0x40, 0x00);
            break;
        case ',':
            printLetter(0x00, 0x20, 0x40, 0x00);
            break;
        case ':':
            printLetter(0x00, 0x00, 0x14, 0x00);
            break;
        case '=':
            printLetter(0x14, 0x14, 0x14, 0x14);
            break;
        default:
            return 1;
    }
    return 0;
}

// TODO: switch to the new frame buffer system, and reverse characters
uint16_t string_screen(char* str)
{
    int count = 0;
    char* start = str - 1;
    while (*str != '\0')
    {
        count++;
        str++;
    }
    if(count > 19)
    {
        return 1;
    }
    while (str != start)
    {
        char_screen(*str);
        str--;
    }
    return 0;
}

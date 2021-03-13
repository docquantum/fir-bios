/*
 * graphics.h
 *
 *  Created on: Mar 11, 2021
 *      Author: docquantum
 */

#ifndef INCLUDE_GRAPHICS_H_
#define INCLUDE_GRAPHICS_H_
#include "ezdsp5502.h"
#include "stdint.h"

void screen_start();
void select_screen(Uint8 screen);
void clear_screen(Uint8 screen);
uint16_t screen_char(char c);
uint16_t screen_string(char* str);

#endif /* INCLUDE_GRAPHICS_H_ */

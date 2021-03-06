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
void clear_screen();
uint16_t char_screen(char c);
uint16_t string_screen(char* str);

#endif /* INCLUDE_GRAPHICS_H_ */

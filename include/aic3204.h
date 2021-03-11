/*
 * aic3204.h
 *
 *  Created on: Feb 10, 2021
 *      Author: docquantum
 */

#ifndef AIC3204_H_
#define AIC3204_H_

#define AIC3204_I2C_ADDR  0x18

Int16 AIC3204_rset( Uint16 regnum, Uint16 regval );
Int16 AIC3204_rget(  Uint16 regnum, Uint16* regval );
void setup_aic3204();
void cleanup_aic3204();

Int16 read_sample();
void play_sample(Int16 sample, Uint8 amplitude);

#endif /* AIC3204_H_ */

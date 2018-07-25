/*
 * LEDMatrix.h
 *
 *  Created on: Jul 24, 2018
 *      Author: rscovill
 */

#ifndef PONG_LEDMATRIX_H_
#define PONG_LEDMATRIX_H_

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 16
void LEDMatrix_init();
void LEDMatrix_update(int in_m[][SCREEN_HEIGHT]);
void LEDMatrix_clear();

#endif /* PONG_LEDMATRIX_H_ */

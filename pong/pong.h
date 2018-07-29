/*
 * pong.h
 *
 *  Created on: Jul 24, 2018
 *      Author: rscovill
 */

#ifndef PONG_PONG_H_
#define PONG_PONG_H_

void Pong_init(int player);
void Pong_movePaddle(int player, int dir);
void Pong_increaseReadyCount(void);
void Pong_resetGame(void);

#endif /* PONG_PONG_H_ */

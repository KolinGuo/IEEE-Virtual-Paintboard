// Copyright 2019 Kolin Guo
#ifndef IRDATA_H
#define IRDATA_H

// IR Data for Keys
// The data corresponds to the 4-digit TV Device code: 1156
#define KEY0 		0b00000000
#define KEY1 		0b10000000
#define KEY2 		0b01000000
#define KEY3 		0b11000000
#define KEY4 		0b00100000
#define KEY5 		0b10100000
#define KEY6 		0b01100000
#define KEY7 		0b11100000
#define KEY8 		0b00010000
#define KEY9 		0b10010000
#define ENTER       0b11101000
#define LAST 		0b00000010
#define MUTE 		0b00001000
#define VOL_PLUS 	0b01011000      // Same as RIGHT
#define VOL_MINUS 	0b01111000      // Same as LEFT
#define CH_PLUS		0b11011000      // Same as UP
#define CH_MINUS	0b11111000      // Same as DOWN
#define EXITTOTV	0b00011010
#define INFO		0b01101000
#define LEFT		0b01111000
#define RIGHT		0b01011000
#define UP			0b11011000
#define DOWN		0b11111000
#define OK			0b00000001
#define MENU		0b11011010
#define ON_DEMAND	0b00001001
#define TV_VIDEO	0b00101000
#define POWER		0b01001000

#endif 	// IRDATA_H

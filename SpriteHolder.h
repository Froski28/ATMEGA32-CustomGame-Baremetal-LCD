/*
 * SpriteHolder.h
 *
 * Created: 6/2/2025 11:59:14 PM
 *  Author: jujuf
 */ 


#ifndef SPRITEHOLDER_H_
#define SPRITEHOLDER_H_

void create_SpriteRunning1(uint8_t* holder)
{
	holder[0] = 0x0E;
	holder[1] =	0x19;		//0001 1001 
	holder[2] =	0x13;		//0001 0011
	holder[3] = 0x0E;
	holder[4] =	0x14;		// 10100
	holder[5] =	0x0E;		// 01110
	holder[6] =	0x05;		// 0 0101
	holder[7] =	0x0A;		//0 1010
}

void create_SpriteRunning2(uint8_t* holder)
{
	holder[0] = 0x0E;
	holder[1] =	0x19;
	holder[2] =	0x13;
	holder[3] = 0x0E;
	holder[4] = 0x05; // 0 0101
	holder[5] = 0x0E; // 0 1110
	holder[6] = 0x14; // 1 0100
	holder[7] = 0x1B; // 1 1011
}

void create_SpriteJumping(uint8_t* holder)
{
	holder[0] = 0x0E;
	holder[1] =	0x19;
	holder[2] =	0x13;
	holder[3] = 0x0E;
	holder[4] =	0x15;		// 1 0101
	holder[5] =	0x0E;		// 0 1110
	holder[6] = 0x04;			// 0 0100
	holder[7] = 0x1B;			// 1 1011
}

void create_SpriteSliding(uint8_t* holder)
{
	holder[0] = 0;
	holder[1] = 0;
	holder[2] = 0;
	holder[3] = 0;
	holder[4] = 0x0E;
	holder[5] = 0x19;
	holder[6] = 0x13;
	holder[7] = 0x0E;
}


void create_SpriteLongPipeHolder(uint8_t* holder)
{
	holder[0] = 0x0E;
	holder[1] = 0x0E;
	holder[2] = 0x0E;
	holder[3] = 0x0E;
	holder[4] = 0x0E;
	holder[5] = 0x0E;
	holder[6] = 0x0E;
	holder[7] = 0x0E;
}

void create_SpriteShortPipeUp(uint8_t* holder)
{
	holder[0] = 0;
	holder[1] = 0x1F;
	holder[2] = 0x1F;
	holder[3] = 0x1F;
	holder[4] = 0x0E;
	holder[5] = 0x0E; 
	holder[6] = 0x0E;
	holder[7] = 0x0E;
}

void create_SpriteSlidingShortPipeDown(uint8_t* holder)
{
	holder[0] = 0x0E;
	holder[1] = 0x1F;
	holder[2] = 0x1F;
	holder[3] = 0;
	holder[4] = 0x0E;
	holder[5] = 0x19;
	holder[6] = 0x13;
	holder[7] = 0x0E;
}

void create_SpriteShortPipeDown(uint8_t* holder)
{
	holder[0] = 0x0E;
	holder[1] = 0x1F;
	holder[2] = 0x1F;
	holder[3] = 0;
	holder[4] = 0;
	holder[5] = 0;
	holder[6] = 0;
	holder[7] = 0;
}



#endif /* SPRITEHOLDER_H_ */
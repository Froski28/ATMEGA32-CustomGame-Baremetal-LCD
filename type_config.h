/*
 * type_config.h
 *
 * Created: 6/1/2025 3:08:03 PM
 *  Author: jujuf
 */ 


#ifndef TYPE_CONFIG_H_
#define TYPE_CONFIG_H_


typedef enum {LongPipeSlide, ShortPipeJump, ShortPipeSlide, Platform, DoublePipeJump} Objects;
#define TYPE Objects

typedef enum {SpriteNone=0x1A, Spriterunning1=0, Spriterunning2=1, SpriteJumping=2, SpriteSliding=3, SpriteLongPipeHolder=4, SpriteShortPipeUp=5, SpriteSlidingShortPipeDown=6, SpriteShortPipeDown=7} Sprites;

typedef enum {easy, medium, hard} Difficulty;

typedef enum {start, running, dead, chooseDifficulty} States;

typedef enum {moveCursorSound, selectSound} StateSounds;

typedef enum {OnGround, InAir, Sliding, OnPlatform, SlidingOnPlatform} PlayerStates;

typedef enum {DeathSound, JumpSound, IdleSound} PlayerSounds;

typedef struct {
	PlayerStates currentState;
	Sprites prevSprite;
	PlayerSounds currentSound;
	int jump_slide_frames;
	Difficulty difficulty;
} Player;

#endif /* TYPE_CONFIG_H_ */
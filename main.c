/*
 * Project5.c
 *
 * Created: 5/31/2025 10:14:04 PM
 * Author : jujuf
 */ 

#include "avr.h"
#include "my_lcd.h"
#include "keypad.h"
#include "type_config.h"
#include "my_queue.h"
#include "SpriteHolder.h"
#include <stdlib.h>

#define GAME_SPEED 125
#define ISR_GAME_SPEED 150
#define PLAYER_ANIM_SPEED 200
#define MAX_JUMP_FRAMES 2
#define MAX_SLIDE_FRAMES 2
#define PLATFORM 0xDB
#define SPECIAL_PLATFORM 0xBA


void timer1()
{
	// Set to mode (Clear Timer on Match)
	TCCR1A = 0; // Disable PWM
	TCCR1B |= (1 << WGM12);
	// Sec 1 second delay - formula in notes
	OCR1A = (uint16_t)(((float)(8000000.0) * (ISR_GAME_SPEED / 1000.0)) / 256) - 1;
	// Sets the prescaler (256)
	TCCR1B |= (1 << CS12);
	// Enable OCR1A so it can flag timer
	TIMSK |= (1 << OCIE1A);
	// Enable the global interrupt
	sei();
}

volatile uint8_t RUN_TIMER_FLAG = 0;

ISR(TIMER1_COMPA_vect)
{
	RUN_TIMER_FLAG = 1;
}

void spawn_specific_object(Objects object){
	
	switch (object){
		case LongPipeSlide:
			LCD_write_char(SpriteLongPipeHolder, 16, 0);
			LCD_write_char(SpriteShortPipeDown, 16, 1);
		break;
		case ShortPipeSlide:
			LCD_write_char(SpriteShortPipeDown, 16, 0);
			LCD_write_char(SPECIAL_PLATFORM, 16, 1);
		break;
		case DoublePipeJump:
		case ShortPipeJump: LCD_write_char(SpriteShortPipeUp, 16, 1); break;
		case Platform: LCD_write_char(PLATFORM, 16, 1); break;
	}
}

void set_object_queue(Queue* q)
{
	if (q->isFull(q)) return;
	else if (q->size <= 1){
		while (q->size < SIZE){
			int obj_num = (1 + random() % 100) % 4;
			// 5-Platform 6-ShortPipeUp 7-ShortPipeDownSprite 8-LongPipeDown are objects
			if (obj_num == 0){
				int min = 3; int max = 5;
				int amount = min + ((random() % (max+1 - min)));
				amount = (amount < 1) ? 2: 3;
				for (int i=0; i < amount; ++i){
					if (i > 1 && (random() % 2))
						q->push(q, ShortPipeSlide);
					else
						q->push(q, Platform);
				}
				//q->push(q, Platform);
			}
			else if (obj_num == 1){
				q->push(q, ShortPipeJump);
			}
			else if (obj_num == 2){	/// Now Double Pipe Jump
				q->push(q, DoublePipeJump);
				q->push(q, DoublePipeJump);
			}
			else if (obj_num == 3){
				q->push(q, LongPipeSlide);
			}
		}
		
	}
	
}

Sprites get_next_player_sprite(Player* player){
	Sprites next_sprite = 'E';
	uint8_t isSlidingGround = 1;
	switch (player->currentState){
		case InAir: 
			if (player->jump_slide_frames <= MAX_JUMP_FRAMES){
				player->prevSprite = SpriteJumping;
				next_sprite = SpriteJumping;
			}
			else{
				player->prevSprite = Spriterunning1;
				next_sprite = Spriterunning1;
			}
			break;
		case SlidingOnPlatform: isSlidingGround = 0;
		case Sliding:
			if (LCD_get_at_pos(3, isSlidingGround) == SpriteShortPipeDown){
				player->prevSprite = SpriteSliding;
				next_sprite = SpriteSlidingShortPipeDown;
			}
			else if (player->jump_slide_frames <= MAX_SLIDE_FRAMES){
				player->prevSprite = SpriteSliding;
				next_sprite = SpriteSliding;
			}
			else {
				player->prevSprite = Spriterunning1;
				next_sprite = Spriterunning1;
			}
			break;
		case OnGround:
			if (player->prevSprite == Spriterunning1){
				player->prevSprite = Spriterunning2;
				next_sprite = Spriterunning2;
			}
			else{
				player->prevSprite = Spriterunning1;
				next_sprite = Spriterunning1;
			}
			break;
		case OnPlatform: 
			if (player->prevSprite == Spriterunning1){
				player->prevSprite = Spriterunning2;
				next_sprite = Spriterunning2;
			}
			else{
				player->prevSprite = Spriterunning1;
				next_sprite = Spriterunning1;
			}
			break;
	}
	if (LCD_get_at_pos(2, 0) == SpriteLongPipeHolder) LCD_write_char(SpriteShortPipeDown, 2, 1);
	if (LCD_get_at_pos(2, 1) == SPECIAL_PLATFORM) {LCD_write_char(SpriteShortPipeDown, 2, 0);}
	return next_sprite;
}

void show_hide_player(PlayerStates player_state, uint8_t isShowing, Sprites sprite){ // Player on Col3
	uint8_t pos = (player_state == InAir || player_state == OnPlatform || player_state == SlidingOnPlatform) ? 0: 1;
	uint8_t data = (isShowing) ? sprite: ' ';
	LCD_write_char(data, 3, pos);
}

// A for Jump and B for Slide
void process_input(Player* player, KeyPad input)
{
	switch (input){
		case KEYPAD_A:
		if (player->currentState != InAir && player->currentState != OnPlatform){
			player->currentState = InAir;
			LCD_write_char(' ', 3, 0);
		}
		break; // jump
		case KEYPAD_B:
			if (player->currentState == OnGround) player->currentState = Sliding;
			else if (player->currentState == OnPlatform) player->currentState = SlidingOnPlatform;
		break; // Slide
		default: break;
	}
}

Bool is_platform_under_player(){
	char data = LCD_get_at_pos(3, 1);
	if (data == PLATFORM) return true;
	else if (data == SPECIAL_PLATFORM) return true;
	else if (data == 'P') return true;
	else return false;
}

States check_collision(Player* player)
{
	uint8_t height = (player->currentState == InAir || player->currentState == OnPlatform || player->currentState == SlidingOnPlatform) ? 0: 1;
	uint8_t object = LCD_get_at_pos(3, height);
	
	if (object != ' ' && object != SpriteShortPipeDown) return dead;
	else if (object == SpriteShortPipeDown){
		if (height && player->currentState != Sliding) return dead;
		else if (!height && player->currentState != SlidingOnPlatform) return dead;
	}
	return running;
}

void run_physics(Player* player)
{
	if (player->currentState == InAir){
		if (player->jump_slide_frames >= MAX_JUMP_FRAMES){
			player->jump_slide_frames = 0;
			player->currentState = (is_platform_under_player()) ? OnPlatform: OnGround;
			if (player->currentState == OnGround) {
				//show_hide_player(InAir, 0, SpriteNone);
				//show_hide_player(OnGround, 1, Spriterunning1);
			}
		}
		else ++player->jump_slide_frames;
	}
	else if (player->currentState == Sliding){
		if (player->jump_slide_frames >= MAX_SLIDE_FRAMES){
			player->jump_slide_frames = 0;
			player->currentState = OnGround;
		}
		else ++player->jump_slide_frames;
	}
	else if (player->currentState == SlidingOnPlatform || is_platform_under_player()){
		if (player->jump_slide_frames >= MAX_SLIDE_FRAMES){
			player->jump_slide_frames = 0;
			player->currentState = (is_platform_under_player()) ? OnPlatform: OnGround;
		}
		else ++player->jump_slide_frames;
	}
	else if (player->currentState == OnPlatform){
		if (!is_platform_under_player()){
			player->currentState = OnGround;
			//show_hide_player(InAir, 0, SpriteNone);
			//show_hide_player(OnGround, 1, Spriterunning2);
		}
	}
}

States process_frame(Player* player, KeyPad input, Queue* objectQueue, States state, uint8_t isSpawning, Objects* previousSpawnObject)
{
	// Stop Showing Player
	show_hide_player(player->currentState, 0, SpriteNone);
	// Shift Screen						// Note* When shifting screen if object collides with player re-display player over it - would be collision
	LCD_shift_display_rows(0, 0);
	// Spawn Object(s)
		// Set Spawning Queue
		set_object_queue(objectQueue);
		// Spawn Object if time
		if (isSpawning){
			*previousSpawnObject = objectQueue->top(objectQueue);
			spawn_specific_object(*previousSpawnObject);
			objectQueue->pop(objectQueue);
		}
	// Process input (Jumping, Sliding)
	process_input(player, input);
	// Run Physics'
	run_physics(player);
	// Check for Collision
	state = check_collision(player);
	// Show Player
	show_hide_player(player->currentState, 1, get_next_player_sprite(player));
	return state;
}

Bool is_spawning_object(int* frameSinceLastSpawn, Difficulty currentDifficulty, Objects currentSpawnObject, Objects previousSpawnObject, uint8_t* islastObjDoublePipe)
{
	// Check if MultiObject
	if (previousSpawnObject == Platform && (currentSpawnObject == Platform || currentSpawnObject == ShortPipeSlide)) {
		*frameSinceLastSpawn = 0;
		return true;
	}
	else if (previousSpawnObject == DoublePipeJump && currentSpawnObject == DoublePipeJump && !(*islastObjDoublePipe)){
		*frameSinceLastSpawn = 0;
		*islastObjDoublePipe = 1;
		return true;
	}
	
	// Check Difficulty & Frames
	int low, high;
	switch (currentDifficulty)
	{
		case easy:
			low = 4;
			high = 6;
			break;
		case hard:
			low = 1;
			high = 2;
			break;
		case medium:
		default:
			low = 3;
			high = 5;
			break;
	}
	
	int val = 1 + random() % 100;
	val = (val % high) + low;
	if (*frameSinceLastSpawn >= val) {
		*frameSinceLastSpawn = 0;
		*islastObjDoublePipe = 0;
		return true;
	}
	return false;
}

void playerInit(Player* player, Difficulty diff){
	player->currentState = OnGround;
	player->prevSprite = SpriteNone;
	player->currentSound = IdleSound;
	player->difficulty = diff;
	player->jump_slide_frames = 0;
}

States run_game(Difficulty diff, unsigned int seed_timer)
{									// PLAYER on Col3
	LCD_clear();
	srandom(seed_timer);
	// Set State
	States state = running;
	// Initialize Object Queue
	Queue objectQueue; Queue_init(&objectQueue);
	// Initialize Frame Counter for Object Spawning
	int frameTracker = 0;
	// Previous Holder Object
	Objects previousHolder = 0;
	uint8_t islastObjDoublePipe = 0;
	// Initialize Player
	Player player; playerInit(&player, diff);
		//if (player.currentState == OnGround) LCD_write_char('P', 1, 1);
	avr_wait(200);
	while (state == running){
		if (RUN_TIMER_FLAG){
			KeyPad input = get_key_press();
			uint8_t isSpawning = (objectQueue.isEmpty(&objectQueue)) ? 0: is_spawning_object(&frameTracker, player.difficulty, objectQueue.top(&objectQueue), previousHolder, &islastObjDoublePipe);
			state = process_frame(&player, input, &objectQueue, state, isSpawning, &previousHolder);
			++frameTracker;
			RUN_TIMER_FLAG = 0;
		}
	}
	return state;
}

States process_start_actions(States state, KeyPad input, Difficulty* diff)
{
	switch (input){
		case KEYPAD_A:
		state = running;
		break;
		case KEYPAD_B:
		if (*diff == easy){
			*diff = medium;
			LCD_write_char('M', 16, 1);
		}
		else if (*diff == medium){
			*diff = hard;
			LCD_write_char('H', 16, 1);
		}
		else {
			*diff = easy;
			LCD_write_char('E', 16, 1);
		}
		break;
		default: break;
	}
	return state;
}

States run_start(Difficulty* diff, unsigned int* seed_timer){
	avr_wait(100);
	States state = start;
	LCD_clear();
	LCD_write_string("Guy Run", 6, 0);
	LCD_write_string("A:Start B:Diff:M", 1, 1);
	while (state == start){
		KeyPad input = get_key_press();
		state = process_start_actions(state, input, diff);
		if (*seed_timer > 60000) *seed_timer = 0;
		++(*seed_timer);
		avr_wait(100);
	}
	return state;
}

States run_death(States state)
{
	LCD_clear();
	LCD_write_string("    YOU DIED  :(", 1, 0);
	LCD_write_string("A:Restart?", 4, 1);
	
	while (state == dead){
		KeyPad input = get_key_press();
		if (input == KEYPAD_A) state = start;
		avr_wait(150);	
	}
	return state;
}

/*Changes the state and runs the code accordingly*/
States state_controller(States state, Difficulty* diff, unsigned int* seed_timer)
{
	switch (state){
		case start: 
		state = run_start(diff, seed_timer);
		break;
		case running:
		state = run_game(*diff, *seed_timer);
		break;
		case dead:
		avr_wait(1000);
		state = run_death(state);
		break;
		default: break;
	}
	return state;
}

void write_sprite(uint8_t* sprite, uint8_t num){
	LCD_write_data((0x40 + (num*8)), 0, 0, 0);
	for (int i=0; i < 8; ++i)
		LCD_write_data(*(sprite + i), 1, 0, 0);
}

void Sprite_Init()
{
	uint8_t running1[8]; create_SpriteRunning1(running1);
	write_sprite(running1, 0);
	
	uint8_t running2[8]; create_SpriteRunning2(running2);
	write_sprite(running2, 1);
	
	uint8_t jumping[8]; create_SpriteJumping(jumping);
	write_sprite(jumping, 2);
	
	uint8_t sliding[8]; create_SpriteSliding(sliding);
	write_sprite(sliding, 3);
	
	uint8_t longPipeHolder[8]; create_SpriteLongPipeHolder(longPipeHolder);
	write_sprite(longPipeHolder, 4);
	
	uint8_t shortPipeUp[8]; create_SpriteShortPipeUp(shortPipeUp);
	write_sprite(shortPipeUp, 5);
	
	uint8_t slidingShortPipeDown[8]; create_SpriteSlidingShortPipeDown(slidingShortPipeDown);
	write_sprite(slidingShortPipeDown, 6);
	
	uint8_t shortPipeDown[8]; create_SpriteShortPipeDown(shortPipeDown);
	write_sprite(shortPipeDown, 7);
}

int main(void)
{	
	timer1();
	
	fourBit_LCDInit();
	
	LCD_TurnOnOff(1, 0, 0);
	
	LCD_write_string("Initialized...", 1, 0);
	
	LCD_shift_display_rows(1, 1);
	
	LCD_clear();
	
	Sprite_Init();
	
	States state = start;
	
	Difficulty difficulty = medium;
	
	unsigned int seed_timer = 0;
	
    /* Replace with your application code */
    while (1)
    {
		state = state_controller(state, &difficulty, &seed_timer);
    }
}


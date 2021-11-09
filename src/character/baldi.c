/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "baldi.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Baldi character structure
enum
{
	Baldi_ArcMain_IdleA0,
	Baldi_ArcMain_IdleA1,
	Baldi_ArcMain_IdleA2,
	Baldi_ArcMain_IdleA3,
	Baldi_ArcMain_LeftA0,
	Baldi_ArcMain_LeftA1,
	Baldi_ArcMain_LeftA2,
	Baldi_ArcMain_LeftA3,
	Baldi_ArcMain_LeftB0,
	Baldi_ArcMain_LeftB1,
	Baldi_ArcMain_DownA0,
	Baldi_ArcMain_DownA1,
	Baldi_ArcMain_DownA2,
	Baldi_ArcMain_DownA3,
	Baldi_ArcMain_DownB0,
	Baldi_ArcMain_DownB1,
	Baldi_ArcMain_UpA0,
	Baldi_ArcMain_UpA1,
	Baldi_ArcMain_UpA2,
	Baldi_ArcMain_UpA3,
	Baldi_ArcMain_UpB0,
	Baldi_ArcMain_UpB1,
	Baldi_ArcMain_RightA0,
	Baldi_ArcMain_RightA1,
	Baldi_ArcMain_RightA2,
	Baldi_ArcMain_RightA3,
	Baldi_ArcMain_RightB0,
	Baldi_ArcMain_RightB1,
	
	Baldi_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Baldi_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Baldi;

//Baldi character definitions
static const CharFrame char_baldi_frame[] = {
	{Baldi_ArcMain_IdleA0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle a 1
	{Baldi_ArcMain_IdleA1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle a 2
	{Baldi_ArcMain_IdleA2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle a 3
	{Baldi_ArcMain_IdleA3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle a 4

	{Baldi_ArcMain_LeftA0, {  0,   0, 256, 256}, { 125, 250}}, //4 left a 1
	{Baldi_ArcMain_LeftA1, {  0,   0, 256, 256}, { 125, 250}}, //5 left a 2
	{Baldi_ArcMain_LeftA2, {  0,   0, 256, 256}, { 125, 250}}, //6 left a 3
	{Baldi_ArcMain_LeftA3, {  0,   0, 256, 256}, { 125, 250}}, //7 left a 4
	
	{Baldi_ArcMain_LeftB0, {  0,   0, 256, 256}, { 125, 250}}, //8 left b 1
	{Baldi_ArcMain_LeftB1, {  0,   0, 256, 256}, { 125, 250}}, //9 left b 2
	
	{Baldi_ArcMain_DownA0, {  0,   0, 256, 256}, { 125, 250}}, //10 down a 1
	{Baldi_ArcMain_DownA1, {  0,   0, 256, 256}, { 125, 250}}, //11 down a 2
	{Baldi_ArcMain_DownA2, {  0,   0, 256, 256}, { 125, 250}}, //12 down a 3
	{Baldi_ArcMain_DownA3, {  0,   0, 256, 256}, { 125, 250}}, //13 down a 4
	
	{Baldi_ArcMain_DownB0, {  0,   0, 256, 256}, { 125, 250}}, //14 down b 1
	{Baldi_ArcMain_DownB1, {  0,   0, 256, 256}, { 125, 250}}, //15 down b 2
	
	{Baldi_ArcMain_UpA0, {   0,   0,  256, 256}, { 125, 250}}, //16 up a 1
	{Baldi_ArcMain_UpA1, {   0,   0,  256, 256}, { 125, 250}}, //17 up a 2
	{Baldi_ArcMain_UpA2, {   0,   0,  256, 256}, { 125, 250}}, //18 up a 3
	{Baldi_ArcMain_UpA3, {   0,   0,  256, 256}, { 125, 250}}, //19 up a 4
	
	{Baldi_ArcMain_UpB0, {   0,   0,  256, 256}, { 125, 250}}, //20 up b 1
	{Baldi_ArcMain_UpB1, {   0,   0,  256, 256}, { 125, 250}}, //21 up b 2
	
	{Baldi_ArcMain_RightA0, {  0,   0, 256, 256}, { 104, 250}}, //22 right a 1
	{Baldi_ArcMain_RightA1, {  0,   0, 256, 256}, { 104, 250}}, //23 right a 2
	{Baldi_ArcMain_RightA2, {  0,   0, 256, 256}, { 104, 250}}, //24 right a 3
	{Baldi_ArcMain_RightA3, {  0,   0, 256, 256}, { 104, 250}}, //25 right a 4
	
	{Baldi_ArcMain_RightB0, {  0,   0, 256, 256}, { 104, 250}}, //26 right b 1
	{Baldi_ArcMain_RightB1, {  0,   0, 256, 256}, { 104, 250}}, //27 right b 2
};

static const Animation char_baldi_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  3,  3,  3, ASCR_REPEAT, 1}},      	   			   //CharAnim_Idle
	{2, (const u8[]){ 4,  5,  6,  7,  6,  5, ASCR_BACK, 1}},       		                           //CharAnim_Left
	{2, (const u8[]){ 8,  9,  8, ASCR_BACK, 1}},                   					   //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 12, 13, 12, 11, ASCR_BACK, 1}},  		                           //CharAnim_Down
	{2, (const u8[]){14, 15, 14, ASCR_BACK, 1}},   	          	                                   //CharAnim_DownAlt
	{2, (const u8[]){16, 17, 18, 19, 18, 17, ASCR_BACK, 1}},              			           //CharAnim_Up
	{2, (const u8[]){20, 21, 20, ASCR_BACK, 1}},                                                       //CharAnim_UpAlt
	{2, (const u8[]){22, 23, 24, 25, 24, 23, ASCR_BACK, 1}},  					   //CharAnim_Right
	{2, (const u8[]){26, 27, 26, ASCR_BACK, 1}},                             			   //CharAnim_RightAlt
};

//Baldi character functions
void Char_Baldi_SetFrame(void *user, u8 frame)
{
	Char_Baldi *this = (Char_Baldi*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_baldi_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Baldi_Tick(Character *character)
{
	Char_Baldi *this = (Char_Baldi*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Baldi_SetFrame);
	Character_Draw(character, &this->tex, &char_baldi_frame[this->frame]);
}

void Char_Baldi_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Baldi_Free(Character *character)
{
	Char_Baldi *this = (Char_Baldi*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Baldi_New(fixed_t x, fixed_t y)
{
	//Allocate baldi object
	Char_Baldi *this = Mem_Alloc(sizeof(Char_Baldi));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Baldi_New] Failed to allocate baldi object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Baldi_Tick;
	this->character.set_anim = Char_Baldi_SetAnim;
	this->character.free = Char_Baldi_Free;
	
	Animatable_Init(&this->character.animatable, char_baldi_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BALDI.ARC;1");
	
	const char **pathp = (const char *[]){
		"idlea0.tim", //Baldi_ArcMain_IdleA0
		"idlea1.tim", //Baldi_ArcMain_IdleA1
		"idlea2.tim", //Baldi_ArcMain_IdleA2
		"idlea3.tim", //Baldi_ArcMain_IdleA3
		"lefta0.tim", //Baldi_ArcMain_LeftA0
		"lefta1.tim", //Baldi_ArcMain_LeftA1
		"lefta2.tim", //Baldi_ArcMain_LeftA2
		"lefta3.tim", //Baldi_ArcMain_LeftA3
		"leftb0.tim", //Baldi_ArcMain_LeftB0
		"leftb1.tim", //Baldi_ArcMain_LeftB1
		"downa0.tim", //Baldi_ArcMain_DownA0
		"downa1.tim", //Baldi_ArcMain_DownA1
		"downa2.tim", //Baldi_ArcMain_DownA2
		"downa3.tim", //Baldi_ArcMain_DownA3
		"downb0.tim", //Baldi_ArcMain_DownB0
		"downb1.tim", //Baldi_ArcMain_DownB1
		"upa0.tim",   //Baldi_ArcMain_UpA0
		"upa1.tim",   //Baldi_ArcMain_UpA1
		"upa2.tim",   //Baldi_ArcMain_UpA2
		"upa3.tim",   //Baldi_ArcMain_UpA3
		"upb0.tim",   //Baldi_ArcMain_UpB0
		"upb1.tim",   //Baldi_ArcMain_UpB1
		"righta0.tim", //Baldi_ArcMain_RightA0
		"righta1.tim", //Baldi_ArcMain_RightA1
		"righta2.tim", //Baldi_ArcMain_RightA2
		"righta3.tim", //Baldi_ArcMain_RightA3
		"rightb0.tim", //Baldi_ArcMain_RightB0
		"rightb1.tim", //Baldi_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}

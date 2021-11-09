/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "../stage/week7.h"

//GF character structure
enum
{
	GF_ArcMain_Left0,
	GF_ArcMain_Left1,
	GF_ArcMain_Left2,
	GF_ArcMain_Left3,
 	GF_ArcMain_Left4,
	GF_ArcMain_Left5,
	GF_ArcMain_Right0,
	GF_ArcMain_Right1,
	GF_ArcMain_Right2,
	GF_ArcMain_Right3,
	GF_ArcMain_Right4,
	GF_ArcMain_Right5,
	
	GF_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GF_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Pico test
	u16 *pico_p;
} Char_GF;

//GF character definitions
static const CharFrame char_gf_frame[] = {
	{GF_ArcMain_Left0, {  0,   0, 256, 256}, { 39,  74}}, //0 bop left 1
	{GF_ArcMain_Left1, {  0,   0, 256, 256}, { 39,  74}}, //1 bop left 2
	{GF_ArcMain_Left2, {  0,   0, 256, 256}, { 39,  74}}, //2 bop left 3
	{GF_ArcMain_Left3, {  0,   0, 256, 256}, { 39,  74}}, //3 bop left 4
	{GF_ArcMain_Left4, {  0,   0, 256, 256}, { 39,  74}}, //4 bop left 5
	{GF_ArcMain_Left5, {  0,   0, 256, 256}, { 39,  74}}, //5 bop left 6

	{GF_ArcMain_Right0, {  0,   0, 256, 256}, { 39,  74}}, //0 bop right 1
	{GF_ArcMain_Right1, {  0,   0, 256, 256}, { 39,  74}}, //1 bop right 2
	{GF_ArcMain_Right2, {  0,   0, 256, 256}, { 39,  74}}, //2 bop right 3
	{GF_ArcMain_Right3, {  0,   0, 256, 256}, { 39,  74}}, //3 bop right 4
	{GF_ArcMain_Right4, {  0,   0, 256, 256}, { 39,  74}}, //4 bop right 5
	{GF_ArcMain_Right5, {  0,   0, 256, 256}, { 39,  74}}, //5 bop right 6
};

static const Animation char_gf_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Idle
	{1, (const u8[]){ 0,  0,  1,  1,  2,  2,  3,  3,  4,  5, ASCR_BACK, 1}}, //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                                  //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_UpAlt
	{1, (const u8[]){ 6,  7,  8,  8,  9,  9, 10, 10, 11, 11, ASCR_BACK, 1}}, //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_RightAlt
};

//GF character functions
void Char_GF_SetFrame(void *user, u8 frame)
{
	Char_GF *this = (Char_GF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GF_Tick(Character *character)
{
	Char_GF *this = (Char_GF*)character;
	
	//Initialize Pico test
	if (stage.stage_id == StageId_7_3 && stage.back != NULL && this->pico_p == NULL)
		this->pico_p = ((Back_Week7*)stage.back)->pico_chart;
	
	if (this->pico_p != NULL)
	{
		if (stage.note_scroll >= 0)
		{
			//Scroll through Pico chart
			u16 substep = stage.note_scroll >> FIXED_SHIFT;
			while (substep >= ((*this->pico_p) & 0x7FFF))
			{
				//Play animation and bump speakers
				character->set_anim(character, ((*this->pico_p) & 0x8000) ? CharAnim_Right : CharAnim_Left);
				this->pico_p++;
			}
		}
	}
	else
	{
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			//Perform dance
			if ((stage.song_step % stage.gf_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_Left)
					character->set_anim(character, CharAnim_Right);
				else
					character->set_anim(character, CharAnim_Left);
				
				//Bump speakers
			}
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GF_SetFrame);
	Character_Draw(character, &this->tex, &char_gf_frame[this->frame]);
	
	//Tick speakers
}

void Char_GF_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GF_Free(Character *character)
{
	Char_GF *this = (Char_GF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GF_New(fixed_t x, fixed_t y)
{
	//Allocate gf object
	Char_GF *this = Mem_Alloc(sizeof(Char_GF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GF_New] Failed to allocate gf object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GF_Tick;
	this->character.set_anim = Char_GF_SetAnim;
	this->character.free = Char_GF_Free;
	
	Animatable_Init(&this->character.animatable, char_gf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(16,1);
	this->character.focus_y = FIXED_DEC(-50,1);
	this->character.focus_zoom = FIXED_DEC(13,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GF.ARC;1");
	
	const char **pathp = (const char *[]){
		"left0.tim",  //GF_ArcMain_Left0
		"left1.tim",  //GF_ArcMain_Left1
		"left2.tim",  //GF_ArcMain_Left2
		"left3.tim",  //GF_ArcMain_Left3
		"left4.tim",  //GF_ArcMain_Left4
		"left5.tim",  //GF_ArcMain_Left5
		"right0.tim", //GF_ArcMain_Right0
		"right1.tim", //GF_ArcMain_Right1
		"right2.tim", //GF_ArcMain_Right2
		"right3.tim", //GF_ArcMain_Right3
		"right4.tim", //GF_ArcMain_Right4
		"right5.tim", //GF_ArcMain_Right5
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize speaker
	
	//Initialize Pico test
	if (stage.stage_id == StageId_7_3 && stage.back != NULL)
		this->pico_p = ((Back_Week7*)stage.back)->pico_chart;
	else
		this->pico_p = NULL;
	
	return (Character*)this;
}

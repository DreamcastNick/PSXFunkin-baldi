/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week6.h"

#include "../mem.h"
#include "../archive.h"

//Week 6 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background
	Gfx_Tex tex_back1; //Window
	Gfx_Tex tex_back2; //Lightning window
} Back_Week6;

//Week 6 background functions
void Back_Week6_DrawBG(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	//Draw background
	RECT back_src = {0, 0, 256, 256};
	RECT_FIXED back_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(367,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
	
	#if SCREEN_WIDTH > 320
		RECT backl_src = {0, 0, 1, 256};
		RECT_FIXED backl_dst = {
			FIXED_DEC(-320,1) - FIXED_DEC(SCREEN_WIDEADD,2) - fx,
			FIXED_DEC(-150,1) - fy,
			FIXED_DEC(SCREEN_WIDEADD,2),
			FIXED_DEC(267,1)
		};
		RECT backr_src = {255, 0, 0, 256};
		RECT_FIXED backr_dst = {
			FIXED_DEC(168,1) - fx,
			FIXED_DEC(-125,1) - fy,
			FIXED_DEC(SCREEN_WIDEADD,2),
			FIXED_DEC(267,1)
		};
		
		Stage_DrawTex(&this->tex_back0, &backl_src, &backl_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back0, &backr_src, &backr_dst, stage.camera.bzoom);
	#endif
}

void Back_Week6_DrawFG(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	//Draw window
	RECT window_src = {0, 0, 256, 256};
	RECT_FIXED window_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(367,1)
	};
	Stage_DrawTex(&this->tex_back1, &window_src, &window_dst, stage.camera.bzoom);
}

void Back_Week6_Free(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week6_New(void)
{
	//Allocate background structure
	Back_Week6 *this = (Back_Week6*)Mem_Alloc(sizeof(Back_Week6));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week6_DrawBG;
	this->back.free = Back_Week6_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK6\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}

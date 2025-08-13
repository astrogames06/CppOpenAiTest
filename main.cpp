#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "gpt.hpp"

const int WIDTH = 850;
const int HEIGHT = 450;

void UpdateDrawFrame();

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");

	#if defined(PLATFORM_WEB)
    	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
	#else
		SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
		//--------------------------------------------------------------------------------------

		// Main game loop
		while (!WindowShouldClose())    // Detect window close button or ESC key
		{
			UpdateDrawFrame();
		}
	#endif

	CloseWindow();

	return 0;
}

bool text_box = false;
std::string text_box_str;
void UpdateDrawFrame()
{
	BeginDrawing();
	
	ClearBackground(RAYWHITE);

	DrawText("Ask AI", 100, 100, 20, BLACK);

	std::string gpt_d = "GPT: " + gpt_str;
    DrawText(gpt_d.c_str(), 100, 130, 20, BLACK);

	if (GuiTextBox({100, 155, 100, 45}, &text_box_str[0], 20, text_box)) text_box = !text_box;
    if (GuiButton({100, 200, 100, 45}, "SEND!"))
	{
		ask_gpt(text_box_str.c_str());
	}

	EndDrawing();
}
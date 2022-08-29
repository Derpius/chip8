#include "raylib.h"
#include "rlgl.h"

#include "VM.h"

#include <fstream>

int main()
{
	const int WIDTH = 1280, HEIGHT = 720;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(WIDTH, HEIGHT, "Chip8 Emulator");
	HideCursor();
	SetTargetFPS(60);

	RenderTexture2D chip8Display = LoadRenderTexture(64, 32);

	uint32_t programSize;
	uint8_t* program = LoadFileData("D:\\Programming\\GitHub\\chip8\\test_opcode.ch8", &programSize);

	Chip8 vm = Chip8();
	vm.LoadProgram(program, programSize);
	UnloadFileData(program);

	Shader shader = LoadShader(0, "shaders/main.fs.glsl");
	int screenLoc = GetShaderLocation(shader, "screen");
	int resolutionLoc = GetShaderLocation(shader, "resolution");
	int curvatureLoc = GetShaderLocation(shader, "curvature");
	int crtBorderColourLoc = GetShaderLocation(shader, "crtBorderColour");

	float resolution[2] = { WIDTH, HEIGHT };
	SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

	float curvature = 4.f;
	SetShaderValue(shader, curvatureLoc, &curvature, SHADER_UNIFORM_FLOAT);

	float crtBorderColour[3] = { 0.1, 0.1, 0.1 };
	SetShaderValue(shader, crtBorderColourLoc, crtBorderColour, SHADER_UNIFORM_VEC3);

	while (!WindowShouldClose()) {
		vm.Tick(GetFrameTime());
		const uint8_t* vram = vm.GetVRAM();

		BeginTextureMode(chip8Display);
		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 64; x++) {
				uint8_t pixel = vram[y * 64 + x];
				DrawPixel(x, y, Color{
					static_cast<uint8_t>(pixel > 0 ? 255 : 0),
					static_cast<uint8_t>(pixel > 0 ? 255 : 0),
					static_cast<uint8_t>(pixel > 0 ? 255 : 0),
					255,
				});
			}
		}
		EndTextureMode();

		BeginDrawing();
			BeginShaderMode(shader);
			SetShaderValueTexture(shader, screenLoc, chip8Display.texture);
				//DrawTexturePro(chip8Display.texture, Rectangle{ 0, 0, 64, 32 }, Rectangle{ 0, 0, WIDTH, HEIGHT }, Vector2{ 0, 0 }, 0.f, WHITE);
				DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
			EndShaderMode();
		EndDrawing();
	}

	UnloadRenderTexture(chip8Display);
	UnloadShader(shader);
	CloseWindow();

	return 0;
}

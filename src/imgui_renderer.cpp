#include "imgui_renderer.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "imgui_memory_editor.h"
#include "mbc/cart.h"
#include "gb_system.h"
#include "cpu.h"
#include "ppu.h"
#include "renderer.h"

static SDL_Texture* screen_tex;

void process_imgui_event(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void init_imgui(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture)
{
	screen_tex = texture;

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
}

static bool info_window = 1;
static bool memory_editor = 1;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void render_imgui(SDL_Renderer* debug_renderer)
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	static float width = 160;
	static float height = 144;
	ImGui::Begin("Screen");
	ImGui::Text("Size = %i x %i", width, height);
	ImGui::Image((void*)(intptr_t)screen_tex, ImVec2(width, height));
	ImGui::End();

	ImGui::Begin("System");
	ImGui::Text("Title: %s", cart.title);
	ImGui::Text("Cart Type: MBC%i", cart.cart_type);
	ImGui::Text("Frame Rate: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();

	static MemoryEditor memory_editor;
	static u8* data;
	static size_t size;
	static int e = 0;
	switch (e)
	{
		case 0:
			data = cart.rom;
			size = cart.rom_size;
			break;
		case 1:
			data = cart.sram;
			size = cart.ram_size;
			break;
		case 2:
			data = gb.wram;
			size = 0x2000;
			break;
		case 3:
			data = ppu.vram;
			size = 0x2000;
			break;
	}
	ImGui::Begin("Memory Editor"); 
	ImGui::RadioButton("ROM", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("SRAM", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("WRAM", &e, 2); ImGui::SameLine();
	ImGui::RadioButton("VRAM", &e, 3);
	memory_editor.DrawContents(data, size);
	ImGui::End();

	ImGui::Begin("Debugger");
	ImGui::Text("z: %d n: %d h: %d c: %d", cpu.r.ZF, cpu.r.NF, cpu.r.HF, cpu.r.CF);
	ImGui::Text("af = %.4X", cpu.r.AF); ImGui::SameLine();
	ImGui::Text("lcdc = %.4X", ppu.lcdc);
	ImGui::Text("bc = %.4X", cpu.r.BC); ImGui::SameLine();
	ImGui::Text("stat = %.4X", ppu.stat);
	ImGui::Text("de = %.4X", cpu.r.DE); ImGui::SameLine();
	ImGui::Text("ly   = %.4X", ppu.ly);
	ImGui::Text("hl = %.4X", cpu.r.HL);
	ImGui::Text("sp = %.4X", cpu.SP);
	ImGui::Text("pc = %.4X", cpu.PC);
	ImGui::Text("cycles = %i", gb.cycles);
	ImGui::End();

	ImGui::Begin("Run");
	if (ImGui::Button("Cap Framerate (1)"))
		cap_framerate = !cap_framerate;
	if (ImGui::Button("Pause"))
		cap_framerate = !cap_framerate;
	if (ImGui::Button("Step"))
		cap_framerate = !cap_framerate;
	ImGui::End();

	ImGui::Render();
	SDL_SetRenderDrawColor(debug_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	SDL_RenderClear(debug_renderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(debug_renderer);
}

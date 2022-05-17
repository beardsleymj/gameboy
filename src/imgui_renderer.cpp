#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui_renderer.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"
#include "imgui_memory_editor.h"
#include "mbc/cart.h"
#include "gb_system.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "renderer.h"
#include "event.h"
#include "SDL_opengl.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

static SDL_Window* window;
static SDL_GLContext context;
static GLuint texture;
static ImGuiIO* io;
static ImGuiViewport* main_viewport;
static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);


void process_imgui_event(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void init_imgui(SDL_Window* win, SDL_GLContext gl_context, GLuint gl_texture)
{
	context = gl_context;
	texture = gl_texture;

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	 //When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	window = win;
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(NULL);
	main_viewport = ImGui::GetMainViewport();
}

static bool info_window = 1;
static bool memory_editor = 1;

static bool show_system = 0;
static bool show_memory_editor = 0;
static bool show_debugger = 0;



void render_imgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Render Game
	ImGui::GetBackgroundDrawList(main_viewport)->AddImage((void*)(intptr_t)texture, main_viewport->Pos, main_viewport->Pos + ImVec2(main_viewport->Size.y * (1.1111111), main_viewport->Size.y));

	if (SDL_GetMouseFocus())	draw_menu_bar();
	if (show_system)			draw_system();
	if (show_memory_editor)		draw_memory_editor();
	if (show_debugger)			draw_debugger();

	ImGui::EndFrame();
	ImGui::Render();
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
	SDL_GL_SwapWindow(window);
}

void draw_menu_bar()
{
	if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save"))
				{
					cart_write_save();
				}
				if (ImGui::MenuItem("Quit"))
				{
					quit = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Run"))
			{
				if (ImGui::MenuItem("Cap Framerate"))
				{
					cap_framerate = !cap_framerate;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::BeginMenu("Size"))
				{
					if (ImGui::MenuItem("1x"))
					{
						SDL_SetWindowSize(window, 160, 144);
					}
					if (ImGui::MenuItem("2x"))
					{
						SDL_SetWindowSize(window, 160 * 2, 144 * 2);
					}
					if (ImGui::MenuItem("3x"))
					{
						SDL_SetWindowSize(window, 160 * 3, 144 * 3);
					}
					if (ImGui::MenuItem("4x"))
					{
						SDL_SetWindowSize(window, 160 * 4, 144 * 4);
					}
					if (ImGui::MenuItem("5x"))
					{
						SDL_SetWindowSize(window, 160 * 5, 144 * 5);
					}
					if (ImGui::MenuItem("6x"))
					{
						SDL_SetWindowSize(window, 160 * 6, 144 * 6);
					}
					if (ImGui::MenuItem("7x"))
					{
						SDL_SetWindowSize(window, 160 * 7, 144 * 7);
					}
					if (ImGui::MenuItem("8x"))
					{
						SDL_SetWindowSize(window, 160 * 8, 144 * 8);
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("System"))
				{
					show_system = !show_system;
				}
				if (ImGui::MenuItem("Debugger"))
				{
					show_debugger = !show_debugger;
				}
				if (ImGui::MenuItem("Memory"))
				{
					show_memory_editor = !show_memory_editor;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
	}
}

void draw_system()
{
	ImGui::Begin("System", &show_system);
	ImGui::Text("Title: %s", cart.title);
	ImGui::Text("Cart Type: MBC%i", cart.cart_type);
	ImGui::Text("CGB Flag: %i", cart.cgb_flag);
	ImGui::Text("Frame Rate: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void draw_debugger()
{
	ImGui::Begin("Debugger", &show_debugger);
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
}

void draw_memory_editor()
{
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
			data = cpu.wram;
			size = 0x2000;
			break;
		case 3:
			data = ppu.vram;
			size = 0x2000;
			break;
		case 4:
			data = (u8*)apu.samples.buffer;
	}
	ImGui::Begin("Memory Editor", &show_memory_editor);
	ImGui::RadioButton("ROM", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("SRAM", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("WRAM", &e, 2); ImGui::SameLine();
	ImGui::RadioButton("VRAM", &e, 3); ImGui::SameLine();
	ImGui::RadioButton("APU Samples BUffer", &e, 4);
	memory_editor.DrawContents(data, size);
	ImGui::End();
}
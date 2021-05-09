#include "renderer.h"

static SDL_Window* window;
static uint16_t window_width;
static uint16_t window_height;
static SDL_Renderer* renderer;
static SDL_Event event;
static uint8_t scale;
static GLubyte* image; // RGB Storage
static GLuint m_tex; // texture map
static GLuint fbo; // frame buffer object

void renderer_init() {
    scale = 4;
    image = calloc((160 * scale) * (144 * scale) * 3, 1);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 1);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 1);
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);;
    window = SDL_CreateWindow("GameBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * scale, 144 * scale, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewInit();
   
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 160 * scale, 144 * scale, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    // Attach the texture to the fbo
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);

    // Restore window framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void render(gameboy* gb)
{
    gb->draw_frame = false;
    fill_image(gb);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, m_tex); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160 * scale, 144 * scale, 0, GL_RGB, GL_UNSIGNED_BYTE, /*gb->screen*/ image);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBlitFramebuffer(0, 0, 160 * scale, 144 * scale, 0, 0, 160 * scale, 144 * scale, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    SDL_GL_SwapWindow(window);

        GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("error: %s ", glewGetErrorString(err));
    }
}

void fill_image(gameboy* gb) {

    for (int i = 0; i < (144 * scale); i++) {
        for (int j = 0; j < (160 * scale * 3); j += 3) {
            image[(i * 160 * scale * 3) + j] = gb->screen[i / scale][(j / 3) / scale][0];
            image[(i * 160 * scale * 3) + j + 1] = gb->screen[i / scale][(j / 3) / scale][1];
            image[(i * 160 * scale * 3) + j + 2] = gb->screen[i / scale][(j / 3) / scale][2];
        }
    }
}
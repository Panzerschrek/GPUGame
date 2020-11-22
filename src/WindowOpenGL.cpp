#include "WindowOpenGL.hpp"
#include "Assert.hpp"
#include "Log.hpp"
#include "SystemWindow.hpp"
#include <SDL_opengl.h>
#include <algorithm>
#include <cstring>


namespace GPUGame
{


WindowOpenGL::WindowOpenGL(const SystemWindow& system_window)
	: sdl_window_(system_window.GetSDLWindow())
{

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

	#ifdef DEBUG
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
	#endif

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );

	gl_context_= SDL_GL_CreateContext(system_window.GetSDLWindow());
	if(gl_context_ == nullptr)
		Log::FatalError("Could not create OpenGL context");

	Log::Info("");
	Log::Info( "OpenGL configuration: " );
	Log::Info( "Vendor: ", glGetString( GL_VENDOR ) );
	Log::Info( "Renderer: ", glGetString( GL_RENDERER ) );
	Log::Info( "Version: ", glGetString( GL_VERSION ) );
	Log::Info("");
}

WindowOpenGL::~WindowOpenGL()
{
	SDL_GL_DeleteContext(gl_context_);
}

void WindowOpenGL::BeginFrame()
{
	glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void WindowOpenGL::EndFrame()
{
	SDL_GL_SwapWindow(sdl_window_);
}

} // namespace GPUGame

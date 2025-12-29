//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_image.h"
#include "SDL_syswm.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "Timer.h"
#include "Renderer.h"
#if defined(_DEBUG)
	#include "LeakDetector.h"
#endif

using namespace dae;

void DisplayControls()
{
	std::cout << "Controls: \n";
	std::cout << "F1: Switch Renderer (Hardware/Software)\n";
	std::cout << "F2: Toggle Rotation\n";
	std::cout << "F4: Toggle Texture Sampling State (Point/Linear/Anisotropic)\n";
	std::cout << "F5: Cycle Shading Mode (Observed Area/Diffuse/Specular/Combined)\n";
	std::cout << "F6: Toggle Normal Mapping\n";
	std::cout << "F7: Toggle Depth Buffer Visualization\n";
}

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	// Leak detection
	#if defined(_DEBUG)
		LeakDetector detector{};
	#endif

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - ***Nils Hammond/2DAE10***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	DisplayControls();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				//Test for a key
				//if (e.key.keysym.scancode == SDL_SCANCODE_X)
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
					pRenderer->SwapRenderer();
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
					pRenderer->ToggleRotation();
				//if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				//	pRenderer->ToggleFire();
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
					pRenderer->CycleSamplerState();
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
					pRenderer->CycleShadingMode();
				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
					pRenderer->ToggleNormalMap();
				if (e.key.keysym.scancode == SDL_SCANCODE_F7)
					pRenderer->ToggleDepthBufferVisualization();
				//if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				//	pRenderer->ToggleBoundingBoxVisualization();
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}
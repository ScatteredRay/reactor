#include <SDL.h>
#include <assert.h>

#include "core_systems.h"

void init_sdl_system()
{
	SDL_Init(SDL_INIT_JOYSTICK);
}

void finalize_sdl_system()
{
	SDL_Quit();
}

#include <SDL.h>
#include <assert.h>

struct tick_system
{
	unsigned int TickCount;
	unsigned int TickAccum;
};

tick_system* init_tick_system()
{
	tick_system* Ticker = new tick_system();
	Ticker->TickCount = SDL_GetTicks();
	Ticker->TickAccum = 0;

	return Ticker;
}

void destroy_tick_system(tick_system* system)
{
	delete system;
}

void tick_system_tick(tick_system* system)
{
	int NewTick = SDL_GetTicks();
	system->TickAccum += NewTick - system->TickCount;
	
	system->TickCount = NewTick;

	float DeltaTime = 0.0f; // Box2D assumes fixed timestep, lock it?

	if(system->TickAccum >= 20)
		DeltaTime = 0.02f;

	while(system->TickAccum >= 20)
	{
		// Run All Ticks
		//UpdateInput(LocalPlayerInput);
		//UpdatePlayerInput(MainPlayer, LocalPlayerInput);
		//UpdatePlayer(DeltaTime, MainPlayer);
		//PhysicsWorld->Step(DeltaTime, 10);
		system->TickAccum -= 20;
	}
}

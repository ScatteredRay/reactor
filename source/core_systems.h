#ifndef _CORE_SYSTEMS_H_
#define _CORE_SYSTEMS_H_

void init_sdl_system();
void finalize_sdl_system();

struct tick_system;

tick_system* init_tick_system();
void destroy_tick_system(tick_system* system);
void tick_system_tick(tick_system* system);

#endif //_CORE_SYSTEMS_H_

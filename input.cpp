#include "input.h"
//#include <IOKit/hid/IOHIDLib.h>
#include <SDL.h>
#include <list>

using namespace std;

PlayerInput* LocalPlayerInput;

struct InputSymIndexMap
{
	const char* Name;
	int Index;
};

const InputSymIndexMap InputNameMap[] =
{{"jump", Input_Jump},
 {"move-x", Input_Move_X},
 {"move-y", Input_Move_Y},
 {NULL, 0}};

int IndexFromSymName(const char* Symbol)
{
	const InputSymIndexMap* SymMap = InputNameMap;
	while(SymMap->Name != NULL)
	{
		if(strcmp(SymMap->Name, Symbol) == 0)
			return SymMap->Index;
		SymMap++;
	}
}

struct PlayerInput
{
//	IOHIDManagerRef HIDManager;
	SDL_Joystick* Joy;
	list<pair<AxisInputs, pair<int, bool> > > AxisMap;
	list<pair<ButtonInputs, int> > ButtonMap;
	float AxisValues[Input_Max_Axis];
	bool ButtonValues[Input_Max_Button];
};

void InitSchemeInput();

PlayerInput* InitPlayerInput()
{
	PlayerInput* In = new PlayerInput();
	In->Joy = NULL;

	unsigned int num_sticks = SDL_NumJoysticks();
    for(unsigned int i = 0; i<num_sticks; i++)
	{
		In->Joy = SDL_JoystickOpen(i);
		printf("-----------------------\n");
		printf("Joystick: %s\n", SDL_JoystickName(i));
		printf("Axes:%d Buttons:%d Balls:%d\n",
			 SDL_JoystickNumAxes(In->Joy),
			 SDL_JoystickNumButtons(In->Joy),
			 SDL_JoystickNumBalls(In->Joy));
		//InitSchemeInput();
		//scheme_set_external_data(scheme_vm, In);
		//scheme_load_file_name(scheme_vm, "input.scm");
		//scheme_set_external_data(scheme_vm, NULL);

		for(list<pair<AxisInputs, pair<int, bool> > >::iterator p = In->AxisMap.begin(); p != In->AxisMap.end(); p++)
		{
			printf("AxisMap, Input:%d Index:%d\n", (*p).first, (*p).second.first);
		}

		for(list<pair<ButtonInputs, int> >::iterator p = In->ButtonMap.begin(); p != In->ButtonMap.end(); p++)
		{
			printf("ButtonMap, Input:%d Index:%d\n", (*p).first, (*p).second);
		}
	}

	return In;

	/*In->HIDManager = IOHIDManagerCreate(kCFAllocatorDefault, 0);

	CFDictionaryRef MatchingDictRef = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
	CFDictionarySetValue(MatchingDictRef, kIOHIDProductIDKey, &0x268);
	IOHIDManagerSetDeviceMatching(In->HIDManager, NULL);

	CFRelease(MatchingDictRef);*/
	
}

void DestroyPlayerInput(PlayerInput* Input)
{
	//IOHIDManagerClose(Input->HIDManager);
	//CFRelease(In->HIDManager);

	if(Input->Joy)
		SDL_JoystickClose(Input->Joy);
	
	delete Input;
}

void UpdateInput(PlayerInput* Input)
{
	if(!Input->Joy)
		return;
	SDL_JoystickUpdate();

	for(list<pair<AxisInputs, pair<int, bool> > >::iterator p = Input->AxisMap.begin(); p != Input->AxisMap.end(); p++)
	{
		Input->AxisValues[(*p).first] = SDL_JoystickGetAxis(Input->Joy, (*p).second.first)/32767.0f * ((*p).second.second ? -1.0f : 1.0f);
	}

	for(list<pair<ButtonInputs, int> >::iterator p = Input->ButtonMap.begin(); p != Input->ButtonMap.end(); p++)
	{
		Input->ButtonValues[(*p).first] = SDL_JoystickGetButton(Input->Joy, (*p).second);
	}
	printf("Move Input X: %f Y: %f Jump: %d\n", Input->AxisValues[Input_Move_X], Input->AxisValues[Input_Move_Y], Input->ButtonValues[Input_Jump]);
}

bool GetButtonState(PlayerInput* Input, ButtonInputs Btn)
{
	if(!Input->Joy)
		return false;
	return Input->ButtonValues[Btn];
}

float GetAxisState(PlayerInput* Input, AxisInputs Axis)
{
	if(!Input->Joy)
		return 0.0f;
	return Input->AxisValues[Axis];
}

static const char* InputMapDocs =
"(input-map \"Sets up the input mapping for a joystick.\" (nil) (string \"Controller ID\" ((symbol \"Input Dest\") (list \"Input Source\")))";

/*pointer InputMap(scheme* sc, pointer p)
{
	PlayerInput* Input = (PlayerInput*)sc->ext_data;
	const char* InputDev = string_value(pair_car(p));

	if(strcmp(SDL_JoystickName(SDL_JoystickIndex(Input->Joy)), InputDev) == 0)
	{
		Input->AxisMap.clear();
		Input->ButtonMap.clear();
		pointer rest = pair_cdr(p);
		while(rest != sc->NIL)
		{
			pointer map = pair_car(rest);
			if(strcmp(symname(car(map)), "axis-map") == 0)
			{
				bool bInvert = false;
				if(strcmp(symname(cadr(map)), "invert") == 0)
				{
					bInvert = true;
					// Invert Input
					map = cdr(map);
				}
				Input->AxisMap.push_back(pair<AxisInputs, pair<int, bool> >((AxisInputs)IndexFromSymName(symname(cadr(map))), pair<int, bool>(ivalue(caddr(map)), bInvert)));
			}
			else if(strcmp(symname(car(map)), "button-map") == 0)
			{
				Input->ButtonMap.push_back(pair<ButtonInputs, int>((ButtonInputs)IndexFromSymName(symname(cadr(map))), ivalue(caddr(map))));
			}
			rest = pair_cdr(rest);
		}
	}
}

void InitSchemeInput()
{
	scheme_define(scheme_vm, scheme_vm->global_env, mk_symbol(scheme_vm, "input-map"), mk_foreign_func(scheme_vm, &InputMap));
}*/

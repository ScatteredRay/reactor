#include "input.h"
//#include <IOKit/hid/IOHIDLib.h>
#include <SDL.h>
#include <list>

using namespace std;

struct InputMapping
{
    InputType type;
    int input_map;
    int joy_index;
    bool invert;
};

// Playstation 3 controller input map.
const InputMapping DefaultInputMap[] =
{{Input_Type_Axis, Input_Move_X, 0, false},
 {Input_Type_Axis, Input_Move_Y, 1, true},
 {Input_Type_Button, Input_Jump, 14, false},
 {Input_Type_End, 0, 0, false}};

struct PlayerInput
{
//	IOHIDManagerRef HIDManager;
	SDL_Joystick* Joy;
	list<pair<AxisInputs, pair<int, bool> > > AxisMap;
	list<pair<ButtonInputs, int> > ButtonMap;
	float AxisValues[Input_Max_Axis];
	bool ButtonValues[Input_Max_Button];
};

void InitPlayerInputMapping(PlayerInput* In)
{
    const InputMapping* InputMap = DefaultInputMap;
    In->AxisMap.clear();
    In->ButtonMap.clear();
    
    while(InputMap->type != Input_Type_End)
    {
        if(InputMap->type == Input_Type_Axis)
        {
            In->AxisMap.push_back(
                pair<AxisInputs, pair<int, bool> >(
                    (AxisInputs)InputMap->input_map,
                    pair<int, bool>(
                        InputMap->joy_index,
                        InputMap->invert)));
        }
        else
        {
            In->ButtonMap.push_back(
                pair<ButtonInputs, int>(
                    (ButtonInputs)InputMap->input_map,
                    InputMap->joy_index));
        }
        InputMap++;
    }
}

unsigned int InitPlayerInputs(PlayerInput** Inputs, unsigned int DesiredInputs)
{
	unsigned int num_sticks = SDL_NumJoysticks();
    for(unsigned int i = 0; i<DesiredInputs; i++)
	{
        if(i >= num_sticks)
        {
            Inputs[i] = NULL;
            continue;
        }

        PlayerInput* In = new PlayerInput();
        In->Joy = NULL;
		In->Joy = SDL_JoystickOpen(i);
		printf("-----------------------\n");
		printf("Joystick: %s\n", SDL_JoystickName(i));
		printf("Axes:%d Buttons:%d Balls:%d\n",
			 SDL_JoystickNumAxes(In->Joy),
			 SDL_JoystickNumButtons(In->Joy),
			 SDL_JoystickNumBalls(In->Joy));

        InitPlayerInputMapping(In);

		for(list<pair<AxisInputs, pair<int, bool> > >::iterator p = In->AxisMap.begin(); p != In->AxisMap.end(); p++)
		{
			printf("AxisMap, Input:%d Index:%d\n", (*p).first, (*p).second.first);
		}

		for(list<pair<ButtonInputs, int> >::iterator p = In->ButtonMap.begin(); p != In->ButtonMap.end(); p++)
		{
			printf("ButtonMap, Input:%d Index:%d\n", (*p).first, (*p).second);
		}
        Inputs[i] = In;
	}

    return num_sticks;

}

void DestroyPlayerInputs(PlayerInput** Input, unsigned int Inputs)
{
    for(unsigned int i=0; i<Inputs; i++)
    {
        if(!Input[i])
            continue;

        if(Input[i]->Joy)
            SDL_JoystickClose(Input[i]->Joy);
	
        delete Input[i];
        Input[i]  = NULL;
    }
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
	//printf("Move Input X: %f Y: %f Jump: %d\n", Input->AxisValues[Input_Move_X], Input->AxisValues[Input_Move_Y], Input->ButtonValues[Input_Jump]);
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

InputHandler* InitInputHandler()
{
    return null;
}

void DestroyInputHandler(InputHandler* input) {}

void InputReceiveKey(InputHandler* input, int code, KeyStatus status) {}

void InputMouseEvent(InputHandler* input, int x, int y, MouseButton button, MouseStatus status) {}

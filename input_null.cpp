#include "input.h"

struct PlayerInput
{
};

unsigned int InitPlayerInputs(PlayerInput** Inputs, unsigned int DesiredInputs)
{
    for(unsigned int i = 0; i<DesiredInputs; i++)
	{
        PlayerInput* In = new PlayerInput();
        Inputs[i] = In;
	}

    return DesiredInputs;
}

void DestroyPlayerInputs(PlayerInput** Input, unsigned int Inputs)
{
    for(unsigned int i=0; i<Inputs; i++)
    {
        if(!Input[i])
            continue;

        delete Input[i];
        Input[i] = 0;
    }
}

void UpdateInput(PlayerInput* Input)
{
}

bool GetButtonState(PlayerInput* Input, ButtonInputs Btn)
{
    return false;
}

float GetAxisState(PlayerInput* Input, AxisInputs Axis)
{
    return 0.0f;
}
InputHandler* InitInputHandler()
{
    return null;
}

void DestroyInputHandler(InputHandler* input) {}

void InputReceiveKey(InputHandler* input, int code, KeyStatus status) {}

void InputMouseEvent(InputHandler* input, int x, int y, MouseButton button, MouseStatus status) {}

// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "input.h"
#include "system.h"
#include <assert.h>

// TODO: I've really made a bit of a mess out of input, between duplicated code, duplicated functionality, platforms all working differentlly. Simplify and unify this!

struct InputMapping
{
    InputType type;
    int input_map;
    int key_code;
    int axis_other_key_code;
    bool invert;

    InputMapping() {}

    InputMapping(InputType _t, int _i, int kc, int a_kc, bool i) :
        type(_t),
        input_map(_i),
        key_code(kc),
        axis_other_key_code(a_kc),
        invert(i) {}
};

struct PlayerInput
{
    float AxisValues[Input_Max_Axis];
    bool ButtonValues[Input_Max_Button];
    InputMapping* mappings;
};

struct InputHandler
{
    KeyStatus key_map[MAX_KEYCODES];
};

InputHandler* Input_Handler = 0;

unsigned int InitPlayerInputs(PlayerInput** Inputs, unsigned int DesiredInputs)
{
    InputMapping* mappings = new InputMapping[4];
    unsigned int m = 0;
    mappings[m++] = InputMapping(Input_Type_Axis,
                                 Input_Move_X,
                                 GetKeyCodeFromAscii('d'),
                                 GetKeyCodeFromAscii('a'),
                                 false);
    mappings[m++] = InputMapping(Input_Type_Axis,
                                 Input_Move_Y,
                                 GetKeyCodeFromAscii('w'),
                                 GetKeyCodeFromAscii('s'),
                                 false);
    mappings[m++] = InputMapping(Input_Type_Button,
                                 Input_Jump,
                                 GetKeyCodeFromAscii('c'),
                                 0,
                                 false);
    mappings[m++] = InputMapping(Input_Type_End, 0, 0, 0, false);

    for(unsigned int i = 0; i<DesiredInputs; i++)
    {
        PlayerInput* In = new PlayerInput();

        In->mappings = mappings;

        for(unsigned int a = 0; a < Input_Max_Axis; a++)
        {
            In->AxisValues[a] = 0.0f;
        }

        for(unsigned int b = 0; b < Input_Max_Button; b++)
        {
            In->ButtonValues[b] = false;
        }
        Inputs[i] = In;
    }

    return DesiredInputs;
}

void DestroyPlayerInputs(PlayerInput** Input, unsigned int Inputs)
{
    if(Inputs > 0)
    {
        delete[] Input[0]->mappings;
    }

    for(unsigned int i=0; i<Inputs; i++)
    {
        if(!Input[i])
            continue;

        delete Input[i];
        Input[i] = 0;
    }
}

void UpdateInput(PlayerInput* Input, InputHandler* handler)
{
    InputMapping* mapping = Input->mappings;

    while(mapping->type != Input_Type_End) {
        switch(mapping->type) {
        case Input_Type_Axis:
            assert(mapping->key_code < MAX_KEYCODES);
            assert(mapping->axis_other_key_code < MAX_KEYCODES);
            assert(mapping->input_map < Input_Max_Axis);
            Input->AxisValues[mapping->input_map] =
                ((handler->key_map[mapping->key_code] == Key_Down) ? 1.0f : 0.0f) -
                ((handler->key_map[mapping->axis_other_key_code] == Key_Down) ? 1.0f : 0.0f);
            break;
        case Input_Type_Button:
            assert(mapping->key_code < MAX_KEYCODES);
            assert(mapping->input_map < Input_Max_Button);
            Input->ButtonValues[mapping->input_map] =
                (handler->key_map[mapping->key_code] == Key_Down);
            break;
        }
        mapping++;
    }
}


bool GetButtonState(PlayerInput* Input, ButtonInputs Btn)
{
    return Input->ButtonValues[Btn];
}

float GetAxisState(PlayerInput* Input, AxisInputs Axis)
{
    return Input->AxisValues[Axis];
}

InputHandler* InitInputHandler()
{
    assert(Input_Handler == 0 && "InputHandler currentlly exists as a singleton.");

    Input_Handler = new InputHandler();

    return Input_Handler;
}

void DestroyInputHandler(InputHandler* input)
{
    assert(input == Input_Handler);
    Input_Handler = 0;
    delete input;
}

void InputReceiveKey(InputHandler* input, int code, KeyStatus status)
{
    assert(code < MAX_KEYCODES);
    input->key_map[code] = status;
}

void InputMouseEvent(InputHandler* input, int x, int y, MouseButton button, MouseStatus status)
{
}

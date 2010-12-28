#ifndef _INPUT_H_
#define _INPUT_H_

enum AxisInputs
{
	Input_Move_X = 0,
	Input_Move_Y = 1,
	Input_Max_Axis = 2
};

enum ButtonInputs
{
	Input_Jump = 0,
	Input_Max_Button = 1
};

enum InputType 
{
    Input_Type_Axis,
    Input_Type_Button,
    Input_Type_End
};

struct PlayerInput;

unsigned int InitPlayerInputs(PlayerInput** Inputs, unsigned int DesiredInputs);
void DestroyPlayerInputs(PlayerInput** Input, unsigned int Inputs);
void UpdateInput(PlayerInput* Input);
bool GetButtonState(PlayerInput* Input, ButtonInputs Btn);
float GetAxisState(PlayerInput* Input, AxisInputs Axis);

#endif //_INPUT_H_

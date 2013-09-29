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
struct InputHandler;

unsigned int InitPlayerInputs(PlayerInput** Inputs, unsigned int DesiredInputs);
void DestroyPlayerInputs(PlayerInput** Input, unsigned int Inputs);
void UpdateInput(PlayerInput* Input, InputHandler* handler);
bool GetButtonState(PlayerInput* Input, ButtonInputs Btn);
float GetAxisState(PlayerInput* Input, AxisInputs Axis);

struct InputHandler;

enum KeyStatus
{
    Key_Up,
    Key_Down
};

enum MouseStatus
{
    Mouse_Down,
    Mouse_Up,
    Mouse_Move
};

enum MouseButton
{
    No_Button = 0,
    Left_Button = 1 << 0,
    Right_Button = 1 << 1,
    Mid_Button = 1 << 2
};

InputHandler* InitInputHandler();
void DestroyInputHandler(InputHandler* input);
void InputReceiveKey(InputHandler*, int code, KeyStatus status);
void InputMouseEvent(InputHandler*, int x, int y, MouseButton button, MouseStatus status);

#endif //_INPUT_H_

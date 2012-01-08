// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _VIEW_DECL_H_
#define _VIEW_DECL_H_

// This is a simple set of declerations used to render views, this is the
// interface between the system libraries back into the view.

struct ViewInfo;

ViewInfo* InitView();
// Standard destruction function named because it's up to view to determine
// if it actually destroys anything.
void FinishView(ViewInfo*);
void ResizeView(ViewInfo*, int width, int height);
void UpdateView(ViewInfo*);

struct InputHandler;

enum KeyStatus
{
    Key_Down,
    Key_Up
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

InputHandler* GetInputHandler(ViewInfo*);
void InputReceiveKey(InputHandler*, int code, KeyStatus status);
void InputMouseEvent(InputHandler*, int x, int y, MouseButton button, MouseStatus status);

#endif //_VIEW_DECL_H_

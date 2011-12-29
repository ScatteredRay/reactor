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

InputHandler* GetInputHandler(ViewInfo*);
void ReceiveKeyInput(InputHandler*, int code, KeyStatus status);

#endif //_VIEW_DECL_H_

#ifndef _DEFERRED_H_
#define _DEFERRED_H_

#include "render_target.h" // Temp, remove me.

struct DeferredRender;

DeferredRender* InitDeferred(struct Environment* e);
void DestroyDeferred(DeferredRender* r);
void RenderDeferred(DeferredRender* r, RenderTarget* t, Environment* env);

#endif //_DEFERRED_H_

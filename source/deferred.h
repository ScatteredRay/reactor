#ifndef _DEFERRED_H_
#define _DEFERRED_H_


struct RenderTarget;
RenderTarget* InitRenderTarget(unsigned int width, unsigned int height);
void DestroyRenderTarget(RenderTarget* t);
void ResizeRenderTarget(RenderTarget* t, unsigned int width, unsigned int height);
void BindRenderTarget(RenderTarget* t);
void UnbindRenderTarget(RenderTarget* t);

struct DeferredRender;

DeferredRender* InitDeferred(struct Environment* e);
void DestroyDeferred(DeferredRender* r);
void RenderDeferred(DeferredRender* r, RenderTarget* t, Environment* env);

#endif //_DEFERRED_H_

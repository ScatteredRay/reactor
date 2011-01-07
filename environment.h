#ifndef _ENVIORNMENT_H_
#define _ENVIRONMENT_H_

struct Environment;

void ResizeRenderBuffers(Environment* e, unsigned int width, unsigned int height);

Environment* InitEnvironment(unsigned int width, unsigned int height);
void DestroyEnvironment(Environment* e);

void RenderEnvironment(Environment* e);

struct DeferredRender;

DeferredRender* InitDeferred();
void DestroyDeferred(DeferredRender* r);
void RenderDeferred(DeferredRender* r, Environment* e);

#endif //_ENFIRONMENT_H_

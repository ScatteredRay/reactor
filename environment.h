#ifndef _ENVIORNMENT_H_
#define _ENVIRONMENT_H_

namespace Vectormath {
    namespace Aos {
        class Matrix4;
    }
}

struct Environment;

void ResizeRenderBuffers(Environment* e, unsigned int width, unsigned int height);

Environment* InitEnvironment(unsigned int width, unsigned int height);
void DestroyEnvironment(Environment* e);

void RenderEnvironment(Environment* e);

struct DeferredRender;

DeferredRender* InitDeferred();
void DestroyDeferred(DeferredRender* r);
void RenderDeferred(DeferredRender* r, Environment* e, const Vectormath::Aos::Matrix4 * projection);

#endif //_ENFIRONMENT_H_

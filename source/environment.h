#ifndef _ENVIORNMENT_H_
#define _ENVIRONMENT_H_

typedef unsigned int GLuint;

struct Environment;

Environment* InitEnvironment(const char* world);
void DestroyEnvironment(Environment* e);

void RenderEnvironment(Environment* e, struct Camera* camera);

struct Uniforms* GetScatteringUniforms(Environment* e);
void CaptureScatteringUniforms(Environment* e, GLuint shader);

#include "reflect.h"

DECLARE_REFLECT_TYPE(Environment)

#endif //_ENFIRONMENT_H_

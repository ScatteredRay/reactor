#ifndef _ENVIORNMENT_H_
#define _ENVIRONMENT_H_

struct Environment;

Environment* InitEnvironment();
void DestroyEnvironment(Environment* e);

void RenderEnvironment(Environment* e, struct Camera* camera);

#endif //_ENFIRONMENT_H_

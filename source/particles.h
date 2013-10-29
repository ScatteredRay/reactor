// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _PARTICLES_H_
#define _PARTICLES_H_

struct Particles;

Particles* InitParticles();
void DestroyParticles(Particles* particles);
// This runs on the GPU, so should be called in render.
void UpdateParticles(Particles* particles);

#endif //_PARTICLES_H_

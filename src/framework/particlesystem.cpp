#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include <cmath>
#include "image.h"
#include "particlesystem.h"

float RandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

void ParticleSystem::Init(unsigned int width, unsigned int height) {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        particles[i].position = Vector2(RandomFloat(0.0f, width), RandomFloat(0.0f, height));
        particles[i].velocity = Vector2(RandomFloat(-50.0f, 50.0f), RandomFloat(-50.0f, 50.0f));
        particles[i].color = Color(rand() % 255, rand() % 255, rand() % 255);
        particles[i].inactive = false;
    }
}

void ParticleSystem::Render(Image* framebuffer) {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (!particles[i].inactive) {
            framebuffer->SetPixel(static_cast<int>(particles[i].position.x), static_cast<int>(particles[i].position.y), particles[i].color);
        }
    }
}

void ParticleSystem::Update(float dt, unsigned int width, unsigned int height) {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (!particles[i].inactive) {
            particles[i].position += particles[i].velocity * dt;

            if (particles[i].position.x < 0.0f || particles[i].position.x >= width ||
                particles[i].position.y < 0.0f || particles[i].position.y >= height) {
                particles[i].inactive = true;
            }
        }
    }
}

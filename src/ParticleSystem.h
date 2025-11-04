#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

// Struct per contenere i dati di una singola particella
struct Particle
{
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec4 Color;
    float Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleSystem
{
public:
    ParticleSystem(Shader &shader, unsigned int amount);
    void Update(float dt, glm::vec3 explosionCenter);
    void Draw();

private:
    Shader &shader;
    unsigned int amount;
    std::vector<Particle> particles;
    unsigned int VAO, VBO;

    void init();
    // Funzione che "rianima" una particella morta, riposizionandola al centro dell'esplosione
    void respawnParticle(Particle &particle, glm::vec3 explosionCenter);
};
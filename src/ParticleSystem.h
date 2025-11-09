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
    
    glm::vec3 startColor;
    glm::vec3 endColor;
    float initialLife;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleSystem
{
public:
    ParticleSystem(Shader &shader, unsigned int amount);
    void Update(float dt); // Gestisce solo la fisica delle particelle VIVE
    void Draw();

    // Permette a un sistema esterno (come Shell) di rianimare una particella
    // con proprietà specifiche.
    void RespawnParticle(Particle &particleProperties);

private:
    Shader &shader;
    unsigned int amount;
    std::vector<Particle> particles;
    unsigned int VAO, VBO;

    // tiene traccia dell'ultima particella che abbiamo rianimato, per evitare di
    // controllare tutto l'array ogni volta. È una semplice ottimizzazione.
    unsigned int lastUsedParticle = 0;

    void init();

    unsigned int findUnusedParticle();
};
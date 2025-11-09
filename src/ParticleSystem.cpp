#include "ParticleSystem.h"
#include <glm/gtc/random.hpp> // Per glm::ballRand

ParticleSystem::ParticleSystem(Shader &shader, unsigned int amount)
    : shader(shader), amount(amount)
{
    this->init();
}

void ParticleSystem::init()
{
    // Creiamo il VAO e il VBO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    // Alloca la memoria sul VBO per contenere tutte le nostre particelle.
    // Non carichiamo dati subito (passiamo nullptr), ma allochiamo solo lo spazio.
    // Usiamo GL_STREAM_DRAW come hint per OpenGL, significa che i dati cambieranno molto
    // frequentemente (ogni frame) e saranno usati per disegnare. OpenGL può usare questo
    // hint per ottimizzare, mettendo i dati in una memoria GPU più veloce per gli accessi in scrittura.
    glBufferData(GL_ARRAY_BUFFER, this->amount * sizeof(Particle), nullptr, GL_STREAM_DRAW);

    // Definiamo i layout degli attributi nel vertex shader
    // Questo deve corrispondere a come la struct Particle è organizzata in memoria!

    // Attributo Posizione (layout = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, Position));

    // Attributo Colore (layout = 1)
    glEnableVertexAttribArray(1);
    // Nota: passiamo 4 componenti (vec4) per il colore
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, Color));

    glBindVertexArray(0);

    // Crea un pool iniziale di particelle
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

void ParticleSystem::Update(float dt)
{
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle &p = this->particles[i];
        p.Life -= dt; // Riduci la vita rimanente

        if (p.Life > 0.0f)
        {
            // La particella è viva, aggiorna la sua posizione
            p.Position += p.Velocity * dt;
            // Applichiamo una semplice gravità
            p.Velocity.y -= 9.81f * dt;

            // Calcola il progresso della vita (da 1.0 a 0.0)
            float lifeProgress = p.Life / p.initialLife;
            
            // Interpola il colore tra startColor e endColor
            glm::vec3 currentColor = glm::mix(p.endColor, p.startColor, lifeProgress);
            
            // Facciamo sfumare la particella verso la trasparenza
            p.Color = glm::vec4(currentColor, p.Life * 2.0f); // Moltiplicato per 2 per un fade-out più veloce

            if (p.isEmitter)
            {
                p.emitTimer += dt;
                if (p.emitTimer >= p.emitInterval)
                {
                    // È ora di emettere una particella di scia!
                    Particle trail;
                    trail.Position = p.Position;
                    // La velocità è quasi zero, per farla rimanere indietro
                    trail.Velocity = p.Velocity * 0.1f + glm::ballRand(0.2f);
                    trail.Life = 0.4f; // Vita molto breve
                    trail.initialLife = 0.4f;
                    trail.startColor = glm::vec3(1.0f, 1.0f, 0.8f); // Colore da scintilla
                    trail.endColor = glm::vec3(0.3f, 0.3f, 0.3f);
                    trail.isEmitter = false; // La particella di scia non è un emettitore

                    // Trova uno slot libero e rianimalo
                    RespawnParticle(trail);

                    p.emitTimer = 0.0f; // Resetta il timer
                }
            }
        }
    }

    // Aggiorniamo il VBO con i nuovi dati delle particelle
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // Mappiamo l'intero buffer, ma questo potrebbe essere lento.
    // glBufferSubData è un'alternativa più performante.
    glBufferData(GL_ARRAY_BUFFER, this->amount * sizeof(Particle), this->particles.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::Draw()
{
    // Abilitiamo il blending per permettere la trasparenza.
    // GL_SRC_ALPHA e GL_ONE sono una combinazione comune per effetti additivi
    // come fuoco, magia e fuochi d'artificio, dove le luci si sommano.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Abilitiamo GL_PROGRAM_POINT_SIZE per permettere al vertex shader
    // di impostare la dimensione dei punti con gl_PointSize.
    glEnable(GL_PROGRAM_POINT_SIZE);

    this->shader.use();

    glBindVertexArray(this->VAO);
    // Disegniamo tutte le particelle con una singola chiamata!
    glDrawArrays(GL_POINTS, 0, this->amount);
    glBindVertexArray(0);

    // Ripristiniamo lo stato di blending predefinito
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
}

// Cerca la prossima particella "morta" nel nostro pool.
unsigned int ParticleSystem::findUnusedParticle()
{
    // Cerca dall'ultima particella usata in avanti
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i)
    {
        if (particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    // Se non ne trova, ricomincia dall'inizio
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if (particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    // Se tutte le particelle sono vive, sovrascrivi la prima (caso limite)
    lastUsedParticle = 0;
    return 0;
}

void ParticleSystem::RespawnParticle(Particle &particleProperties)
{
    unsigned int particleIndex = findUnusedParticle();
    this->particles[particleIndex] = particleProperties;
}
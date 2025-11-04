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

void ParticleSystem::Update(float dt, glm::vec3 explosionCenter)
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
            p.Velocity.y -= 9.81f * 0.1f * dt; // TODO: ask AI

            // Facciamo sfumare la particella verso la trasparenza
            p.Color.a = p.Life * 2.0f; //TODO: ask AI Moltiplicato per 2 per un fade-out più veloce
        }
        else
        {
            // La particella è "morta", la facciamo rinascere al centro della prossima esplosione
            // NOTA: in un sistema reale, non la faresti rinascere subito,
            // ma la terresti in un "pool" di particelle morte.
            // Per questo esempio, la rianimiamo per semplicità.
            respawnParticle(p, explosionCenter);
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

void ParticleSystem::respawnParticle(Particle &particle, glm::vec3 explosionCenter)
{
    // Scegliamo una velocità iniziale casuale che si espande in tutte le direzioni.
    // glm::ballRand(1.0f) genera un vettore casuale dentro una sfera di raggio 1.0.
    // Moltiplicandolo per una forza, otteniamo una bella esplosione.
    float randomForce = 5.0f + (rand() % 100) / 100.0f * 5.0f; // Forza tra 5 e 10
    particle.Velocity = glm::ballRand(1.0f) * randomForce;

    // Impostiamo la posizione iniziale al centro dell'esplosione
    particle.Position = explosionCenter;

    // Assegnamo un colore casuale (es. rosso-arancio-giallo)
    particle.Color = glm::vec4(1.0f, 0.5f + (rand() % 100) / 200.0f, 0.0f, 1.0f);

    // Diamo una vita casuale per far sì che le particelle non muoiano tutte insieme
    particle.Life = 1.0f + (rand() % 100) / 100.0f; // Vita tra 1.0 e 2.0 secondi
}
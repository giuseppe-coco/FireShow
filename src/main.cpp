#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "ParticleSystem.h"

// --- Impostazioni ---
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// --- Timing ---
float deltaTime = 0.0f; // Tempo tra il frame corrente e l'ultimo frame
float lastFrame = 0.0f;

// --- Camera ---
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 50.0f);

int main()
{
    // --- Inizializzazione GLFW e GLAD ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fireworks Simulator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // --- Creazione risorse ---
    Shader particleShader("shaders/particle.vert", "shaders/particle.frag");
    ParticleSystem particles(particleShader, 5000); // Creiamo un sistema con 5000 particelle

    // Vogliamo che le esplosioni avvengano ad altezze diverse
    glm::vec3 explosionCenter = glm::vec3(0.0f, 20.0f, 0.0f);

    // --- Render Loop ---
    while (!glfwWindowShouldClose(window))
    {
        // --- Calcolo DeltaTime ---
        // Questo è FONDAMENTALE per avere una fisica indipendente dal framerate.
        // `deltaTime` conterrà il tempo esatto (in secondi) trascorso dall'ultimo frame.
        // Moltiplichiamo tutte le velocità e le accelerazioni per questo valore.
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- Input ---
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Se premi SPAZIO, generiamo una nuova esplosione in un punto casuale
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            explosionCenter.x = (rand() % 40) - 20;    // x tra -20 e 20
            explosionCenter.y = 15.0f + (rand() % 10); // y tra 15 e 25
            explosionCenter.z = (rand() % 20) - 10;    // z tra -10 e 10

            // "Respawna" tutte le particelle nel nuovo punto di esplosione
            particles.Update(999.0f, explosionCenter); // Passiamo un dt enorme per "uccidere" istantaneamente tutte le particelle vecchie
        }

        // --- Rendering ---
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Un blu scuro per il cielo notturno
        glClear(GL_COLOR_BUFFER_BIT);

        // --- Aggiorna e Disegna Particelle ---
        particles.Update(deltaTime, explosionCenter);

        // Setup delle matrici di trasformazione
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        particleShader.use();
        particleShader.setMat4("projection", projection);
        particleShader.setMat4("view", view);

        particles.Draw();

        // --- Swap buffers e poll events ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
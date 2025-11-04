#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "ParticleSystem.h"
#include "FireworksShell.h"

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
    ParticleSystem particleSystem(particleShader, 10000); // Creiamo un sistema con 5000 particelle

    // Creiamo un "pool" di proiettili. Invece di creare e distruggere oggetti,
    // li riutilizziamo. Questo è molto più efficiente.
    const int MAX_SHELLS = 10;
    std::vector<FireworksShell> shellPool;
    for (int i = 0; i < MAX_SHELLS; ++i)
        shellPool.emplace_back(particleSystem); // emplace_back costruisce l'oggetto direttamente nel vettore

    float launchTimer = 0.0f;

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

        // --- Logica di lancio ---
        launchTimer += deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && launchTimer > 0.5f) // Limita il lancio a uno ogni 0.5s
        {
            // Cerca un proiettile inattivo nel pool
            for (auto &shell : shellPool)
            {
                if (shell.GetState() == ShellState::INACTIVE)
                {
                    // Trovato! Lo lanciamo.
                    glm::vec3 startPos = glm::vec3((rand() % 40) - 20, 0.0f, (rand() % 20) - 10);
                    glm::vec3 startVel = glm::vec3(0.0f, 10.0f + (rand() % 10), 0.0f);
                    float fuse = 2.0f + (rand() % 100) / 100.0f;

                    shell.Launch(startPos, startVel, fuse);
                    launchTimer = 0.0f; // Resetta il timer
                    break;              // Esci dal ciclo una volta lanciato un proiettile
                }
            }
        }

        // --- Aggiornamenti ---
        // Aggiorna tutti i proiettili attivi
        for (auto &shell : shellPool)
        {
            shell.Update(deltaTime);
        }
        // Aggiorna la fisica di tutte le particelle
        particleSystem.Update(deltaTime);

        // --- Rendering ---
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Un blu scuro per il cielo notturno
        glClear(GL_COLOR_BUFFER_BIT);

        // Setup delle matrici di trasformazione
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        particleShader.use();
        particleShader.setMat4("projection", projection);
        particleShader.setMat4("view", view);

        particleSystem.Draw();

        // --- Swap buffers e poll events ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
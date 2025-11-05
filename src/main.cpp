#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

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

unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Imposta il wrapping della texture per il terreno
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // glViewport dice a OpenGL qual è la dimensione dell'area di rendering.
    // In questo modo, il nostro rendering si adatterà alla nuova dimensione della finestra.
    glViewport(0, 0, width, height);
}

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // --- Creazione risorse ---
    Shader particleShader("shaders/particle.vert", "shaders/particle.frag");
    Shader groundShader("shaders/ground.vert", "shaders/ground.frag");
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

    ParticleSystem particleSystem(particleShader, 10000);

    // Creiamo un "pool" di proiettili. Invece di creare e distruggere oggetti,
    // li riutilizziamo. Questo è molto più efficiente.
    const int MAX_SHELLS = 10;
    std::vector<FireworksShell> shellPool;
    for (int i = 0; i < MAX_SHELLS; ++i)
        shellPool.emplace_back(particleSystem); // emplace_back costruisce l'oggetto direttamente nel vettore

    float launchTimer = 0.0f;

    float groundVertices[] = {
        // positions            // texture Coords
        25.0f, 0.0f, 25.0f,     25.0f, 0.0f,
        -25.0f, 0.0f, 25.0f,    0.0f, 0.0f,
        -25.0f, 0.0f, -25.0f,   0.0f, 25.0f,

        25.0f, 0.0f, 25.0f,     25.0f, 0.0f,
        -25.0f, 0.0f, -25.0f,   0.0f, 25.0f,
        25.0f, 0.0f, -25.0f,    25.0f, 25.0f
    };
    unsigned int groundVAO, groundVBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float skyVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Configura gli shader con le unità texture
    unsigned int groundTexture = loadTexture("textures/ground.jpg");
    unsigned int skyTexture = loadTexture("textures/sky.jpg");
    groundShader.use();
    groundShader.setInt("groundTexture", 0);
    skyboxShader.use();
    skyboxShader.setInt("skyTexture", 0);

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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Non importa più, ma nero è una buona base

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Setup delle matrici di trasformazione
        glm::mat4 projection = glm::perspective(glm::radians(65.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // --- Disegna lo Skybox ---
        // Cambia la funzione di test di profondità.
        // Il trucco è dire a OpenGL: "disegna lo skybox solo se la sua profondità è
        // MINORE O UGUALE a quella già presente". Poiché lo skybox ha sempre la
        // profondità massima (1.0), questo test passerà solo dove non c'è nient'altro,
        // riempiendo gli spazi vuoti del cielo.
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE); // Disabilita la scrittura sul depth buffer
        skyboxShader.use();
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyTexture);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 2. DISEGNA GLI OGGETTI OPACHI (TERRENO)
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        groundShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(20.0f)); // Rendi il terreno molto grande
        groundShader.setMat4("model", model);
        groundShader.setMat4("view", view);
        groundShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);

        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- Disegna le Particelle ---
        // Disabilita la scrittura sul depth buffer per le particelle.
        // Questo evita che particelle trasparenti "taglino" quelle dietro di loro in modo strano.
        glDepthMask(GL_FALSE);
        particleShader.use();
        particleShader.setMat4("projection", projection);
        particleShader.setMat4("view", view);
        particleSystem.Draw();

        glDepthMask(GL_TRUE); // Riabilita la scrittura per il resto della scena

        // --- Swap buffers e poll events ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "../vendors/imgui/imgui.h"
#include "../vendors/imgui/imgui_impl_glfw.h"
#include "../vendors/imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <memory> // Per std::unique_ptr 

#include "Shader.h"
#include "ParticleSystem.h"
#include "shell/Shell.h"
#include "Timeline.h"
#include "FireworkTypes.h"
#include "Editor.h"

// --- Impostazioni ---
#define SETTINGS_HEIGHT 270

// --- Timing ---
float deltaTime = 0.0f; // Tempo tra il frame corrente e l'ultimo frame
float lastFrame = 0.0f;

// --- Camera ---
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 50.0f);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
unsigned int loadTexture(char const *path);

int main()
{
    // --- Inizializzazione GLFW e GLAD ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    const unsigned int SCR_WIDTH = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
    const unsigned int SCR_HEIGHT = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;

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

    // --- Setup di DEAR IMGUI ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io; 
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Abilita controllo da tastiera
    ImGui::StyleColorsDark(); // Scegli lo stile
    // Inizializza i backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --- Creazione risorse ---
    Timeline timeline;
    Editor editor;
    std::vector<Firework> &lib = editor.getFireworksLibrary();
    Shader groundShader("shaders/ground.vert", "shaders/ground.frag");
    Shader particleShader("shaders/particle.vert", "shaders/particle.frag");

    ParticleSystem particleSystem(particleShader, 10000); // Creiamo un sistema con 10000 particelle

    // --- POOL DI PROIETTILI ---
    // Creiamo un "pool" di proiettili. Invece di creare e distruggere oggetti, li riutilizziamo. Questo è molto più efficiente.
    // std::unique_ptr gestisce automaticamente la memoria, prevenendo memory leak.
    const int MAX_SHELLS = 20;
    std::vector<std::unique_ptr<Shell>> shellPool(MAX_SHELLS);

    float groundVertices[] = {
        // positions          // texture Coords
        25.0f, 0.0f, 25.0f, 25.0f, 0.0f,
        -25.0f, 0.0f, 25.0f, 0.0f, 0.0f,
        -25.0f, 0.0f, -25.0f, 0.0f, 25.0f,

        25.0f, 0.0f, 25.0f, 25.0f, 0.0f,
        -25.0f, 0.0f, -25.0f, 0.0f, 25.0f,
        25.0f, 0.0f, -25.0f, 25.0f, 25.0f};
    
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

    unsigned int groundTexture = loadTexture("textures/ground.jpg");
    groundShader.use();
    groundShader.setInt("groundTexture", 0);

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

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        // Imposta il viewport per la scena 3D nell'area superiore
        glViewport(0, SETTINGS_HEIGHT, windowWidth, windowHeight - SETTINGS_HEIGHT);
        
        // --- Inizia un nuovo frame di ImGui ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowMetricsWindow();

        timeline.DrawUI(windowWidth / 2.0f, windowHeight, SETTINGS_HEIGHT, lib);
        editor.DrawUI(windowWidth / 2.0, windowHeight, SETTINGS_HEIGHT);

        // --- Input ---
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Aggiorna la timeline e ottieni gli eventi da eseguire
        auto eventsToTrigger = timeline.Update(deltaTime);
        if (!eventsToTrigger.empty())
        { 
            for (const auto *eventData : eventsToTrigger)
            {
                // Cerca un proiettile inattivo e lancialo con i dati dell'evento
                for (auto &shellPtr : shellPool)
                {
                    if (!shellPtr) // Se il puntatore è nullo, lo slot è libero
                    {
                        // Usa la factory per creare la shell corretta
                        shellPtr = Shell::createShell(eventData->fire, particleSystem);
                        shellPtr->Launch(*eventData);
                        
                        break;
                    } 
                }
            }
        } 

        // Aggiorna tutti i proiettili attivi
        for (auto &shellPtr : shellPool)
        {
            if (shellPtr) // Assicurati che il puntatore non sia nullo
            {
                shellPtr->Update(deltaTime);
                // Se la shell ha finito, resetta il puntatore per liberare lo slot
                if (shellPtr->GetState() == ShellState::INACTIVE)
                    shellPtr.reset(); // Questo distrugge l'oggetto e imposta il ptr a nullptr
            }
        }

        // Aggiorna la fisica di tutte le particelle
        particleSystem.Update(deltaTime);

        // --- Rendering ---
        glClearColor(0.10, 0.10, 0.22, 1.0);     // Un blu scuro
        // glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Un blu scuro per il cielo notturno
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Setup delle matrici di trasformazione
        float aspectRatio = (float)windowWidth / (float)(windowHeight - SETTINGS_HEIGHT);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // --- Disegna il Terreno ---
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

        // --- Disegna il Particelle  ---
        glDepthMask(GL_FALSE);
        particleShader.use();
        particleShader.setMat4("projection", projection);
        particleShader.setMat4("view", view);
        particleSystem.Draw();
        glDepthMask(GL_TRUE);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // --- Swap buffers e poll events ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

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

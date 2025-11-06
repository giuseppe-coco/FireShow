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
#include <map>

#include "Shader.h"
#include "ParticleSystem.h"
#include "FireworksShell.h"
#include "Timeline.h"
#include "FireworkTypes.h"

// --- Impostazioni ---
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

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

    // --- LIBRERIA DI FUOCHI D'ARTIFICIO ---
    std::map<int, FireworkType> fireworkLibrary;
    int nextFireworkTypeId = 0;

    // Creiamo un tipo di default
    FireworkType defaultType;
    defaultType.id = nextFireworkTypeId++;
    defaultType.name = "Default Peony";
    fireworkLibrary[defaultType.id] = defaultType;

    // Puntatore al tipo di fuoco attualmente selezionato nell'editor
    FireworkType *selectedType = &fireworkLibrary[0];

    // --- Creazione risorse ---
    Timeline timeline;
    Shader groundShader("shaders/ground.vert", "shaders/ground.frag");

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

    Shader particleShader("shaders/particle.vert", "shaders/particle.frag");
    ParticleSystem particleSystem(particleShader, 10000); // Creiamo un sistema con 10000 particelle

    // Creiamo un "pool" di proiettili. Invece di creare e distruggere oggetti,
    // li riutilizziamo. Questo è molto più efficiente.
    const int MAX_SHELLS = 10;
    std::vector<FireworksShell> shellPool;
    for (int i = 0; i < MAX_SHELLS; ++i)
        shellPool.emplace_back(particleSystem); // emplace_back costruisce l'oggetto direttamente nel vettore

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
        const int timelineHeight = 200; // Altezza in pixel riservata alla timeline
        // Imposta il viewport per la scena 3D nell'area superiore
        glViewport(0, timelineHeight, windowWidth, windowHeight - timelineHeight);
        
        // --- Inizia un nuovo frame di ImGui ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        timeline.DrawUI(windowWidth, windowHeight, timelineHeight, fireworkLibrary);

        // --- DISEGNA UI EDITOR FUOCHI D'ARTIFICIO ---
        ImGui::Begin("Firework Editor");
        // --- Lista dei tipi di fuochi (la nostra libreria) ---
        if (ImGui::Button("Add New Type"))
        {
            FireworkType newType;
            newType.id = nextFireworkTypeId++;
            newType.name = "New Firework " + std::to_string(newType.id);
            fireworkLibrary[newType.id] = newType;
            selectedType = &fireworkLibrary[newType.id]; // Seleziona il nuovo tipo
        }
        ImGui::Separator();
        ImGui::Text("Library:");
        for (auto &pair : fireworkLibrary)
        {
            // Seleziona un tipo dalla lista
            if (ImGui::Selectable(pair.second.name.c_str(), selectedType && selectedType->id == pair.second.id))
                selectedType = &pair.second;
        }
        ImGui::Separator();
        // --- Editor delle proprietà del tipo selezionato ---
        if (selectedType)
        {
            ImGui::Text("Editing: %s", selectedType->name.c_str());
            char nameBuffer[128];
            strncpy(nameBuffer, selectedType->name.c_str(), sizeof(nameBuffer));
            if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
                selectedType->name = nameBuffer;

            ImGui::DragInt("Particle Count", &selectedType->particleCount, 10, 10, 10000);
            ImGui::DragFloatRange2("Lifetime", &selectedType->minLifetime, &selectedType->maxLifetime, 0.1f, 0.1f, 10.0f);
            ImGui::DragFloatRange2("Speed", &selectedType->minSpeed, &selectedType->maxSpeed, 0.5f, 0.0f, 100.0f);
            ImGui::ColorEdit3("Start Color", (float *)&selectedType->startColor);
            ImGui::ColorEdit3("End Color", (float *)&selectedType->endColor);
            ImGui::SliderFloat("Gravity Modifier", &selectedType->gravityModifier, 0.0f, 2.0f);
        }
        ImGui::End();

        // --- Input ---
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Aggiorna la timeline e ottieni gli eventi da eseguire
        auto eventsToTrigger = timeline.Update(deltaTime);
        if (!eventsToTrigger.empty())
        {
            for (const auto *eventData : eventsToTrigger)
            {
                if (fireworkLibrary.count(eventData->fireworkTypeId))
                {
                    const FireworkType *typeToLaunch = &fireworkLibrary.at(eventData->fireworkTypeId);
                    // Cerca un proiettile inattivo e lancialo con i dati dell'evento
                    for (auto &shell : shellPool)
                    {
                        if (shell.GetState() == ShellState::INACTIVE)
                        {
                            shell.Launch(*eventData, typeToLaunch);
                            break; // Lancia solo un proiettile per evento e passa al prossimo
                        }
                    }
                }
            }
        }

        // Aggiorna tutti i proiettili attivi
        for (auto &shell : shellPool)
        {
            shell.Update(deltaTime);
        }
        // Aggiorna la fisica di tutte le particelle
        particleSystem.Update(deltaTime);

        // --- Rendering ---
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Un blu scuro per il cielo notturno
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Setup delle matrici di trasformazione
        float aspectRatio = (float)windowWidth / (float)(windowHeight - timelineHeight);
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

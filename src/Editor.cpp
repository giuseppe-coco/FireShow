// Editor.cpp

#include "Editor.h"
#include <GLFW/glfw3.h>
#include "../vendors/imgui/imgui.h"

Editor::Editor()
{
    createHardcodedFireworks();
    // Imposta un default per selectedType, per evitare crash
    if (!fireworksLibrary.empty())
        selectedType = &fireworksLibrary[0];
}

void Editor::createHardcodedFireworks()
{
    // --- 1. PEONIA ROSSA CLASSICA ---
    Firework peonyRed;
    peonyRed.id = nextFireworkTypeId++;
    peonyRed.name = "Classic Red Peony";
    peonyRed.family = FireworkFamily::Peony;

    // Proprietà particelle
    peonyRed.particleCount = 300;
    peonyRed.minLifetime = 1.0f;
    peonyRed.maxLifetime = 1.5f;
    peonyRed.minSpeed = 18.0f;
    peonyRed.maxSpeed = 28.0f;
    peonyRed.startColor = glm::vec3(1.0f, 0.1f, 0.1f); // Rosso vivo
    peonyRed.endColor = glm::vec3(0.4f, 0.0f, 0.0f);   // Rosso scuro che svanisce

    // Proprietà lancio (valori di default)
    peonyRed.startShellPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    peonyRed.startShellVelocity = glm::vec3(0.0f, 15.0f, 0.0f);
    peonyRed.fuseTime = 2.0f;
    
    fireworksLibrary.push_back(peonyRed);

    // --- 2. CRISANTEMO BIANCO SCINTILLANTE ---
    // Nota: La simulazione visiva del Crisantemo richiede che le particelle
    // generino altre particelle (sub-emitters). Poiché non abbiamo ancora questa
    // logica, lo simuleremo con più particelle, una vita più lunga e un colore
    // che ricorda le scintille. Sarà visivamente un "fratello maggiore" della Peonia.
    Firework chrysanthemumWhite;
    chrysanthemumWhite.id = nextFireworkTypeId++;
    chrysanthemumWhite.name = "Sparkling White Chrysanthemum";
    chrysanthemumWhite.family = FireworkFamily::Chrysanthemum; // Cambia la famiglia

    // Proprietà particelle
    chrysanthemumWhite.particleCount = 300; // Più denso
    chrysanthemumWhite.minLifetime = 1.0f; // Vita più lunga per un effetto più persistente
    chrysanthemumWhite.maxLifetime = 1.5f;
    chrysanthemumWhite.minSpeed = 18.0f;
    chrysanthemumWhite.maxSpeed = 28.0f;
    chrysanthemumWhite.startColor = glm::vec3(0.1f, 1.0f, 0.1f);
    chrysanthemumWhite.endColor = glm::vec3(0.0f, 0.4f, 0.0f);   // Rosso scuro che svanisce

    // Proprietà lancio
    chrysanthemumWhite.startShellPosition = glm::vec3(10.0f, 0.0f, 0.0f); // Lancio leggermente spostato
    chrysanthemumWhite.startShellVelocity = glm::vec3(0.0f, 15.0f, 0.0f); // Sale un po' più in alto
    chrysanthemumWhite.fuseTime = 2.0f;

    fireworksLibrary.push_back(chrysanthemumWhite);

    // --- 3. PEONIA BLU VELOCE ---
    Firework peonyBlue;
    peonyBlue.id = nextFireworkTypeId++;
    peonyBlue.name = "Fast Blue Peony";
    peonyBlue.family = FireworkFamily::Peony;

    // Proprietà particelle
    peonyBlue.particleCount = 200; // Esplosione più piccola
    peonyBlue.minLifetime = 0.8f;
    peonyBlue.maxLifetime = 1.2f;
    peonyBlue.minSpeed = 30.0f; // Molto più veloce!
    peonyBlue.maxSpeed = 40.0f;
    peonyBlue.startColor = glm::vec3(0.2f, 0.2f, 1.0f); // Blu
    peonyBlue.endColor = glm::vec3(0.0f, 0.0f, 0.3f);   // Blu scuro

    // Proprietà lancio
    peonyBlue.startShellPosition = glm::vec3(-30.0f, 0.0f, 0.0f);
    peonyBlue.startShellVelocity = glm::vec3(0.0f, 15.0f, 0.0f);
    peonyBlue.fuseTime = 2.0f;

    fireworksLibrary.push_back(peonyBlue);
}

void Editor::DrawUI(
    const int editorWidth,
    const int windowHeight,
    const int editorHeight
)
{
    // Imposta la posizione e la dimensione della prossima finestra di ImGui
    ImGui::SetNextWindowPos(ImVec2(editorWidth, windowHeight - editorHeight)); // Posizione (0,0) in alto a sinistra dello schermo
    ImGui::SetNextWindowSize(ImVec2(editorWidth, editorHeight));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Firework Editor", nullptr, window_flags);
    if (ImGui::Button("Add New Type"))
    {
        Firework newType;
        newType.id = nextFireworkTypeId++;
        newType.name = "New Firework " + std::to_string(newType.id);
        fireworksLibrary.push_back(newType);
    }
    ImGui::Separator();
    ImGui::Text("Library:");
    for (auto& elem : fireworksLibrary)
    {
        // Seleziona un tipo dalla lista
        if (ImGui::Selectable(elem.name.c_str()))
            selectedType = &elem;
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
        ImGui::InputFloat("Start Shell Position", &selectedType->startShellPosition.x, 1.0f, 100.0f, "%.1f");
        ImGui::InputFloat("Start Shell velocity", &selectedType->startShellVelocity.y, 1.0f, 100.0f, "%.1f");
        ImGui::ColorEdit3("Start Color Particle", (float *)&selectedType->startColor);
        ImGui::ColorEdit3("End Color Particle", (float *)&selectedType->endColor);
    }
    ImGui::End();
}
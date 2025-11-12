// Editor.cpp

#include "Editor.h"
#include <GLFW/glfw3.h>
#include <memory> // Per std::unique_ptr
#include "../vendors/imgui/imgui.h"

#include "firework/Firework.h"
#include "firework/Peony.h"
#include "firework/Chrysanthemum.h"
#include "firework/Willow.h"
#include "firework/Volcano.h"

Editor::Editor(ParticleSystem &ps)
{
    createHardcodedFireworks(ps);
    // Imposta un default per selectedType, per evitare crash
    if (!fireworksLibrary.empty())
        selectedType = fireworksLibrary[0].get();
}

void Editor::createHardcodedFireworks(ParticleSystem &ps)
{
    // --- 1. PEONIA ROSSA CLASSICA ---
    auto peonyRed = std::make_unique<Peony>(ps);
    peonyRed->id = nextFireworkTypeId++;
    peonyRed->name = "Classic Red Peony";
    peonyRed->family = FireworkFamily::Peony;

    // Proprietà particelle
    peonyRed->particleCount = 50;
    peonyRed->minLifetime = 1.0f;
    peonyRed->maxLifetime = 1.5f;
    peonyRed->minSpeed = 18.0f;
    peonyRed->maxSpeed = 28.0f;
    peonyRed->startColor = glm::vec3(1.0f, 0.1f, 0.1f); // Rosso vivo
    peonyRed->endColor = glm::vec3(0.4f, 0.0f, 0.0f);   // Rosso scuro che svanisce

    // Proprietà lancio (valori di default)
    peonyRed->position = glm::vec3(0.0f, 0.0f, 0.0f);
    peonyRed->velocity = glm::vec3(0.0f, 20.0f, 0.0f);
    peonyRed->fuse = 2.0f;

    fireworksLibrary.push_back(std::move(peonyRed));

    // --- 2. CRISANTEMO BIANCO SCINTILLANTE ---
    auto chrysanthemumWhite = std::make_unique<Chrysanthemum>(ps);
    chrysanthemumWhite->id = nextFireworkTypeId++;
    chrysanthemumWhite->name = "Sparkling White Chrysanthemum";
    chrysanthemumWhite->family = FireworkFamily::Chrysanthemum;

    chrysanthemumWhite->particleCount = 50;
    chrysanthemumWhite->minLifetime = 1.0f;
    chrysanthemumWhite->maxLifetime = 1.5f;
    chrysanthemumWhite->minSpeed = 18.0f;
    chrysanthemumWhite->maxSpeed = 28.0f;
    chrysanthemumWhite->startColor = glm::vec3(0.1f, 1.0f, 0.1f);
    chrysanthemumWhite->endColor = glm::vec3(0.0f, 0.4f, 0.0f);

    chrysanthemumWhite->position = glm::vec3(10.0f, 0.0f, 0.0f);
    chrysanthemumWhite->velocity = glm::vec3(0.0f, 20.0f, 0.0f);
    chrysanthemumWhite->fuse = 2.0f;

    fireworksLibrary.push_back(std::move(chrysanthemumWhite));

    auto willow = std::make_unique<Willow>(ps);
    willow->id = nextFireworkTypeId++;
    willow->name = "Willow";
    willow->family = FireworkFamily::Willow;
    willow->particleCount = 200;
    willow->minLifetime = 2.0f;
    willow->maxLifetime = 2.0f;
    willow->minSpeed = 10.0f;
    willow->maxSpeed = 12.0f;
    willow->startColor = glm::vec3(0.9f, 0.743f, 0.0f);
    willow->endColor = glm::vec3(1.0f, 0.843f, 0.0f);
    willow->position = glm::vec3(-30.0f, 0.0f, 0.0f);
    willow->velocity = glm::vec3(0.0f, 20.0f, 0.0f);
    willow->fuse = 2.0f;
    fireworksLibrary.push_back(std::move(willow));

    auto volcano = std::make_unique<Volcano>(ps);
    volcano->id = nextFireworkTypeId++;
    volcano->name = "Volcano";
    volcano->family = FireworkFamily::Volcano;
    volcano->particleCount = 200;
    volcano->minLifetime = 2.0f;
    volcano->maxLifetime = 2.0f;
    volcano->minSpeed = 10.0f;
    volcano->maxSpeed = 12.0f;
    volcano->startColor = glm::vec3(0.9f, 0.743f, 0.0f);
    volcano->endColor = glm::vec3(1.0f, 0.843f, 0.0f);
    volcano->position = glm::vec3(-30.0f, 0.0f, 0.0f);
    volcano->velocity = glm::vec3(0.0f, 20.0f, 0.0f);
    volcano->fuse = 2.0f;
    fireworksLibrary.push_back(std::move(volcano));

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
    ImGui::Separator();
    ImGui::Text("Library:");
    for (auto& elem : fireworksLibrary)
    {
        // Seleziona un tipo dalla lista
        if (ImGui::Selectable(elem->name.c_str()))
            selectedType = elem.get();
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
        ImGui::InputFloat("Start Firework Position", &selectedType->position.x, 1.0f, 100.0f, "%.1f");
        ImGui::InputFloat("Start Firework velocity", &selectedType->velocity.y, 1.0f, 100.0f, "%.1f");
        ImGui::ColorEdit3("Start Color Particle", (float *)&selectedType->startColor);
        ImGui::ColorEdit3("End Color Particle", (float *)&selectedType->endColor);
    }
    ImGui::End();
}
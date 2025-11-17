// Editor.h
#pragma once

#include <map>

#include "FireworkTypes.h"

class Editor
{
private:
    int nextFireworkTypeId;
    std::map<int, Firework> fireworksLibrary;
    Firework *selectedType = nullptr; // tipo di fuoco attualmente selezionato nell'editor
    void createHardcodedFireworks();

public: 
    Editor();
    std::map<int, Firework>& getFireworksLibrary() { return fireworksLibrary; }
    void DrawUI(int windowWidth, int windowHeight, const int editorHeight);
};
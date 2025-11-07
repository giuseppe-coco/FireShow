// Editor.h

#pragma once

#include <map>

#include "FireworkTypes.h"

class Editor
{
private:
    int nextFireworkTypeId;
    // --- LIBRERIA DI FUOCHI D'ARTIFICIO ---
    std::map<int, FireworkType> fireworksLibrary;
    // Puntatore al tipo di fuoco attualmente selezionato nell'editor
    FireworkType *selectedType = nullptr;

public: 
    Editor();
    std::map<int, FireworkType>& getFireworksLibrary() { return fireworksLibrary; }
    void DrawUI(int windowWidth, int windowHeight, const int editorHeight);
};
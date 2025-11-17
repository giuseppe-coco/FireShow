#pragma once

#include "miniaudio.h"

#include <string>
#include <map>
#include <vector>
#include <memory>

// Definiamo un "deleter" personalizzato per ma_sound con unique_ptr.
// Questo insegna a unique_ptr come liberare correttamente una ma_sound.
struct MaSoundDeleter
{
    void operator()(ma_sound *p) const
    {
        if (p)
        {
            ma_sound_uninit(p);
            delete p;
        }
    }
};

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    // Metodo per inizializzare il motore e caricare tutti i suoni
    // Ritorna 'true' in caso di successo, 'false' altrimenti.
    bool Init();

    // Riproduce un suono pre-caricato usando il suo nome/ID
    void Play(const std::string &soundName);

    // Libera tutte le risorse
    void Shutdown();

private:
    ma_engine engine;

    // Cache di suoni. La chiave è il nome del suono,
    // il valore è l'oggetto ma_sound che contiene i dati decodificati.
    std::map<std::string, std::unique_ptr<ma_sound, MaSoundDeleter>> soundCache;

    // Flag per sapere se il motore è stato inizializzato correttamente
    bool isInitialized;

    // Metodo helper per caricare un singolo suono. Usato da Init().
    bool loadSound(const std::string &soundName, const std::string &filepath);
};
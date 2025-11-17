#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"

#include <iostream>

AudioManager::AudioManager() : isInitialized(false) {}

AudioManager::~AudioManager()
{
    Shutdown();
}

bool AudioManager::Init()
{
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
        std::cerr << "ERROR: Failed to initialize audio engine." << std::endl;
        return false;
    }
    std::cout << "Audio engine initialized successfully." << std::endl;

    loadSound("default_launch", "sounds/launch-whistle.wav");
    loadSound("default_explosion", "sounds/loud-short-explosion.wav");

    // TODO: add firework-specific sounds in the future
    isInitialized = true;
    return true;
}

bool AudioManager::loadSound(const std::string &soundName, const std::string &filepath)
{
    if (soundCache.count(soundName))
        return true;

    // 1. Alloca un nuovo oggetto ma_sound sull'heap.
    // Usiamo std::unique_ptr per gestire automaticamente la sua memoria.
    auto soundPtr = std::unique_ptr<ma_sound, MaSoundDeleter>(new ma_sound());

    // 2. Inizializza l'oggetto puntato.
    ma_result result = ma_sound_init_from_file(&engine, filepath.c_str(), 0, NULL, NULL, soundPtr.get());

    if (result != MA_SUCCESS)
    {
        std::cerr << "WARNING: Failed to load sound file: " << filepath << std::endl;
        return false;
    }

    // 3. Sposta il puntatore intelligente nella mappa.
    // La memoria ora è gestita dalla mappa.
    soundCache[soundName] = std::move(soundPtr);
    std::cout << "Loaded sound: " << soundName << " from " << filepath << std::endl;
    return true;
}

void AudioManager::Play(const std::string &soundName)
{
    if (!isInitialized) return;

    auto it = soundCache.find(soundName);
    if (it != soundCache.end())
    {
        float randomPitch = 0.95f + (rand() % 100 / 1000.0f);
        float randomVolume = 0.9f + (rand() % 100 / 200.0f);
        ma_sound_set_pitch(it->second.get(), randomPitch);
        ma_sound_set_volume(it->second.get(), randomVolume);

        ma_sound_seek_to_pcm_frame(it->second.get(), 0); // Rewinds to the start
        ma_sound_start(it->second.get());
    }
    else
        std::cerr << "WARNING: Attempted to play sound not found in cache: " << soundName << std::endl;
}

void AudioManager::Shutdown()
{
    if (!isInitialized) return;

    // // Prima di de-inizializzare il motore, dobbiamo liberare
    // // la memoria di ogni suono che abbiamo caricato.
    // for (auto &pair : soundCache)
    //     ma_sound_uninit(&pair.second);
    
    soundCache.clear();
    ma_engine_uninit(&engine);
    isInitialized = false;
    std::cout << "Audio engine shut down." << std::endl;
}
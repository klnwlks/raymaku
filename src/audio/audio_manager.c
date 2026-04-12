#include "audio_manager.h"

static Sound soundPool[MAX_SOUND_EVENTS] = { 0 };

// Map event to file path
static const char* soundPaths[MAX_SOUND_EVENTS] = {
    "resources/shoot.wav",       // SND_SHOOT
    "resources/hit.wav",         // SND_HIT_ENEMY
    "resources/bomb.wav",        // SND_BOMB
    "resources/move.wav",        // SND_MENU_MOVE
    "resources/select.wav",      // SND_MENU_SELECT
    "resources/player_hit.wav",  // SND_PLAYER_HIT
    "resources/enemy_die.wav",   // SND_ENEMY_DIE
    "resources/enemy_shoot.wav", // SND_ENEMY_SHOOT
    "resources/boss_shoot.wav",  // SND_BOSS_SHOOT
    "resources/boss_phase.wav",  // SND_BOSS_PHASE
    "resources/boss_defeat.wav", // SND_BOSS_DEFEAT
    "resources/graze.wav"        // SND_GRAZE
};

void InitAudioSystem(void)
{
    // NOTE: InitAudioDevice() should be called in main before this
    
    for (int i = 0; i < MAX_SOUND_EVENTS; i++)
    {
        soundPool[i] = LoadSound(soundPaths[i]);
        
        // Validation (Raylib will log if it fails, but we can check here)
        if (soundPool[i].frameCount == 0)
        {
            // TraceLog(LOG_WARNING, "AUDIO: Failed to load sound [%s]", soundPaths[i]);
        }
    }
}

void UnloadAudioSystem(void)
{
    for (int i = 0; i < MAX_SOUND_EVENTS; i++)
    {
        if (soundPool[i].frameCount > 0)
        {
            UnloadSound(soundPool[i]);
        }
    }
}

void PlaySoundEvent(SoundEvent event)
{
    if (event >= 0 && event < MAX_SOUND_EVENTS)
    {
        if (soundPool[event].frameCount > 0)
        {
            // Play sound with slight pitch variation for more natural feel
            float pitch = (float)GetRandomValue(95, 105) / 100.0f;
            SetSoundPitch(soundPool[event], pitch);
            SetSoundVolume(soundPool[event], 0.4f);
            PlaySound(soundPool[event]);
        }
    }
}

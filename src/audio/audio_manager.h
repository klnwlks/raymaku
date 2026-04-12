#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "raylib.h"

// Sound events that can be triggered in the game
typedef enum SoundEvent {
    SND_SHOOT = 0,
    SND_HIT_ENEMY,
    SND_BOMB,
    SND_MENU_MOVE,
    SND_MENU_SELECT,
    SND_PLAYER_HIT,
    SND_ENEMY_DIE,
    MAX_SOUND_EVENTS
} SoundEvent;

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the audio system and load all sound assets
void InitAudioSystem(void);

// Unload all sound assets and clean up the audio system
void UnloadAudioSystem(void);

// Play the sound associated with a specific event
void PlaySoundEvent(SoundEvent event);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_MANAGER_H

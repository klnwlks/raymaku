# Bullet Pattern Specification

This document details the intended behavior and implementation strategies for advanced bullet patterns in the Raylib Bullet Hell Engine.

## 1. Homing Bullets
**Behavior**: Bullets that track the player's position, adjusting their trajectory over time.

### Implementation Strategy
- **Data Requirements**: The `Bullet` struct (in `bullet.h`) should be extended to include a `homingIntensity` (float) or a `turnRate`.
- **Logic**:
    - In `UpdateBulletPools`, for each active enemy bullet:
        1. Calculate the direction vector to the player: `Vector2Subtract(GetPlayer()->position, bullet->position)`.
        2. Normalize this vector.
        3. Determine the current angle of the bullet's velocity.
        4. Determine the target angle to the player using `atan2f`.
        5. Use a small increment (based on `turnRate * GetFrameTime()`) to shift the current velocity angle toward the target angle.
        6. Re-calculate the velocity vector: `cosf(newAngle) * speed`, `sinf(newAngle) * speed`.

## 2. Freeze and Aim
**Behavior**: Bullets that spawn, travel briefly, stop in place, re-target the player, and then fire again.

### Implementation Strategy
- **Data Requirements**: The `Bullet` struct needs a `state` enum (e.g., `BULLET_NORMAL`, `BULLET_FREEZING`, `BULLET_LOCKED`) and a `timer`.
- **Logic**:
    - **Phase 1 (Spawn)**: Bullet moves as normal. A timer counts down.
    - **Phase 2 (Freeze)**: When the timer hits zero, set `velocity` to `(0, 0)`. The bullet "freezes" for a set duration.
    - **Phase 3 (Re-aim)**: At the end of the freeze duration, calculate the new direction to the player using `AimPlayer(bullet->position)`.
    - **Phase 4 (Launch)**: Set a new `velocity` based on the calculated direction and an optional `acceleration`.

## 3. Multi-Phase Patterns
**Behavior**: Complex attacks that transition between different types of fire (e.g., a circle burst that turns into a spiral).

### Implementation Strategy
- **Architectural Approach**: This should be handled at the **Emitter/Enemy** level rather than inside the bullet logic.
- **Logic**:
    - Use a state machine in the enemy's update function (e.g., `screen_gameplay.c` or a dedicated enemy system).
    - **Phase 1**: Call `ExecPattern` with a `PatternConfig` set for a wide arc.
    - **Phase 2**: After a delay, call `ExecPattern` with a `PatternConfig` that has a high `spin` value and narrow `arc`.
    - Use the `GetTime()` or frame counting to trigger transitions between phases.

## 4. Curving / Spiral Patterns
**Behavior**: Bullets that naturally curve without explicitly homing.

### Implementation Strategy
- **Logic**:
    - Apply a perpendicular force to the velocity every frame.
    - Alternatively, rotate the velocity vector by a constant small angle every update:
      ```c
      // Example rotation logic (pseudocode)
      float angle = atan2f(v.y, v.x) + curveAmount * GetFrameTime();
      v.x = cosf(angle) * speed;
      v.y = sinf(angle) * speed;
      ```

## Implementation via Enums

To manage these behaviors cleanly in C, use Enums to define the bullet's nature and its current life-cycle phase.

### 1. Bullet Behavior Enum
Defines the movement logic applied to the bullet.
```c
typedef enum {
    BULLET_LINEAR,  // Standard straight-line movement
    BULLET_HOMING,  // Tracks player position
    BULLET_CURVING  // Constant angular velocity (spiral)
} BulletBehavior;
```

### 2. Bullet State Enum
Defines the current phase of the bullet, primarily for "Freeze and Aim" patterns.
```c
typedef enum {
    STATE_ACTIVE,   // Normal movement/update logic
    STATE_FROZEN,   // Movement halted, waiting for timer
    STATE_LOCKED    // Targeting complete, ready for launch
} BulletState;
```

### 3. Structural Integration
Extend the `Bullet` struct in `bullet.h`:
- `BulletBehavior behavior;`
- `BulletState state;`
- `float timer;` (Used for transition between states or behavior durations)

---

## 5. Implementation Status
The following bullet types have been implemented:
- **BULLET_LINEAR**: Standard straight-line movement using velocity and acceleration.
- **BULLET_HOMING**: Tracks the player's position, rotating its velocity vector toward the player at a set `rotationSpeed`.
- **BULLET_CURVING**: Constant angular rotation applied to the velocity vector.
- **BULLET_FREEZE**: State-based movement that moves, stops, re-aims at the player, and then launches as a linear bullet.

### Structural Updates
- `Bullet` struct extended with `behavior`, `state`, `timer`, `speed`, and `rotationSpeed`.
- `PatternConfig` extended to support these new behaviors and rotation speeds.
- `SpawnBullet` and `ExecPattern` updated to pass through these new parameters.

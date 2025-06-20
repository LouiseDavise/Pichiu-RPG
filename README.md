# 🐼 Pichiu RPG Shooter

A fast-paced, top-down **2D RPG Shooter** written in **C** using **Allegro 5**. In this game, you fight off enemies, switch weapons, collect coins, and reach the golden trophy — all while your stats are tracked and ranked globally.

🎥 **Watch Gameplay Demo**:  
[[Gameplay Demo]](https://youtu.be/fWRezf_m-Uc)

---

## 🎮 Features

### 🔐 Player System
- **New Player Entry**: Input your name to get a unique UID.
- **Returning Player**: Use UID to restore progress and view score history.
- **Persistent Save**: Player data stored in `Assets/player_uid.txt`.

### ⚔️ Gameplay Mechanics
- **Weapons**: Switch between machine gun and sniper (`1` and `2`).
- **Combat**: Real-time shooting, bullet physics, enemy HP, and knockback.
- **Enemies**: Includes Slimes and Super Slimes with pathfinding behavior.
- **Scoring System**: Combines kills, coins, time, and remaining HP.

### 🗺️ Stage & Map System
- **Multiple Stages**: With distinct map layouts and increasing difficulty.
- **Interactive Tiles**: Walls, healing potions, locked doors, and trophy.
- **Score Bonus**: Based on level, time, and health remaining.

### 📈 Leaderboard
- **Global Rankings**: Tracked in `Assets/leaderboard.txt`.
- **Leaderboard UI**: Paginated and accessible in menu and win/lose scenes.
- **Player History**: Returning players see personal score records.

### 🎛️ Settings Menu
- **Volume Controls**: BGM and SFX sliders with visual feedback.
- **Responsive UI**: Buttons with hover/click animations and sounds.

---

## 🕹️ Controls

| Input         | Action                         |
|---------------|--------------------------------|
| `W`, `A`, `S`, `D` | Move the player             |
| `Mouse`       | Aim & Shoot                    |
| `1` / `2`     | Switch weapon (Gun / Sniper)   |
| `ESC`         | Open Pause Menu                |

---

## 🛠️ Technologies Used

- **Language**: C (C99)
- **Graphics/Audio**: [Allegro 5](https://liballeg.org/)
- **File I/O**: For saving player UIDs, map data, and leaderboard scores
- **Data Files**:
  - `Assets/map_*.txt` – stage layouts
  - `Assets/player_uid.txt` – player records
  - `Assets/leaderboard.txt` – top scores

---

## 🧪 Build Instructions

### Requirements
- Allegro 5 development libraries (including font, image, primitives, audio, acodec)

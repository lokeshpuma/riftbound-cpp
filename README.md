# Riftbound: Echoes of Ember

[![Build and Test](https://github.com/lokeshpuma/riftbound-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/lokeshpuma/riftbound-cpp/actions/workflows/build.yml)
[![Deploy to GitHub Pages](https://github.com/lokeshpuma/riftbound-cpp/actions/workflows/deploy-pages.yml/badge.svg)](https://github.com/lokeshpuma/riftbound-cpp/actions/workflows/deploy-pages.yml)
[![Live Web Demo](https://img.shields.io/badge/Play%20Online-GitHub%20Pages-brightgreen)](https://lokeshpuma.github.io/riftbound-cpp/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![raylib](https://img.shields.io/badge/raylib-5.5%2B-red.svg)](https://www.raylib.com/)

A fast, responsive 2D side-scrolling action platformer built in modern **C++ (C++20)** using **raylib**. Developed with a shared gameplay codebase that compiles directly to a high-performance **native desktop executable** (macOS, Linux, Windows) as well as a **WebAssembly / HTML5 build** that deploys automatically to **GitHub Pages**.

---

## Table of Contents

- [1. How It Works](#1-how-it-works)
  - [Architecture Overview](#architecture-overview)
  - [Game Loop & Rendering](#game-loop--rendering)
  - [Physics & Collision Detection](#physics--collision-detection)
  - [Combat & Attack Mechanics](#combat--attack-mechanics)
  - [Enemy AI State Machines](#enemy-ai-state-machines)
  - [Boss Encounters: The Ashen Warden](#boss-encounters-the-ashen-warden)
  - [Procedural Audio & Visual Engine](#procedural-audio--visual-engine)
  - [State Machine & Persistence](#state-machine--persistence)
- [2. Tech Stack](#2-tech-stack)
- [3. Game Controls](#3-game-controls)
- [4. How to Install and Run on Your Computer](#4-how-to-install-and-run-on-your-computer)
  - [Prerequisites](#prerequisites)
  - [macOS](#macos-apple-silicon--intel)
  - [Ubuntu / Debian Linux](#ubuntu--debian-linux)
  - [Windows](#windows-msvc--visual-studio)
  - [Running Unit Tests](#running-unit-tests)
- [5. How to Deploy to GitHub Pages](#5-how-to-deploy-to-github-pages)
  - [Automated Deployment via GitHub Actions](#automated-deployment-via-github-actions)
  - [Enabling GitHub Pages in Your Repository](#enabling-github-pages-in-your-repository)
  - [Local Web Testing with Emscripten](#local-web-testing-with-emscripten)
- [6. Directory Structure](#6-directory-structure)
- [7. Assets & Licensing](#7-assets--licensing)

---

## 1. How It Works

### Architecture Overview
The game is written in clean, modular C++20 with strict separation between game simulation logic and rendering backends:

```
┌────────────────────────────────────────────────────────┐
│                        main.cpp                        │
│          (Native Loop / Emscripten Main Loop)          │
└───────────────────────────┬────────────────────────────┘
                            │
┌───────────────────────────▼────────────────────────────┐
│                       Game Engine                      │
│             GameStateId (State Machine)                │
└───────┬───────────────────┬───────────────────┬────────┘
        │                   │                   │
┌───────▼────────┐  ┌───────▼────────┐  ┌───────▼────────┐
│  PhysicsWorld  │  │ Level & Entity │  │  UI & Camera   │
│ AABB Collision │  │ Player, Enemy, │  │ HUD, Menus,    │
│ Sub-stepping   │  │ Boss, Hazards  │  │ Letterboxing   │
└────────────────┘  └────────────────┘  └────────────────┘
```

### Game Loop & Rendering
1. **Virtual Resolution & Letterboxing:** The entire game world renders to an internal $640 \times 360$ `RenderTexture2D` using point texture sampling. This guarantees pixel-perfect retro aesthetics.
2. **Aspect-Ratio Preserving Blit:** During the final pass, the internal buffer is scaled and drawn centered with black letterbox bars to fit any monitor resolution or browser canvas without distortion.
3. **Fixed Physics Updates:** Delta-time is clamped ($\le 0.05\text{s}$) to avoid simulation tunneling or spiraling during frame rate drops.

### Physics & Collision Detection
- **Axis-Separated AABB Resolution:** Horizontal ($X$) and vertical ($Y$) movement steps are resolved independently against level geometry colliders.
- **Tunneling Prevention via Sub-Stepping:** High velocities are subdivided into micro-steps ($8\text{px}$ maximum step length) so entities never pass through walls or floors even at high speeds.
- **Platformer Kinematics:**
  - Acceleration, deceleration, friction, gravity, and terminal fall speeds.
  - **Variable Jump Height:** Releasing the jump button early clamps upward velocity to minimum jump threshold.
  - **Coyote Time ($0.12\text{s}$):** Players can still jump for $120\text{ms}$ after walking off a platform ledge.
  - **Jump Buffering ($0.12\text{s}$):** Pressing jump slightly before touching the ground triggers a jump immediately upon landing.
- **One-Way Platforms:** Entities can jump upward through one-way ledges and land safely on top. Pressing `S + Space` (or `Down + Jump`) drops through one-way platforms.

### Combat & Attack Mechanics
- **Weapon A — Ember Blade (Melee):**
  - Generates an arcing hitbox sweep in front of the player.
  - Deals knockback and damage to enemies.
  - **Multi-Hit Prevention:** Tracks each swing's unique `attackId` so a single sword swipe hits each enemy only once.
- **Weapon B — Pulse Bolts (Ranged):**
  - Fires high-velocity energy projectiles with cyan particle trails.
  - Consumes Energy from a regenerating energy pool ($100$ max energy, $+15/\text{s}$ regen).
  - Deactivates with impact sparks upon hitting solid walls or enemy targets.
- **Damage & Invulnerability:** Taking damage triggers invulnerability frames with visual sprite blinking and directional knockback.

### Enemy AI State Machines
The game includes 3 distinct enemy archetypes:
1. **Patroller:** Crawls back and forth on platforms, checks ahead for cliff edges and walls before turning, and inflicts contact damage.
2. **Ranged Sentry:** Levitates with gentle sinusoidal bobbing, tracks player position within detection range ($280\text{px}$), charges up with an energy ring telegraph, and fires plasma bolts.
3. **Chaser:** Sleeps until the player enters its aggro radius ($190\text{px}$), charges aggressively, and leaps over obstacles to attack.
- **Loot Table:** Enemies have a drop table rewarding Health potions, Energy crystals, or Ember Cores on death.

### 5 Handcrafted Levels
- **Level 1 — Awakening Ruins:** Platforming tutorial, patrollers, pickups, and checkpoint.
- **Level 2 — The Sunken Spire:** Vertical ascension, spike hazard pits, one-way ledges, ranged sentries, and chasers.
- **Level 3 — Molten Catacombs:** Subterranean lava caverns, falling platform steps, timing hazards, chaser packs, and sentries.
- **Level 4 — The Skyward Ramparts:** High-altitude citadel with floating platforms, deep chasms, and aggressive sniper sentries.
- **Level 5 — The Ashen Crucible:** Advanced hazard gauntlet culminating in the locked Ashen Warden boss encounter.

### Boss Encounters: The Ashen Warden
Level 5 culminates in a locked arena encounter against **The Ashen Warden**:
- **Arena Gate:** Crossing into the arena triggers a boundary lockdown preventing retreat.
- **Phase 1 (100%–66% HP):** High-altitude ground slam with a visible telegraph marker on the floor, followed by a meteoric crash and bilateral ground shockwaves.
- **Phase 2 (66%–33% HP):** Multi-shot fiery spread volleys (3 to 5 projectiles).
- **Phase 3 (<33% HP):** Enraged high-speed flaming dash charges across the arena.
- **Vulnerability Windows:** After executing heavy attacks, the boss enters an exhausted `Recovery` state, allowing the player clear windows to counterattack.

### Procedural Audio & Visual Engine
- **Procedural Pixel Art:** In [AssetManager.cpp](src/core/AssetManager.cpp), character sprites (Hero, Patroller, Sentry, Chaser, Ashen Warden), tilesets, and effect animations are synthesized into textures in memory during engine boot.
- **Procedural Sound Synthesis:** In [AudioPlayer.cpp](src/core/AudioPlayer.cpp), footsteps, jumps, slashes, laser blasts, impacts, boss roars, and victory fanfares are generated using mathematical wave formulas (sine, square, triangle, white noise) directly into 16-bit PCM raylib `Wave` and `Sound` buffers.
- **No Missing Asset Errors:** The game requires zero external image or audio files to run, but can load overrides from `assets/` if provided.

### State Machine & Persistence
- **Game States:** `Boot`, `MainMenu`, `LevelSelect`, `Playing`, `Paused`, `Settings`, `GameOver`, `LevelComplete`, and `Victory`.
- **Persistence:** [SaveSystem.cpp](src/core/SaveSystem.cpp) serializes highest unlocked level, total score, collected cores, and audio volume to `riftbound_save.dat` on desktop, and syncs via IDBFS in browser builds.

---

## 2. Tech Stack

| Technology | Purpose |
|---|---|
| **C++20** | Core programming language (Modern C++ features, RAII, strongly typed enums, `std::unique_ptr`) |
| **raylib 5.5+** | Hardware-accelerated 2D rendering, audio output, window management, and input polling |
| **CMake 3.20+** | Cross-platform build configuration with `FetchContent` dependency management |
| **Emscripten** | Compiles C++ code and raylib into WebAssembly (`.wasm`) and JavaScript glue code |
| **CTest** | Automated unit testing framework for physics, combat, and progression validation |
| **GitHub Actions** | Automated CI/CD pipeline for building, running tests, and deploying to GitHub Pages |
| **GitHub Pages** | Static web hosting for the WebAssembly browser build |

---

## 3. Game Controls

| Action | Primary Keyboard | Alternate / Mouse |
|---|---|---|
| **Move Left** | `A` | `Left Arrow` |
| **Move Right** | `D` | `Right Arrow` |
| **Jump** (Tap for low, hold for high) | `Space` | `W` / `Up Arrow` |
| **Drop Through One-Way Ledge** | `S + Space` | `Down Arrow + Jump` |
| **Melee Attack (Ember Blade)** | `J` | `Left Mouse Button` |
| **Ranged Attack (Pulse Bolts)** | `K` | `Right Mouse Button` |
| **Switch Weapon** | `Q` | `1` / `2` |
| **Pause / Resume** | `Escape` | `P` |
| **Confirm / Select** | `Enter` | `E` / Mouse Click |
| **Toggle Debug Hitboxes** | `F1` | — |

---

## 4. How to Install and Run on Your Computer

### Prerequisites
- Modern C++20 compiler (`clang++`, `g++`, or MSVC)
- CMake 3.20 or newer
- Git

### macOS (Apple Silicon & Intel)

```bash
# 1. Clone the repository
git clone https://github.com/lokeshpuma/riftbound-cpp.git
cd riftbound-cpp

# 2. Optional: Install raylib via Homebrew (CMake fetches it automatically if omitted)
brew install raylib

# 3. Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4

# 4. Launch the game!
./build/riftbound
```

### Ubuntu / Debian Linux

```bash
# 1. Install required development libraries
sudo apt-get update
sudo apt-get install -y build-essential cmake git libasound2-dev libx11-dev \
    libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev

# 2. Clone the repository
git clone https://github.com/lokeshpuma/riftbound-cpp.git
cd riftbound-cpp

# 3. Configure, build, and run
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j $(nproc)
./build/riftbound
```

### Windows (MSVC / Visual Studio)

```powershell
# 1. Open 'Developer Command Prompt for VS' or PowerShell
git clone https://github.com/lokeshpuma/riftbound-cpp.git
cd riftbound-cpp

# 2. Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 3. Run the executable
.\build\Release\riftbound.exe
```

### Running Unit Tests

The test suite validates AABB collision logic, platforming resolution, damage cooldowns, weapon attack IDs, boss phase transitions, and progression persistence:

```bash
ctest --test-dir build --output-on-failure

# Or run the test binary directly:
./build/riftbound_tests
```

---

## 5. How to Deploy to GitHub Pages

### Automated Deployment via GitHub Actions
This repository includes a turnkey GitHub Actions workflow at [`.github/workflows/deploy-pages.yml`](.github/workflows/deploy-pages.yml). Whenever code is pushed to the `main` branch, the workflow:
1. Sets up the Emscripten toolchain (`emcc`).
2. Compiles raylib and the game into WebAssembly (`index.html`, `index.js`, `index.wasm`).
3. Packages the static web output into a GitHub Pages deployment artifact.
4. Publishes it directly to GitHub Pages.

### Enabling GitHub Pages in Your Repository
1. Push this project to your GitHub repository:
   ```bash
   git remote add origin https://github.com/lokeshpuma/riftbound-cpp.git
   git push -u origin main
   ```
2. On GitHub, navigate to **Settings** > **Pages** (under the "Code and automation" section).
3. Under **Build and deployment** > **Source**, change the dropdown from *Deploy from a branch* to **GitHub Actions**.
4. That's it! GitHub Actions will trigger automatically, and your game will be live at:
   **`https://lokeshpuma.github.io/riftbound-cpp/`**

### Local Web Testing with Emscripten
To test the WebAssembly build locally before deploying:

```bash
# 1. Activate Emscripten SDK
source /path/to/emsdk/emsdk_env.sh

# 2. Configure CMake for Web
emcmake cmake -B build-web -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF

# 3. Build the HTML5/WASM bundle
cmake --build build-web -j 4

# 4. Start a local HTTP server (browsers block WASM via file://)
cd build-web
python3 -m http.server 8080
```
Open **`http://localhost:8080/index.html`** in any modern web browser.

---

## 6. Directory Structure

```text
riftbound-cpp/
├── CMakeLists.txt              # Cross-platform CMake build system
├── README.md                   # Full documentation, guides, and controls
├── PRD_2D_Action_Platformer_CPP.md # Product Requirements Document
├── ASSETS.md                   # Asset provenance & licensing records
├── LICENSE                     # MIT License
├── .gitignore                  # Git ignore rules
├── .github/
│   └── workflows/
│       ├── build.yml           # CI build & automated CTest runner
│       └── deploy-pages.yml    # Emscripten WebAssembly GitHub Pages deployer
├── assets/                     # Optional audio, sprite, and level assets
├── include/
│   ├── core/                   # Config, Types, AssetManager, Input, SaveSystem, Audio, Particles
│   ├── physics/                # AABB, Collider, PhysicsWorld
│   ├── combat/                 # DamageInfo, Weapon, EmberBlade, PulseBolts, Projectile
│   ├── entities/               # Entity, Player, Enemy, Patroller, Sentry, Chaser, Boss
│   ├── level/                  # Tile, Level, LevelManager, CameraController, Collectible, Hazard, Checkpoint
│   ├── ui/                     # HUD, MenuManager, UIElements (Button, Slider)
│   └── game/                   # Game, GameStateId
├── src/                        # Implementations corresponding to include/
├── tests/                      # CTest suite (physics, combat, progression)
└── web/
    └── shell.html              # Custom Emscripten HTML shell with loading bar & responsive canvas
```

---

## 7. Assets & Licensing

- **Game Code:** Licensed under the [MIT License](LICENSE).
- **Visuals & Sounds:** Documented in [`ASSETS.md`](ASSETS.md). All visual sprites, particles, and audio sound effects are procedurally generated original assets licensed under the MIT License and fully compliant with open-source and commercial hosting.

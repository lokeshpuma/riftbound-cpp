# ASSETS.md — Riftbound: Echoes of Ember

This document records the origin, creator, and license terms for all visual, audio, and font assets utilized in *Riftbound: Echoes of Ember*, adhering to PRD specifications.

---

## 1. Visual Assets

### Runtime Procedural Sprites & Tiles
- **Description:** Character sprites (Player, Patroller, Ranged Sentry, Chaser, Ashen Warden boss), weapon slash animations, pulse projectiles, environmental tiles (stone platforms, hazards/spikes, one-way ledges), and collectible particles are synthesized programmatically at engine boot via custom software renderers and raylib `Image` / `Texture2D` generation routines.
- **Author:** Riftbound Project Contributors
- **License:** MIT License (Permissive open source)
- **Modifications:** Generated procedurally in code (`AssetManager.cpp`).

---

## 2. Audio Assets

### Runtime Procedural Sound Effects & Audio Synthesis
- **Description:** Sound effects (footsteps, jumping, sword slash, pulse bolt fire, impact/hit, player hurt, collectible pickup, checkpoint activation, boss roar, boss ground slam, death) and ambient music loops are synthesized procedurally using mathematical wave generators (sine, square, triangle, and frequency-modulated white noise waves converted to 16-bit PCM raylib `Wave` and `Sound` objects).
- **Author:** Riftbound Project Contributors
- **License:** MIT License (Permissive open source)
- **Modifications:** Pure code generation with fallback support for loading `.wav` / `.ogg` files placed in `assets/audio/`.

---

## 3. Fonts

### Default Raylib Bitmap & Vector Font
- **Description:** Built-in font provided by raylib for menus, HUD indicators, and damage popups.
- **Author:** Ramon Santamaria (@raysan5) and raylib contributors.
- **License:** zlib/libpng license (Permissive open source).
- **Source:** [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)

---

## 4. Third-Party Asset Policy
No copyrighted music, sprite rips, or unauthorized assets are incorporated in this project. Any future asset additions must provide verifiable public-domain (CC0) or MIT/BSD licensing recorded in this document before merging.

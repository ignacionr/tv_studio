# tv_studio

A simple 2D C++ game/simulation framework built on top of the SDL2 framework (using SDL2, SDL2_image, and SDL2_ttf).

---

## 🎯 Core Goal & Vision

The central goal of this repository is to construct a lightweight, educational C++ framework that enables **high-level, expressive formulation of relatively complex 2D scenes**. 

Instead of forcing developer-students to write redundant boilerplate, hardcode render-loops, or manually translate coordinates, the framework uses clean composition patterns (such as combining behavior updates with `operator+=`) and generic parametric programming (templates) to let students express sophisticated layers, animated characters, event routing, and AI steering behaviors in an elegant, declarative-like style.

---

## 🏛️ Engine Architecture

The framework is organized into three main layers: **SDL2 Wrapper Utilities**, the **Core Engine Architecture**, and the **Behavior/Steering** layer.

Below is a visualization of the architecture:

![TV Studio Architecture](rsrc/architecture.png)

### 1. SDL2 Wrappers (`src/sdladapters.h`)
* **`EventPump`**: Manages the main game loop (`SDL_WaitEventTimeout`) and routes input events to registered handlers.
* **`Window` & `Renderer`**: Encapsulate creation and lifetime management of `SDL_Window` and `SDL_Renderer`.
* **`Texture` & `Surface`**: RAII wrappers for image buffers and loaded sprites.
* **`Font`**: Manages TrueType fonts (`SDL2_ttf`) for text rendering.

### 2. Core Engine Components (`src/`)
* **`Scene`** ([scene.h](src/scene.h)): Composes a list of overlapping `Plane` layers of identical dimensions. Manages global simulation time (`age()`) and propagates tick updates and events downward.
* **`Plane`** ([plane.h](src/plane.h)): Represents a depth layer (Z-index) containing a background image and list of renderable entities.
* **`Character`** ([character.h](src/character.h)): Any renderable entity placed on a plane. Can contain a list of custom behavior lambdas (`_update`) combined using `operator+=` and an event listener callback (`_react`).
* **`Sprite`** ([sprite.h](src/sprite.h)): Binds to a `Character` to slice sprite sheets, maintain animation states (e.g. walk left/right), and perform rendering.
* **`Camera`** ([camera.h](src/camera.h)): Translates the `Scene` coordinates to the screen, updating the viewport scroll offsets.

### 3. Steering & Behaviors
* **`HMove`** ([move.h](src/move.h)): A horizontal movement behavior that increments/decrements a character's position over time using simulation metrics (`units::Speed`).
* **`Prosecution`** ([will/prosecution.h](src/will/prosecution.h)): A pursuit AI behavior that controls a character to track and steer towards a target coordinates.

---

## 🎬 Current Scene Implementation (`ForestScene`)

By default, the simulation executes `ForestScene` (defined in [forest.h](src/scenes/forest.h)), which implements:
1. **Parallax Background layers**: A multi-layered scenery consisting of background planes:
   - Plane 4: `rsrc/backgrounds/bg.png`
   - Plane 3: `rsrc/backgrounds/far_trees.png`
   - Plane 2: `rsrc/backgrounds/mid_trees.png`
   - Plane 0: `rsrc/backgrounds/close_trees.png`
2. **Steering AI Interaction**:
   - An static **Ice block** (`ice`) is placed on Plane 1.
   - A **Kanako character** (`girl`) is placed on Plane 2.
   - The `girl` is assigned a `Prosecution` steering script targeting the `ice` block. Every time the logic update ticks, it cancels her active move, registers a new `HMove` towards the ice block, and selects the matching walking animation (e.g., `walkLeft` or `walkRight`).

---

## 🛠️ Local Build (via Nix)

This repository includes a Nix flake development shell. To build and run:

### Build & Run directly:
```bash
nix run
```

### Alternatively, using CMake inside Nix Shell:
```bash
nix develop
# Then compile:
cmake -B build && cmake --build build
# Run the binary:
./build/tv_studio
```

---

## 🎨 Asset Credits
We are using the following assets for this demo. Kudos to the amazing creators:
* Demon Woods Parallax Background by [Aethrall](https://aethrall.itch.io)
* Kanako Platformer Character Sprite Set by [Maytch](https://maytch.itch.io)

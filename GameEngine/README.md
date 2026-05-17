# GameEngine

A small C++ OpenGL-based game engine demo with an immediate-mode UI (ImGui), basic lighting, model loading, and sample assets.

**Quick Intro**
- **What:** Lightweight engine used for experimenting with rendering, materials, and UI.
- **Language:** C++ (Visual Studio on Windows)
- **Renderer:** OpenGL shaders in `assets/shaders`

![Architecture](assets/docs/architecture.svg)

**Quick Start**
- **Build:** Open [GameEngine.vcxproj](GameEngine.vcxproj) in Visual Studio (x64). Build `Debug` or `Release`.
- **Run:** Executable appears under `x64/Debug/` or `x64/Release/` when built.

**Features**
- Basic PBR-like material support via `Material.h`
- Simple scene graph with `Entity.h` and `Transform.h`
- Shader examples under `assets/shaders/`
- Example primitive models and textures in `assets/`

**Assets**
- Shaders: `assets/shaders/`
- Models & MTL: `assets/shapes/primitives/`
- Textures: `assets/textures/`

**Screenshots — Engine Demos**
- **Scene 1 — Spotlight (cone):** ![Scene 1](assets/docs/scene_1.png)
  - Demonstrates a spotlight illuminating the scene (visualized as a cone).
- **Scene 2 — Point + Directional (sphere & square):** ![Scene 2](assets/docs/scene_2.png)
  - Demonstrates a point light (visualized as a sphere) together with a directional light (visualized as a square) and their combined shading effects.

**Contributing**
- Fork the repo, create a branch, and open a PR. Keep changes focused and include screenshots for visual changes.
**License & Contact**
- This repository and all included files are dedicated to the public domain using the **CC0 1.0 Universal** license. You may copy, modify, distribute, and perform the work, even for commercial purposes, all without asking permission.
- See the `LICENSE` file for the full text of the CC0 1.0 Universal dedication.
- For questions or discussion, open an issue.

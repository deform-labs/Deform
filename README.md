# Deform Engine
# I'm looking for contributors to help rewrite Deform in Rust. The engine already has a working C++ version. Now moving to Rust with winit, wgpu and egui.

> A custom-built Rust game engine focused on transformation and deformation systems.

Deform is built to explore low-level engine architecture and real-time object manipulation, demonstrating core concepts behind modern engines, system design, modular architecture, and transformation pipelines. The engine and editor are kept separate: the engine ships as a Windows DLL (`DEFORM_API`) and the editor links against it, keeping the core lean and the tooling flexible.

## Status 

The Rust rewrite is in progress. 
The engine currently has stubs for several systems. 
Check the cpp-version branch for the complete C++ version. Contributions welcome!

## Building

### Requirements

- Cargo

### Steps

1. Clone the repository
   ```bash
   git clone https://github.com/yydev-official/Deform.git
   cd Deform
   ```
2. Start building however you want!
---

## Roadmap

- [x] Engine/editor DLL split with `DEFORM_API`
- [x] Dear ImGui docking layout
- [x] Editor panel system (`Editor::UI`)
- [ ] Scene graph and entity system
- [ ] Asset pipeline (mesh, texture import)
- [x] Vulkan backend via RHI abstraction layer
- [ ] Scripting layer

---

## License

MIT — do whatever you want with it.

---

## Author

Made by [@yydev-official](https://github.com/yydev-official)

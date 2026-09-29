# 👻 Haunted Library

A real-time OpenGL application featuring a custom ghost model constructed from parametric Bézier surfaces, smooth per-vertex normal interpolation, dynamic Bézier curve trajectory generation ($C^1$ continuity), and quaternion-based orientation and roll dynamics[cite: 5].

Developed for **CENG 469: Computer Graphics II** at METU[cite: 5].

---

## 📖 Project Documentation & Technical Blog

Explore the full write-up, mathematical derivations, implementation challenges, and tessellation benchmarks:

**[Read the Full Blog Post](https://esatcivitci.github.io/haunted-library/)**

---

## Key Highlights

* **Parametric Geometry**: Watertight ghost mesh constructed from multiple bicubic Bézier patches with control point stitching[cite: 5].
* **Smooth Shading**: Per-vertex normal averaging across adjacent patch triangles for smooth specular and diffuse lighting[cite: 5].
* **Continuous Trajectory**: Runtime $C^1$-continuous cubic Bézier curve flight path constrained within the camera room bounds[cite: 5].
* **Quaternion Rotations**: Alternating roll motion around the gaze vector computed using unit quaternions without gimbal lock[cite: 5].
* **Interactive Controls**:
  * `Space` — Pause / unpause flight motion[cite: 5].
  * `W` — Toggle Wireframe / Solid rendering modes[cite: 5].
  * `P` — Toggle Bézier trajectory line visibility[cite: 5].

---

##

https://github.com/user-attachments/assets/427e1806-8276-434b-836b-e4a049b7cf5d

 How to Run

Compile and run with your desired tessellation resolution ($s \times t$)[cite: 5]:

```bash
make
./main 24

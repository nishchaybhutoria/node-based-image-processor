# Node-Based Image Processor 🎛️🖼️

A C++ desktop application that allows users to construct custom image processing pipelines using a visual, node-based interface — inspired by tools like Blender's Shader Editor and Substance Designer.

Built as part of the **Mixar Assignment**.

---

## 📌 Features

✅ Node-based editor for constructing visual image pipelines  
✅ Real-time image preview at each stage  
✅ Click-and-drag linking and link deletion (with ALT + click)  
✅ Drag-and-drop node creation via right-click  
✅ Error detection for invalid links and cycles  
✅ Topological sorting for correct evaluation  
✅ Preview and export final image  
✅ Parameter tuning in real-time from properties panel  
✅ Modern UI built using **Dear ImGui** and **ImNodes**

---

## 📦 Implemented Nodes

### 🔰 Basic Nodes
- **Input Node**
  - Load JPG, PNG, BMP
  - Show metadata (dimensions, file size, channels)
- **Output Node**
  - Save image with format & quality options
  - Preview final output
- **Brightness/Contrast Node**
  - Adjust brightness (-100 to +100)
  - Adjust contrast (0 to 3)
  - Reset buttons

### 💧 Blur
- **Blur Node**
  - Gaussian blur with radius control (1–20)
  - Optional directional mode
  - Reset radius

> More nodes were planned but not implemented due to time constraints (e.g., blend, threshold, edge detection, noise, convolution).

---

## 🛠 Build Instructions

### Prerequisites
- C++17 compiler (e.g., `g++`)
- OpenGL + GLFW
- OpenCV (`libopencv-dev`)
- CMake (optional)
- Fonts & image assets in `assets/`

### Compile

```bash
make
./demo_app
```

> Make sure the working directory is `src/`. Font should be at `../assets/Inter_18pt-Regular.ttf` and at least one image should be placed at `../assets/test.png`.

---

## 🧠 Architecture

- **Graph Class**: Handles node evaluation, link management, and topological sorting.
- **Node Base Class**: All nodes inherit and override `process`, `preview`, `renderPropertiesUI`, etc.
- **GUI**: Built using Dear ImGui + ImNodes for visual programming.
- **Evaluation**: Uses Kahn's algorithm to compute topological order and detect cycles.

---

## 📝 Notes

- Link removal: Hover link + ALT + click
- Cycles are detected and halt graph evaluation
- Each node has a unique `id * 1000 + portIndex` for handling connections
- Output is saved using OpenCV `imwrite`, supporting quality flags for JPG
- Full undo/redo or serialization is **not** implemented

---

## 📂 Directory Structure

```
node-based-image-processor/
├── assets/                # Fonts and test image
├── src/                   # Main application code
│   ├── core/              # Graph, Node base class
│   ├── nodes/             # All node implementations
│   ├── utils/             # Texture handling utilities
│   └── main.cpp           # Entry point
├── Makefile
└── README.md
```

---

## 🎥 Demo Video

📽️ [Watch the 3-min demo here](https://drive.google.com/file/d/1c1DbTIKx6AVjsxBx00OqZUzk0zUjFoeJ/view?usp=sharing)  
_Covers loading, connecting, previewing, and exporting an image pipeline._

---

## 🧾 Credits

- **Dear ImGui** — UI rendering
- **ImNodes** — Visual node editor
- **OpenCV** — Image processing backend
- **GLFW + OpenGL** — Graphics rendering
- **Icons8 / Google Fonts** — Interface assets

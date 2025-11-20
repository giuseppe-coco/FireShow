# FireShow - Pyrotechnic Display Design Software

![FireShow Demo](https://github.com/giuseppe-coco/FireShow/issues/1#issue-3648466122)

**FireShow** is a 3D fireworks display design and visualization software developed in C++ and OpenGL. Inspired by professional tools like FWsim and Finale 3D, this project allows designers to create, synchronize, and visualize complex pyrotechnic effects in a real-time 3D environment.

## ✨ Features

*   **Real-Time 3D Viewer:** A scene rendered with OpenGL
*   **Advanced Particle System:** Simulates the physics of various firework types, including cascading effects (trails) for enhanced realism.
*   **Pre-defined Firework Types:** Includes a library of classic effects such as Peonies, Chrysanthemums, Willows, and Volcanoes, ready to be used.
*   **Firework Editor:** An integrated user interface (built with Dear ImGui) that allows creating and modifying the properties of each firework type (color, velocity, particle count, etc.).
*   **Interactive Timeline:** A control panel for placing pyrotechnic events on a timeline, with full playback controls (play, pause, reset).
*   **Integrated Audio:** Each effect can be associated with launch and explosion sounds for a more immersive experience, powered by the `miniaudio` library.

## 🛠️ Tech Stack

*   **Language:** C++17
*   **Graphics:** OpenGL 3.3+
*   **Core Libraries:**
    *   **GLFW:** For window and input management.
    *   **GLAD:** For loading OpenGL functions.
    *   **GLM:** For graphics-related mathematical operations.
    *   **Dear ImGui:** For the graphical user interface.
    *   **stb_image:** For texture loading.
    *   **miniaudio:** For audio playback.
*   **Build System:** `make` (with `g++`)

---

## 🚀 How to Run Locally (Windows Guide)

This project uses `make` and `g++`. The easiest way to get these tools on Windows is by using **MinGW-w64** (distributed via MSYS2).

### Prerequisites

1.  **Install MSYS2:**
    *   Download and install MSYS2 from the [official website](https://www.msys2.org/).
    *   Follow the instructions to update the base system (usually running `pacman -Syu` followed by `pacman -Su`).

2.  **Install the Development Toolchain:**
    *   Open an MSYS2 UCRT64 (or MINGW64) terminal.
    *   Install the `g++` compiler, `make`, and `git` with the following command:
        ```bash
        pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain git
        ```

3.  **Download GLFW:**
    *   This project requires the pre-compiled GLFW libraries.
    *   Go to the [GLFW download page](https://www.glfw.org/download.html) and download the **"64-bit Windows binaries"** for MinGW-w64.
    *   Extract the zip file.

### Project Setup

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
    cd YOUR_REPOSITORY
    ```

2.  **Configure Dependencies (GLFW):**
    *   Inside the project's root folder, create the following directories: `libs/glfw/include` and `libs/glfw/lib`.
    *   From the downloaded GLFW files, copy the `GLFW` folder (which contains `glfw3.h`) into `libs/glfw/include/`.
    *   From the downloaded `lib-mingw-w64` folder (or similar), copy the files `libglfw3.a` and `libglfw3dll.a` into `libs/glfw/lib/`.

    The final folder structure for the dependencies should look like this:
    ```
    /YOUR_REPOSITORY
    |-- /libs
    |   |-- /GLFW
    |       |-- /include
    |       |   |-- /GLFW
    |       |       |-- glfw3.h
    |       |-- /lib
    |           |-- libglfw3.a
    |   |-- /glad
    |       .......
    |..............
    |-- /src
    |-- /vendors
    |-- Makefile
    ...
    ```

### Compilation and Execution

1.  **Compile the Project:**
    *   Make sure you are in the root directory of the repository within your MSYS2 terminal.
    *   Run the `make` command:
        ```bash
        make
        ```
    *   This will compile all source files and create the final executable `FireworksSimulator.exe` in the `bin/` directory.

2.  **Run the Software:**
    *   Execute the program with:
        ```bash
        ./bin/FireworksSimulator.exe
        ```

### Cleaning Up

To remove all generated build files (objects and the executable), run:
```bash
make clean
```

# Quoridor 

A complete, feature-rich C++ implementation of the classic strategy board game **Quoridor**, built using the **SFML 3** graphics library. 

This project features a fully interactive graphical interface, custom UI elements, an undo/history system, and built-in AI opponents ranging from easy to hard.

## Features

* **Multiplayer Modes:** Play locally with 2 or 4 players.
* **AI Opponents:** Play against the computer with three distinct difficulty levels:
  * *Easy:* Random valid moves and wall placements.
  * *Medium:* A mix of easy and hard strategies.
  * *Hard:* Evaluates shortest paths using a BFS (Breadth-First Search/Lee algorithm) to block opponents optimally while advancing to the goal.
* **Customization:** Personalize each player's name and pawn color before starting a match.
* **Interactive Match History:** Scroll through the match history using a custom-built scrollbar and undo moves if you make a mistake.
* **Audio System:** Built-in background music with an interactive visual volume slider.
* **Smart Board Logic:** Prevents players from entirely blocking the path to the finish line, ensuring every game remains winnable.

## How to Play

### The Goal
Reach the opposite side of the board before your opponent(s). 

### Controls
* **Left-Click:** * Move your pawn to an adjacent, highlighted cell.
  * Interact with menus, buttons, and volume sliders.
  * Click the "Undo" button to revert to a previous turn.
* **Right-Click:** * Place a wall between cells to block opponents. (Click on the gap between cells; horizontal gaps place horizontal walls, vertical gaps place vertical walls).
* **Scroll Wheel / Click & Drag:** Scroll through the match history.
* **ESC:** Return to the main menu.

## Prerequisites

To compile and run this game, you will need:

1. A **C++17** compatible compiler (GCC, Clang, or MSVC).
2. **CMake** (version 4.1.2 or higher).
3. **SFML 3** (Graphics, Window, System, and Audio modules).

## Building and Running

This project uses CMake for building. By default, the `CMakeLists.txt` file is configured to copy the `assets` folder into your build directory automatically.

1. **Clone or download the repository.**
```bash
git clone https://github.com/vlxdnt/Quoridor.git
```
2. **Open a terminal** and navigate to the project's root directory.
3. **Create a build folder and navigate into it:**
```bash
mkdir build
cd build
```


4. **Generate the build files:**
```bash
cmake ..
```


5. **Compile the game:**
```bash
cmake --build .
```


6. **Run the executable:**
* **Windows:** `.\Quoridor.exe`
* **Linux/macOS:** `./Quoridor`

## License

This project is licensed under the **MIT License** .

See the [LICENSE](LICENSE) file for more details.
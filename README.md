# Runic: Image & Fractal Engine

## Overview
Runic is a high-performance C-based engine designed for procedural fractal generation and advanced image manipulation. The project implements a complete pipeline from parsing recursive grammar systems to rendering high-resolution binary images.

## Key Modules
- **L-System Parser:** Processes Lindenmayer systems to generate complex self-similar structures (fractals) from production rules.
- **Turtle Graphics Engine:** A custom implementation of the Turtle Graphics paradigm, using **Bresenham's Line Algorithm** to render paths with integer-only arithmetic for maximum efficiency.
- **PPM Image Processor:** Handles binary P6 PPM files, supporting transformations and direct pixel manipulation.
- **Font Renderer:** Parses BDF (Glyph Bitmap Distribution Format) files to allow text overlays on generated graphics.
- **BitCheck Integrity:** A specialized bitwise analysis tool to detect specific data patterns and corruption in binary streams.

## Technical Implementation
- **Memory Management:** Zero memory leaks, validated with Valgrind. Use of dynamic allocation for variable-length fractal derivations.
- **Mathematics:** Precise geometric calculations for rotations and scaling within a discrete pixel grid.
- **Architecture:** Modular design with separated concerns for parsing, logic, and rendering.

## Getting Started
### Prerequisites
- GCC Compiler
- GNU Make
- A PPM viewer (like GIMP or Feh)

### Installation & Execution
1. Clone the repository:
   ```bash
   git clone [https://github.com/stefan-popovici/Runic-Image-Engine.git](https://github.com/stefan-popovici/Runic-Image-Engine.git)
2. Compile the engine:
   ```bash
   make build
3. Run with an input script:
   ```bash
   ./runic < examples/input.text

Author
**Stefan Popovici** - *National University of Science and Technology Politehnica Bucharest (UNSTPB)*
*Faculty of Automatic Control and Computers, Computer Science and Engineering (CTI)*

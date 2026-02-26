
# CPFlappyBird

Flappy Bird ported for the Classpad II fx-CP400.

## Controls
 - Up key to jump
 - Clear/Power key to end the game
 - EXE key to restart

## Installation

1. Download the latest release (artifact) from the Actions tab or Releases page.
2. Extract the bundle.
3. Copy the `CPFlappyBird.hh3` file to your calculator's storage (e.g., via USB mass storage).
4. Copy the `usr` folder to the root of your calculator's storage, merging it with any existing `usr` folder. This installs the necessary fonts and textures.
5. Launch `CPFlappyBird.hh3` on your calculator.

## Building from Source

To build the project yourself:

1. Ensure you have the [Hollyhock-3 SDK](https://github.com/ClasspadDev/hollyhock-3) environment set up (e.g., via Docker or devcontainer).
2. Install Python 3 and Pillow: `pip install Pillow`.
3. Generate resources:
   ```bash
   python3 convert_textures.py
   python3 convert_fonts.py
   ```
4. Compile the project:
   ```bash
   make
   ```

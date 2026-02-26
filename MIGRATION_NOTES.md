# Lessons Learned & Notes for Future HH3 (V3) Ports

This document summarizes key insights and common pitfalls encountered during the migration from Hollyhock V2 (HHK) to Hollyhock V3 (HH3).

## 1. Project Structure & Build System
*   **Source Directory**: It is highly recommended to move all source code (`.cpp`, `.c`, `.S`, `.hpp`, `.h`) into a `src/` directory. This keeps the root clean and simplifies the `Makefile`.
*   **Makefile**: Start with a fresh V3-compatible `Makefile` (like the one in `HHK3template`). Ensure it uses the `sh4a_nofpueb-elf-*` toolchain and includes flags like `-gdwarf-5`, `-flto`, and `-O2`.
*   **Linker Script**: The `linker.ld` file is no longer needed and should be removed. The SDK handles linking automatically.
*   **Assembly Files**: Rename `.s` files to `.S` (uppercase) if they rely on C preprocessor directives, or update the Makefile to handle `.s` files correctly.

## 2. Main Entry Point
*   **Signature**: The main function must be `int main() { ... return 0; }`.
*   **Linkage**: Do **NOT** wrap `main` in `extern "C"`. It requires C++ linkage in the V3 SDK environment.
    ```cpp
    // Correct
    int main() {
        return 0;
    }
    ```

## 3. Input Handling
*   **Event Loop**: The old polling-based `GetKey` function is deprecated. Use `GetInput` in a loop.
*   **Non-Blocking**: For real-time apps (games), call `GetInput(&event, 0, 0x10)`. The second argument is the timeout; using `0` ensures non-blocking polling, preventing the game loop from freezing.
*   **Key Mapping**: V3 uses specific `KEYCODE_*` enums (defined in `<sdk/os/input.h>`). You may need to map legacy `KEY_*` constants to these new values.
    *   Example: `KEY_CLEAR` -> `KEYCODE_POWER_CLEAR`, `KEY_MULTIPLY` -> `KEYCODE_TIMES`.
*   **Sparse Keycodes**: V3 keycodes are not sequential and can be large (e.g., `0xEE10`). Avoid using keycodes as direct indices for small arrays. Iterate through active listeners or use a hash map/sparse array approach.

## 4. Graphics & VRAM
*   **Direct Access**: Use `LCD_GetVRAMAddress()` to get a pointer to the frame buffer. `LCD_VRAMBackup()` and `LCD_VRAMRestore()` are obsolete and should be removed.
*   **Global Variables**: If your app relies on global `width` and `height` variables that were previously provided by a template, you must define them globally in your implementation (e.g., in `calc.cpp`) and initialize them in `main` using `LCD_GetSize(&width, &height)`.
*   **Function Conflicts**: The SDK defines `LCD_ClearScreen` as a function pointer. If you have a custom inline function with the same name, wrap it in `#ifdef PC` or rename it to avoid redeclaration errors.

## 5. File I/O
*   **Standard Library**: Refactor code to use standard C library functions (`fopen`, `fread`, `fseek`, `fclose`) provided by newlib, instead of raw SDK calls like `open` or `read`.
*   **Paths**: Ensure paths are correct. The `bundle` workflow creates a `/usr/` directory structure.

## 6. Debugging
*   **Format Strings**: `Debug_Printf` is strict about format specifiers. Always cast variables to match the specifier to avoid `-Werror=format` failures.
    *   Example: `Debug_Printf(..., "%d", (int)my_uint32_var);`
*   **Artifacts**: Configure CI to upload `.elf` and `.map` files as debug artifacts. These are crucial for diagnosing crashes on hardware.

## 7. CI/CD & Bundling
*   **Resource Generation**: Automate texture and font conversion in the CI pipeline using Python.
*   **Bundling**: Create a "bundle" artifact that mimics the calculator's file system (e.g., `CPFlappyBird.hh3` alongside a `usr/` folder). This simplifies installation for end-users.

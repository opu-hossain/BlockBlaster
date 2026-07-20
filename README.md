![Alt text for your image](https://i.ibb.co/t3RK0Gf/Screenshot-2024-07-13-at-6-23-51-PM.png)

<div align="center">
  <img src="https://i.imgur.com/9xXbkxt.jpeg", width="250" />
  <img src="https://i.imgur.com/u7LlFWk.jpeg" width="250" />
  <img src="https://i.imgur.com/pUUEM1p.jpeg" width="250" />
</div>

# BlockBlaster

BlockBlaster is a classic breakout-style game built using Raylib. The objective is to break all the blocks by bouncing a ball with a paddle. This project demonstrates the use of graphics libraries in C to create a fun and interactive game.

## Features

- Classic breakout gameplay
- Simple and intuitive controls
- Graphics powered by Raylib

## Requirements

- C Compiler (GCC, Clang, or MSVC)
- CMake (4.3+)
- Git (needed to fetch Raylib during the build)

Raylib itself does **not** need to be installed manually — CMake fetches and builds it from source automatically as part of the build process.

## Building the Project

1. Clone the repository:

```sh
git clone https://github.com/yourusername/BlockBlaster.git
cd BlockBlaster
```

2. Configure and build with CMake:

```sh
cmake -B build
cmake --build build
```

The first build will take a few minutes, since it downloads and compiles Raylib from source. Subsequent builds are much faster.

3. Run the executable:

**Linux/macOS:**
```sh
./build/BlockBlaster
```

**Windows:**
```powershell
.\build\Debug\BlockBlaster.exe
```
> **Note:** This game is not release-ready yet — expect bugs and incomplete features.

## Gameplay Instructions

- Use the left and right arrow keys to move the paddle.
- Break all the blocks by bouncing the ball with the paddle.
- Try not to let the ball fall off the bottom of the screen!

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.

## Acknowledgments

- Thanks to the Raylib developers for their excellent library.
- Inspired by the classic arcade game Breakout.

## Contact

For any questions or suggestions, please open an issue or contact me at arahaman2330455@bscse.uiu.ac.bd

---

Enjoy playing BlockBlaster!

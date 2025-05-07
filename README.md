# 🎲 Ludo Game

<div align="center">

![C++](https://img.shields.io/badge/C++-11-blue.svg)
![SFML](https://img.shields.io/badge/SFML-2.5.1-green.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)

A modern implementation of the classic Ludo board game using C++ and SFML.

![Game Screenshot](images/window-1.jpg)

</div>

## 📋 Table of Contents

- [Features](#-features)
- [Prerequisites](#-prerequisites)
- [Installation](#-installation)
- [Usage](#-usage)
- [Game Rules](#-game-rules)
- [Project Structure](#-project-structure)
- [Technical Details](#-technical-details)
- [Contributing](#-contributing)
- [License](#-license)
- [Contact](#-contact)

## ✨ Features

- 🎮 **Multiplayer Support**: Play with up to 4 players
- 🎲 **Interactive UI**: Modern graphical interface using SFML
- 🔄 **Real-time Gameplay**: Smooth animations and responsive controls
- 🎯 **Advanced Mechanics**:
  - Safe squares to protect tokens
  - Home column entry requirements
  - Kill counter tracking
  - Home counter tracking
- 🎨 **Custom Assets**: Beautiful game pieces and board design
- 🔒 **Thread Safety**: Multi-threaded player management using POSIX threads

## 🛠️ Prerequisites

Before you begin, ensure you have the following installed:

- C++11 compatible compiler (g++ recommended)
- SFML library (version 2.5.1 or higher)
- POSIX threads (pthreads)
- Make build system

## 📥 Installation

### Windows

1. Install MinGW or Visual Studio
2. Download SFML from [official website](https://www.sfml-dev.org/download.php)
3. Set up environment variables for SFML

### Linux

<<<<<<< HEAD
```bash
sudo apt-get update
sudo apt-get install g++ make libsfml-dev
```


### macOS

```bash
brew install sfml
```

## 🚀 Usage

1. Clone the repository:

```bash
git clone https://github.com/YourUsername/Ludo-Game-CPP.git
cd Ludo-Game-CPP
```

2. Build the project:

```bash
make
```

3. Run the game:

```bash
make run
```

## 🎯 Game Rules

1. **Starting the Game**:

   - Each player has 4 tokens
   - Players take turns rolling a dice
   - A 6 is required to move a token from the starting yard

2. **Movement Rules**:

   - Roll a 6 to move a token from the yard
   - Move tokens clockwise around the board
   - Capture opponent's tokens by landing on them
   - Safe squares protect tokens from capture

3. **Home Column Rules**:
   - Must kill at least one opponent to enter home column
   - Must land exactly on the final position
   - First player to get all tokens home wins

## 📁 Project Structure

```
Ludo-Game-CPP/
├── main.cpp           # Main game loop and window management
├── Player.cpp         # Player class and game logic
├── global.cpp         # Global variables and configuration
├── Makefile          # Build configuration
├── fonts/            # Game fonts
│   ├── Montserrat-Bold.ttf
│   └── ...
└── images/           # Game assets
    ├── dice.png
    ├── tokens/
    └── ...
```

## 🔧 Technical Details

### Key Components

- **Player Class**: Manages player state, token movement, and game rules
- **Game Board**: 52 main squares with home columns and safe squares
- **Thread Management**: POSIX threads for player turns
- **Graphics**: SFML for rendering and window management

### Synchronization

- Mutex locks for thread safety
- Condition variables for turn management
- Atomic operations for game state updates

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📧 Contact

Your Name - [@YourTwitter](https://twitter.com/YourTwitter) - email@example.com

Project Link: [https://github.com/YourUsername/Ludo-Game-CPP](https://github.com/YourUsername/Ludo-Game-CPP)

---

⭐️ From [YourUsername](https://github.com/YourUsername)

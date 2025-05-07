# Ludo Board Game

A C++ implementation of the classic Ludo board game using SFML for graphics and POSIX threads for player management.

## Features

- Multiplayer Ludo game (up to 4 players)
- Graphical user interface using SFML
- Multi-threaded player management
- Custom game assets and fonts
- Real-time dice rolling and token movement
- Safe squares and home column mechanics
- Kill counter and home counter tracking

## Prerequisites

- C++11 compatible compiler (g++ recommended)
- SFML library
- POSIX threads (pthreads)
- Make

## Building the Project

1. Install SFML:

   - On Windows: Download from [SFML website](https://www.sfml-dev.org/download.php)
   - On Linux: `sudo apt-get install libsfml-dev`
   - On macOS: `brew install sfml`

2. Clone the repository:

   ```bash
   git clone https://github.com/AbdullahAsif296/Ludo-Game-CPP.git
   cd Ludo-Game-CPP
   ```

3. Build the project:

   ```bash
   make
   ```

4. Run the game:
   ```bash
   make run
   ```

## Project Structure

- `main.cpp`: Main game loop and window management
- `Player.cpp`: Player class and game logic
- `global.cpp`: Global variables and game configuration
- `Makefile`: Build configuration
- `fonts/`: Game fonts
- `images/`: Game assets (tokens, dice, board)

## Game Rules

- Players take turns rolling a dice
- A 6 is required to move a token from the starting yard
- Tokens must kill at least one opponent to enter the home column
- Safe squares prevent token capture
- First player to get all tokens home wins

## Contributing

Feel free to submit issues and enhancement requests!

## License

[Add your chosen license here]

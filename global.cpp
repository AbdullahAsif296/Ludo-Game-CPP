#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Game board components
sf::RectangleShape playGroundGrids[52];
sf::RectangleShape greenHome[5];
sf::RectangleShape redHome[5];
sf::RectangleShape blueHome[5];
sf::RectangleShape yellowHome[5];
sf::RectangleShape DICE(sf::Vector2f(150, 150));
sf::Texture shuffleDiceTexture;
sf::RectangleShape playerturn(sf::Vector2f(90, 90));
sf::RenderWindow mainWindow;

// Text elements for player names and developers
sf::Text Name1;
sf::Text Name2;
sf::Text Name3;
sf::Text Devs;

// Array of home positions for each color
sf::RectangleShape *playerHomes[4] = {redHome, greenHome, blueHome, yellowHome};

// Game symbols
sf::RectangleShape heartSym(sf::Vector2f(200, 200));
sf::RectangleShape spadeSym(sf::Vector2f(200, 200));
sf::RectangleShape clubSym(sf::Vector2f(200, 200));
sf::RectangleShape diamondSym(sf::Vector2f(200, 200));
sf::RectangleShape SymbolArray[4] = {heartSym, spadeSym, clubSym, diamondSym};

// Safe square textures
sf::Texture greenstop;
sf::Texture redstop;
sf::Texture yellowstop;
sf::Texture bluestop;

// Fonts
sf::Font mainFont;
sf::Font secFont;

// Game state variables
int NO_PLAYERS;
int NO_TOKENS;
int Cycle = 0;
bool gameRunning = true;

// Thread synchronization
pthread_mutex_t turnMutex = PTHREAD_MUTEX_INITIALIZER;

// Safe squares positions
const std::vector<int> SAFE_SQUARES = {1, 9, 14, 22, 27, 35, 40, 48};

// Starting positions for each color
const int START_POSITIONS[4] = {40, 1, 27, 14}; // Red, Green, Blue, Yellow

// Structure to hold game configuration
struct GameConfig
{
    static const int MAX_PLAYERS = 4;
    static const int MAX_TOKENS = 4;
    static const int BOARD_SIZE = 52;
    static const int HOME_SQUARES = 5;
    static const int HOME_ENTRY_DISTANCE = 50;
    static const int HOME_FINAL_DISTANCE = 56;
};

// Add these with the other game components
sf::RectangleShape exitButton(sf::Vector2f(110, 60));
sf::RectangleShape restartButton(sf::Vector2f(50, 50));

// Add these with the other text elements
sf::Text exitText;
sf::Text restartText;

// Kill counter components
sf::RectangleShape killCounterBoxes[4]; // Red, Blue, Yellow, Green boxes
sf::Text killCounterTexts[4];           // Kill count numbers
sf::Text killCounterTitle;              // "KILLS" title

// Reached home counter components
sf::RectangleShape homeCounterBoxes[4]; // Red, Blue, Yellow, Green boxes
sf::Text homeCounterTexts[4];           // Home count numbers
sf::Text homeCounterTitle;              // "HOME" title

// Button textures
sf::Texture exitButtonTexture;
sf::Texture restartButtonTexture;
// Add with other game state variables
// Add with other thread-related variables
pthread_t masterThreadId;

// Add with other thread synchronization variables in global.cpp
pthread_mutex_t gridMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t turnCond = PTHREAD_COND_INITIALIZER;
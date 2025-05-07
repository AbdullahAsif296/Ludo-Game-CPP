#include <SFML/Graphics.hpp>
#include <iostream>
#include <bits/stdc++.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include "Player.cpp"
#include "global.cpp"
using namespace std;
// Add with other external declarations
extern pthread_t masterThreadId;
Player *PLAYERS;

void inputWindow();
void drawHomeGrids(sf::RectangleShape *homeGrid, string color);
void initialize();
void *turn(void *arg);
void *MasterThread(void *);
void fillDiceTexture(int val);
void fillOnBoard();
void killCheck(int currentPlayer);
void printDevNames();
void cleanup();
bool showExitConfirmation();
bool showRestartConfirmation();
void cleanupThreads();
void resetGameState();
void restartGame();
void exitGame();
void initializeSynchronization();
int main()
{
    srand(time(0));

    // Load resources
    if (!mainFont.loadFromFile("fonts/Montserrat-Bold.ttf") ||
        !secFont.loadFromFile("fonts/Montserrat-Regular.ttf") ||
        !shuffleDiceTexture.loadFromFile("images/dice.png"))
    {
        cout << "Error loading fonts" << endl;
        return -1;
    }
    // Initialize synchronization primitives
    initializeSynchronization();
    inputWindow();
    initialize();
    // printDevNames();

    // Create main game window
    mainWindow.create(sf::VideoMode(1100, 750), "Ludo Board Game");
    mainWindow.setPosition(sf::Vector2i(
        sf::VideoMode::getDesktopMode().width / 2 - mainWindow.getSize().x / 2,
        sf::VideoMode::getDesktopMode().height / 2 - mainWindow.getSize().y / 2));

    // Load button textures
    if (!exitButtonTexture.loadFromFile("images/exit-button.png") ||
        !restartButtonTexture.loadFromFile("images/reloading-button.png"))
    {
        cout << "Error loading button textures" << endl;
        return -1;
    }

    // Apply textures to buttons
    exitButton.setTexture(&exitButtonTexture);
    restartButton.setTexture(&restartButtonTexture);

    // Set up buttons
    exitButton.setPosition(870, 660); // Top right corner above the dice
    exitButton.setOutlineThickness(0);

    restartButton.setPosition(770, 20); // Top left corner of the dice
    restartButton.setOutlineThickness(0);

    // Set up game board areas
    sf::RectangleShape areas[4] = {
        sf::RectangleShape(sf::Vector2f(300, 300)), // Red
        sf::RectangleShape(sf::Vector2f(300, 300)), // Green
        sf::RectangleShape(sf::Vector2f(300, 300)), // Blue
        sf::RectangleShape(sf::Vector2f(300, 300))  // Yellow
    };

    // Position areas
    areas[0].setPosition(0, 0);     // Red
    areas[1].setPosition(450, 0);   // Green
    areas[2].setPosition(0, 450);   // Blue
    areas[3].setPosition(450, 450); // Yellow

    // Set area colors
    areas[0].setFillColor(sf::Color(228, 28, 36, 255));  // Red
    areas[1].setFillColor(sf::Color(98, 180, 70, 255));  // Green
    areas[2].setFillColor(sf::Color(30, 112, 185, 255)); // Blue
    areas[3].setFillColor(sf::Color(254, 205, 7, 255));  // Yellow

    // Create center areas (white)
    sf::RectangleShape centerAreas[4];
    for (int i = 0; i < 4; i++)
    {
        centerAreas[i].setSize(sf::Vector2f(200, 200));
        centerAreas[i].setFillColor(sf::Color::White);
    }

    // Position center areas
    centerAreas[0].setPosition(50, 50);   // Red center
    centerAreas[1].setPosition(500, 50);  // Green center
    centerAreas[2].setPosition(50, 500);  // Blue center
    centerAreas[3].setPosition(500, 500); // Yellow center

    // Initialize token layouts
    vector<sf::RectangleShape> tokens[4];
    vector<sf::Vector2f> tokenPositions[4] = {
        // Red token positions
        {{50, 50}, {200, 50}, {50, 200}, {200, 200}},
        // Green token positions
        {{500, 50}, {650, 50}, {500, 200}, {650, 200}},
        // Blue token positions
        {{50, 500}, {200, 500}, {50, 650}, {200, 650}},
        // Yellow token positions
        {{500, 500}, {650, 500}, {500, 650}, {650, 650}}};

    // Create tokens
    for (int color = 0; color < 4; color++)
    {
        for (int t = 0; t < NO_TOKENS; t++)
        {
            sf::RectangleShape token(sf::Vector2f(50, 50));
            token.setPosition(tokenPositions[color][t]);
            token.setTexture(&PLAYERS[color].tokenTexture);
            tokens[color].push_back(token);
        }
    }

    // Set up dice and turn indicator
    DICE.setFillColor(sf::Color::White);
    DICE.setPosition(850, 50);
    playerturn.setPosition(880, 250);

    // Set up center area
    sf::RectangleShape center(sf::Vector2f(150, 150));
    sf::Texture centerTexture;
    if (!centerTexture.loadFromFile("images/center.png"))
    {
        cout << "Error loading center texture" << endl;
        return -1;
    }
    center.setTexture(&centerTexture);
    center.setPosition(300, 300);

    // Initialize game board grids
    for (int i = 0; i < GameConfig::BOARD_SIZE; ++i)
    {
        playGroundGrids[i].setSize(sf::Vector2f(50, 50));
        playGroundGrids[i].setOutlineColor(sf::Color::Black);
        playGroundGrids[i].setOutlineThickness(2);
        playGroundGrids[i].setFillColor(sf::Color::White);

        // Position grids based on index
        if (i < 6)
        {
            playGroundGrids[i].setPosition(400, i * 50);
        }
        else if (i < 12)
        {
            playGroundGrids[i].setPosition(450 + ((i - 6) * 50), 300);
        }
        else if (i == 12)
        {
            playGroundGrids[i].setPosition(700, 350);
        }
        else if (i < 19)
        {
            playGroundGrids[i].setPosition(750 - (i - 12) * 50, 400);
        }
        else if (i < 25)
        {
            playGroundGrids[i].setPosition(400, 400 + (i - 18) * 50);
        }
        else if (i == 25)
        {
            playGroundGrids[i].setPosition(350, 700);
        }
        else if (i < 32)
        {
            playGroundGrids[i].setPosition(300, 750 - (i - 25) * 50);
        }
        else if (i < 38)
        {
            playGroundGrids[i].setPosition(300 - (i - 31) * 50, 400);
        }
        else if (i == 38)
        {
            playGroundGrids[i].setPosition(0, 350);
        }
        else if (i < 45)
        {
            playGroundGrids[i].setPosition(-50 + (i - 38) * 50, 300);
        }
        else if (i < 51)
        {
            playGroundGrids[i].setPosition(300, 250 - (i - 45) * 50);
        }
        else if (i == 51)
        {
            playGroundGrids[i].setPosition(350, 0);
        }
    }

    // Load and set safe square textures
    if (!greenstop.loadFromFile("images/greenstop.gif") ||
        !redstop.loadFromFile("images/redstop.gif") ||
        !yellowstop.loadFromFile("images/yellowstop.gif") ||
        !bluestop.loadFromFile("images/bluestop.gif"))
    {
        cout << "Error loading stop textures" << endl;
        return -1;
    }

    // Set safe squares
    playGroundGrids[1].setTexture(&greenstop);
    playGroundGrids[40].setTexture(&redstop);
    playGroundGrids[14].setTexture(&yellowstop);
    playGroundGrids[27].setTexture(&bluestop);

    // Initialize home grids
    drawHomeGrids(greenHome, "green");
    drawHomeGrids(redHome, "red");
    drawHomeGrids(blueHome, "blue");
    drawHomeGrids(yellowHome, "yellow");

    // Create threads for players
    pthread_t playerThreads[NO_PLAYERS];
    for (int i = 0; i < NO_PLAYERS; ++i)
    {
        int *value = new int(i);
        pthread_create(&playerThreads[i], NULL, turn, value);
    }

    // Create master thread
    pthread_t masterThreadId;
    pthread_create(&masterThreadId, NULL, MasterThread, NULL);

    // Set up kill counter
    killCounterTitle.setFont(mainFont);
    killCounterTitle.setString("KILLS");
    killCounterTitle.setCharacterSize(24);
    killCounterTitle.setFillColor(sf::Color::Black);
    killCounterTitle.setPosition(770, 430);

    // Initialize kill counter boxes and texts
    for (int i = 0; i < 4; i++)
    {
        // Set up boxes
        killCounterBoxes[i].setSize(sf::Vector2f(40, 40));
        killCounterBoxes[i].setPosition(770, 470 + (i * 45));
        killCounterBoxes[i].setOutlineThickness(0);

        // Set box colors and textures
        switch (i)
        {
        case 0: // Red (Heart)
            killCounterBoxes[i].setFillColor(sf::Color(228, 28, 36, 255));
            killCounterBoxes[i].setTexture(&PLAYERS[0].tokenTexture);
            break;
        case 1: // Blue (Club)
            killCounterBoxes[i].setFillColor(sf::Color(30, 112, 185, 255));
            killCounterBoxes[i].setTexture(&PLAYERS[2].tokenTexture);
            break;
        case 2: // Yellow (Diamond)
            killCounterBoxes[i].setFillColor(sf::Color(254, 205, 7, 255));
            killCounterBoxes[i].setTexture(&PLAYERS[3].tokenTexture);
            break;
        case 3: // Green (Spade)
            killCounterBoxes[i].setFillColor(sf::Color(98, 180, 70, 255));
            killCounterBoxes[i].setTexture(&PLAYERS[1].tokenTexture);
            break;
        }

        // Set up counter texts
        killCounterTexts[i].setFont(secFont);
        killCounterTexts[i].setString("0");
        killCounterTexts[i].setCharacterSize(24);
        killCounterTexts[i].setFillColor(sf::Color::Black);
        killCounterTexts[i].setPosition(830, 475 + (i * 45));
    }

    // Set up home counter
    homeCounterTitle.setFont(mainFont);
    homeCounterTitle.setString("HOME");
    homeCounterTitle.setCharacterSize(24);
    homeCounterTitle.setFillColor(sf::Color::Black);
    homeCounterTitle.setPosition(930, 430);

    // Initialize home counter boxes and texts
    for (int i = 0; i < 4; i++)
    {
        // Set up boxes
        homeCounterBoxes[i].setSize(sf::Vector2f(40, 40));
        homeCounterBoxes[i].setPosition(930, 470 + (i * 45));
        homeCounterBoxes[i].setOutlineThickness(0);

        // Set box colors and textures
        switch (i)
        {
        case 0: // Red (Heart)
            homeCounterBoxes[i].setTexture(&PLAYERS[0].tokenTexture);
            break;
        case 1: // Blue (Club)
            homeCounterBoxes[i].setTexture(&PLAYERS[2].tokenTexture);
            break;
        case 2: // Yellow (Diamond)
            homeCounterBoxes[i].setTexture(&PLAYERS[3].tokenTexture);
            break;
        case 3: // Green (Spade)
            homeCounterBoxes[i].setTexture(&PLAYERS[1].tokenTexture);
            break;
        }

        // Set up counter texts
        homeCounterTexts[i].setFont(secFont);
        homeCounterTexts[i].setString("0");
        homeCounterTexts[i].setCharacterSize(24);
        homeCounterTexts[i].setFillColor(sf::Color::Black);
        homeCounterTexts[i].setPosition(990, 475 + (i * 45));
    }

    // Main game loop
    while (mainWindow.isOpen() && gameRunning)
    {
        sf::Event event;
        while (mainWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                gameRunning = false;
                mainWindow.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(mainWindow);

                    // Handle dice click
                    if (DICE.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        cout << "Dice clicked!" << endl;
                    }
                    // Handle exit button click
                    else if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        if (showExitConfirmation())
                        {
                            // Cleanup before exit
                            gameRunning = false;
                            mainWindow.close();

                            // Signal all threads to stop
                            for (int i = 0; i < NO_PLAYERS; i++)
                            {
                                PLAYERS[i].continue_running = false;
                            }

                            // Cleanup resources
                            cleanup();

                            // Get current process ID and terminate
                            pid_t pid = getpid();
                            kill(pid, SIGTERM);
                            exit(0); // Backup exit in case kill fails
                        }
                    }
                    else if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(mainWindow);

                            // Handle exit button click
                            if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                            {
                                if (showExitConfirmation())
                                {
                                    exitGame();
                                    break;
                                }
                            }
                            // Handle restart button click
                            else if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                            {
                                if (showRestartConfirmation())
                                {
                                    restartGame();
                                }
                            }
                            // Handle other clicks...
                        }
                    }
                }
                fillOnBoard();
            }
        }

        // Render game state
        mainWindow.clear(sf::Color::White);

        // Draw board areas
        for (int i = 0; i < 4; i++)
        {
            mainWindow.draw(areas[i]);
            mainWindow.draw(centerAreas[i]);
        }

        // Draw game elements
        mainWindow.draw(DICE);
        mainWindow.draw(playerturn);
        mainWindow.draw(center);

        // Draw board grids
        for (int i = 0; i < GameConfig::BOARD_SIZE; ++i)
        {
            mainWindow.draw(playGroundGrids[i]);
        }

        // Draw home grids
        for (int i = 0; i < GameConfig::HOME_SQUARES; ++i)
        {
            mainWindow.draw(greenHome[i]);
            mainWindow.draw(redHome[i]);
            mainWindow.draw(blueHome[i]);
            mainWindow.draw(yellowHome[i]);
        }

        // Draw player tokens
        for (int color = 0; color < NO_PLAYERS; color++)
        {
            for (int t = 0; t < NO_TOKENS; t++)
            {
                if (PLAYERS[color].tokenPosition[t] == -1)
                {
                    mainWindow.draw(tokens[color][t]);
                }
            }
        }

        // Draw text elements
        mainWindow.draw(Name1);
        mainWindow.draw(Name2);
        mainWindow.draw(Name3);
        mainWindow.draw(Devs);

        mainWindow.draw(exitButton);
        mainWindow.draw(restartButton);

        // Draw kill counter
        mainWindow.draw(killCounterTitle);
        for (int i = 0; i < 4; i++)
        {
            mainWindow.draw(killCounterBoxes[i]);
            mainWindow.draw(killCounterTexts[i]);
        }

        // Draw home counter
        mainWindow.draw(homeCounterTitle);
        for (int i = 0; i < 4; i++)
        {
            mainWindow.draw(homeCounterBoxes[i]);
            mainWindow.draw(homeCounterTexts[i]);
        }

        mainWindow.display();
    }

    // Cleanup and join threads
    for (int i = 0; i < NO_PLAYERS; ++i)
    {
        pthread_join(playerThreads[i], NULL);
    }
    pthread_join(masterThreadId, NULL);

    cleanup();
    return 0;
}

// Replace the relevant section in the turn function in main.cpp
void *turn(void *arg)
{
    int player_id = *((int *)arg);
    delete (int *)arg;

    while (gameRunning) {
        while (PLAYERS[player_id].continue_running && !PLAYERS[player_id].isDisqualified) {
            PLAYERS[player_id].continue_running = false;

            pthread_mutex_lock(&turnMutex);

            // Skip if player has already won
            if (PLAYERS[player_id].hasWon) {
                pthread_mutex_unlock(&turnMutex);
                continue;
            }

            DICE.setTexture(&shuffleDiceTexture);
            playerturn.setTexture(&PLAYERS[player_id].tokenTexture);
            fillOnBoard();

            cout << "\n" << PLAYERS[player_id].color << "'s turn" << endl;

            bool turnComplete = false;
            while (mainWindow.isOpen() && !turnComplete && !PLAYERS[player_id].isDisqualified) {
                sf::Event event;
                while (mainWindow.pollEvent(event)) {
                    if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(mainWindow);
                            if (DICE.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                int val = PLAYERS[player_id].throwDice();
                                fillDiceTexture(val);
                                fillOnBoard();

                                if (val != 6) {
                                    turnComplete = true;
                                }
                            }
                        }
                    }
                }
            }

            // Check for three sixes
            if (PLAYERS[player_id].contains3Sixes())
            {
                cout << "⚠️ Three sixes in a row - turn forfeited!" << endl;
                PLAYERS[player_id].diceValues.clear();
                Cycle++;
                fillOnBoard();
                pthread_mutex_unlock(&turnMutex);
                continue;
            }

            // Process each dice value
            while (!PLAYERS[player_id].diceValues.empty())
            {
                int diceVal = PLAYERS[player_id].diceValues.front();

                // Check if any valid moves are possible with this dice value
                if (!PLAYERS[player_id].canMakeAnyMove(diceVal))
                {
                    cout << "\n❌ " << PLAYERS[player_id].color << " cannot make any moves with " << diceVal << ":" << endl;

                    // Check if token would enter home column without having kills
                    if (!PLAYERS[player_id].canEnterHomeColumn())
                    {
                        cout << "- Cannot enter home column without making a kill first" << endl;
                    }

                    if (!PLAYERS[player_id].checkInPlay() && diceVal != 6)
                    {
                        cout << "- Need a 6 to move tokens from yard" << endl;
                    }

                    cout << "Skipping turn..." << endl;
                    PLAYERS[player_id].diceValues.clear();
                    break;
                }

                // Process valid move
                int selectedDiceVal = PLAYERS[player_id].selectDiceValue();
                if (selectedDiceVal == -1)
                    break;
                PLAYERS[player_id].selectTokenNumber(selectedDiceVal);
                fillOnBoard();
            }

            // Check for kills after movement
            killCheck(player_id);

            // Clear remaining dice values
            if (!PLAYERS[player_id].diceValues.empty())
            {
                PLAYERS[player_id].diceValues.clear();
            }

            // Move to next player
            Cycle++;

            // Update board display
            fillOnBoard();
            pthread_mutex_unlock(&turnMutex);
        }
    }
    return NULL;
}

void *MasterThread(void *)
{
    fillOnBoard(); // Initial board setup

    while (gameRunning) {
        pthread_mutex_lock(&turnMutex);
        
        // Check for disqualification conditions
        for (int i = 0; i < NO_PLAYERS; i++) {
            if (!PLAYERS[i].hasWon && !PLAYERS[i].isDisqualified) {
                if (PLAYERS[i].turnsSinceLastSix >= 20 || PLAYERS[i].turnsSinceLastHit >= 20) {
                    cout << "\n⚠️ " << PLAYERS[i].color << " has been disqualified due to inactivity!" << endl;
                    PLAYERS[i].isDisqualified = true;
                    PLAYERS[i].continue_running = false;
                }
            }
        }

        // Check for game completion
        int activePlayers = 0;
        int winners = 0;
        for (int i = 0; i < NO_PLAYERS; i++) {
            if (!PLAYERS[i].isDisqualified) {
                activePlayers++;
            }
            if (PLAYERS[i].hasWon) {
                winners++;
            }
        }

        // End game if only one active player remains or all but one have won
        if (activePlayers <= 1 || winners >= NO_PLAYERS - 1) {
            cout << "\n🏁 Game Over! 🏁" << endl;
            
            // Print final standings
            cout << "\nFinal Standings:" << endl;
            for (int i = 0; i < NO_PLAYERS; i++) {
                string status;
                if (PLAYERS[i].hasWon) status = "Winner!";
                else if (PLAYERS[i].isDisqualified) status = "Disqualified";
                else status = "Incomplete";
                
                cout << PLAYERS[i].color << ": " << status 
                     << " (Kills: " << PLAYERS[i].killCount << ")" << endl;
            }
            
            gameRunning = false;
            pthread_mutex_unlock(&turnMutex);
            break;
        }

        // Reset cycle and continue game
        if (Cycle == NO_PLAYERS) {
            Cycle = 0;
            for (int i = 0; i < NO_PLAYERS; i++) {
                if (!PLAYERS[i].isDisqualified && !PLAYERS[i].hasWon) {
                    PLAYERS[i].continue_running = true;
                }
            }
        }

        pthread_mutex_unlock(&turnMutex);
        usleep(100000); // Sleep to prevent CPU overload
    }
    return NULL;
}

void killCheck(int currentPlayer)
{
    bool madeKill = false;
    
    for (int i = 0; i < NO_PLAYERS; i++) {
        if (i != currentPlayer && !PLAYERS[i].isDisqualified) {
            for (int j = 0; j < NO_TOKENS; j++) {
                int pos = PLAYERS[i].tokenPosition[j];
                if (pos != -1 && pos != -100) {
                    for (int k = 0; k < NO_TOKENS; k++) {
                        if (PLAYERS[currentPlayer].tokenPosition[k] == pos) {
                            // Don't kill if on safe square or starting position
                            if (PLAYERS[currentPlayer].canStackOnPosition(pos)) {
                                continue;
                            }

                            // Don't kill if part of a block
                            if (PLAYERS[i].isBlock(pos)) {
                                continue;
                            }

                            // Perform kill
                            PLAYERS[i].tokenPosition[j] = -1;
                            PLAYERS[i].tokenDistance[j] = -1;
                            PLAYERS[currentPlayer].killCount++;
                            madeKill = true;

                            // Update kill counter display
                            int killerIndex = -1;
                            if (PLAYERS[currentPlayer].color == "Red") killerIndex = 0;
                            else if (PLAYERS[currentPlayer].color == "Blue") killerIndex = 1;
                            else if (PLAYERS[currentPlayer].color == "Yellow") killerIndex = 2;
                            else if (PLAYERS[currentPlayer].color == "Green") killerIndex = 3;

                            if (killerIndex != -1) {
                                killCounterTexts[killerIndex].setString(to_string(PLAYERS[currentPlayer].killCount));
                            }

                            cout << "\n💥 Kill Details:" << endl;
                            cout << "Token " << k << " of " << PLAYERS[currentPlayer].color
                                 << " killed Token " << j << " of " << PLAYERS[i].color
                                 << " at position " << pos << endl;

                            // Update board display
                            playGroundGrids[pos].setTexture(&PLAYERS[currentPlayer].tokenTexture);
                        }
                    }
                }
            }
        }
    }

    // Reset turn counter if kill was made
    if (madeKill) {
        PLAYERS[currentPlayer].turnsSinceLastHit = 0;
    } else {
        PLAYERS[currentPlayer].turnsSinceLastHit++;
    }
}

void drawHomeGrids(sf::RectangleShape *homeGrid, string color)
{
    for (int i = 0; i < GameConfig::HOME_SQUARES; ++i)
    {
        homeGrid[i].setSize(sf::Vector2f(50, 50));
        homeGrid[i].setOutlineColor(sf::Color::Black);
        homeGrid[i].setOutlineThickness(2);

        if (color == "green")
        {
            homeGrid[i].setFillColor(sf::Color(98, 180, 70, 255));
            homeGrid[i].setPosition(350, 50 + (i * 50));
        }
        else if (color == "red")
        {
            homeGrid[i].setFillColor(sf::Color(228, 28, 36, 255));
            homeGrid[i].setPosition(50 + (i * 50), 350);
        }
        else if (color == "blue")
        {
            homeGrid[i].setFillColor(sf::Color(30, 112, 185, 255));
            homeGrid[i].setPosition(350, 650 - (i * 50));
        }
        else if (color == "yellow")
        {
            homeGrid[i].setFillColor(sf::Color(254, 205, 7, 255));
            homeGrid[i].setPosition(650 - (i * 50), 350);
        }
    }
}

void fillDiceTexture(int val)
{
    string filename = "images/dice-" + to_string(val) + ".png";
    if (!diceTexture.loadFromFile(filename))
    {
        cout << "Error loading dice texture: " << filename << endl;
        return;
    }
    DICE.setTexture(&diceTexture);
}

void fillOnBoard()
{
    // Reset board textures
    for (int i = 0; i < GameConfig::BOARD_SIZE; i++)
    {
        playGroundGrids[i].setTexture(nullptr);
        playGroundGrids[i].setFillColor(sf::Color::White);
    }

    // Set safe squares
    playGroundGrids[1].setTexture(&greenstop);
    playGroundGrids[40].setTexture(&redstop);
    playGroundGrids[14].setTexture(&yellowstop);
    playGroundGrids[27].setTexture(&bluestop);

    // Set gray color for safe squares
    for (int pos : SAFE_SQUARES)
    {
        playGroundGrids[pos].setFillColor(sf::Color(128, 128, 128));
    }

    // Update token positions on board
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        for (int j = 0; j < NO_TOKENS; j++)
        {
            if (PLAYERS[i].tokenPosition[j] >= 0 && PLAYERS[i].tokenPosition[j] < GameConfig::BOARD_SIZE)
            {
                playGroundGrids[PLAYERS[i].tokenPosition[j]].setTexture(&PLAYERS[i].tokenTexture);
            }
        }
    }
}

void inputWindow()
{
    sf::RenderWindow window(sf::VideoMode(300, 350), "Ludo Game Setup");
    window.setPosition(sf::Vector2i(
        sf::VideoMode::getDesktopMode().width / 2 - 150,
        sf::VideoMode::getDesktopMode().height / 2 - 175));

    // Title text
    sf::Text titleText;
    titleText.setFont(mainFont);
    titleText.setString("LUDO GAME");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color::Black);
    titleText.setStyle(sf::Text::Bold);
    // Center the title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition(
        (window.getSize().x - titleBounds.width) / 2,
        30
    );

    // Welcome text
    sf::Text welcomeText;
    welcomeText.setFont(secFont);
    welcomeText.setString("Welcome to the Game!");
    welcomeText.setCharacterSize(18);
    welcomeText.setFillColor(sf::Color::Black);
    // Center the welcome text
    sf::FloatRect welcomeBounds = welcomeText.getLocalBounds();
    welcomeText.setPosition(
        (window.getSize().x - welcomeBounds.width) / 2,
        80
    );

    // Players label
    sf::Text playersLabel;
    playersLabel.setFont(secFont);
    playersLabel.setString("Number of Players (2-4):");
    playersLabel.setCharacterSize(16);
    playersLabel.setFillColor(sf::Color::Black);
    playersLabel.setPosition(50, 120);

    // Players input box
    sf::RectangleShape playersBox(sf::Vector2f(200, 35));
    playersBox.setPosition(50, 145);
    playersBox.setFillColor(sf::Color::White);
    playersBox.setOutlineThickness(1);
    playersBox.setOutlineColor(sf::Color::Black);

    // Players input text
    sf::Text playersInput;
    playersInput.setFont(secFont);
    playersInput.setCharacterSize(16);
    playersInput.setFillColor(sf::Color::Black);
    playersInput.setPosition(60, 152);

    // Tokens label
    sf::Text tokensLabel;
    tokensLabel.setFont(secFont);
    tokensLabel.setString("Number of Tokens (1-4):");
    tokensLabel.setCharacterSize(16);
    tokensLabel.setFillColor(sf::Color::Black);
    tokensLabel.setPosition(50, 190);

    // Tokens input box
    sf::RectangleShape tokensBox(sf::Vector2f(200, 35));
    tokensBox.setPosition(50, 215);
    tokensBox.setFillColor(sf::Color::White);
    tokensBox.setOutlineThickness(1);
    tokensBox.setOutlineColor(sf::Color::Black);

    // Tokens input text
    sf::Text tokensInput;
    tokensInput.setFont(secFont);
    tokensInput.setCharacterSize(16);
    tokensInput.setFillColor(sf::Color::Black);
    tokensInput.setPosition(60, 222);

    // Play button
    sf::RectangleShape playButton(sf::Vector2f(200, 40));
    playButton.setPosition(50, 280);
    playButton.setFillColor(sf::Color(98, 180, 70)); // Green color
    playButton.setOutlineThickness(0);

    // Play button text
    sf::Text playText;
    playText.setFont(mainFont);
    playText.setString("Play Game");
    playText.setCharacterSize(20);
    playText.setFillColor(sf::Color::White);
    // Center the text on the button
    sf::FloatRect playBounds = playText.getLocalBounds();
    playText.setPosition(
        playButton.getPosition().x + (playButton.getSize().x - playBounds.width) / 2,
        playButton.getPosition().y + (playButton.getSize().y - playBounds.height) / 2 - 5
    );

    string playersStr, tokensStr;
    bool playersSelected = false;
    bool tokensSelected = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                exit(0);
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    
                    if (playersBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        playersSelected = true;
                        tokensSelected = false;
                    }
                    else if (tokensBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        tokensSelected = true;
                        playersSelected = false;
                    }
                    else if (playButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        try {
                            NO_PLAYERS = stoi(playersStr);
                            NO_TOKENS = stoi(tokensStr);
                            
                            if (NO_PLAYERS >= 2 && NO_PLAYERS <= 4 && 
                                NO_TOKENS >= 1 && NO_TOKENS <= 4)
                            {
                                window.close();
                                return;
                            }
                            else
                            {
                                playButton.setFillColor(sf::Color::Red);
                            }
                        }
                        catch (...) {
                            playButton.setFillColor(sf::Color::Red);
                        }
                    }
                    else
                    {
                        playersSelected = false;
                        tokensSelected = false;
                    }
                }
            }
            else if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128)
                {
                    if (playersSelected)
                    {
                        if (event.text.unicode == 8 && !playersStr.empty()) // Backspace
                            playersStr.pop_back();
                        else if (isdigit(event.text.unicode))
                            playersStr += static_cast<char>(event.text.unicode);
                        playersInput.setString(playersStr);
                    }
                    else if (tokensSelected)
                    {
                        if (event.text.unicode == 8 && !tokensStr.empty()) // Backspace
                            tokensStr.pop_back();
                        else if (isdigit(event.text.unicode))
                            tokensStr += static_cast<char>(event.text.unicode);
                        tokensInput.setString(tokensStr);
                    }
                }
            }
        }

        // Reset play button color
        playButton.setFillColor(sf::Color(98, 180, 70));

        // Draw everything
        window.clear(sf::Color::White);
        window.draw(titleText);
        window.draw(welcomeText);
        window.draw(playersLabel);
        window.draw(playersBox);
        window.draw(playersInput);
        window.draw(tokensLabel);
        window.draw(tokensBox);
        window.draw(tokensInput);
        window.draw(playButton);
        window.draw(playText);
        window.display();
    }
}

void initialize()
{
    PLAYERS = new Player[NO_PLAYERS];
    string colors[4] = {"Red", "Green", "Blue", "Yellow"};

    for (int i = 0; i < NO_PLAYERS; ++i)
    {
        PLAYERS[i].initializePlayer(colors[i], NO_TOKENS, START_POSITIONS[i], playerHomes[i], &SymbolArray[i]);
    }
}

void printDevNames()
{
    Devs.setFont(mainFont);
    Devs.setString("DEVS");
    Devs.setCharacterSize(60);
    Devs.setStyle(sf::Text::Bold);
    Devs.setFillColor(sf::Color::Black);
    Devs.setPosition(820, 370);

    Name1.setFont(secFont);
    Name1.setString("Hammad");
    Name1.setCharacterSize(35);
    Name1.setFillColor(sf::Color::Black);
    Name1.setPosition(850, 475);

    Name2.setFont(secFont);
    Name2.setString("Abdullah");
    Name2.setCharacterSize(35);
    Name2.setFillColor(sf::Color::Black);
    Name2.setPosition(835, 545);

    Name3.setFont(secFont);
    Name3.setString("Dawood");
    Name3.setCharacterSize(35);
    Name3.setFillColor(sf::Color::Black);
    Name3.setPosition(825, 615);
}

void cleanup()
{
    if (PLAYERS != nullptr)
    {
        delete[] PLAYERS;
        PLAYERS = nullptr;
    }
    pthread_mutex_destroy(&turnMutex);
    pthread_mutex_destroy(&gridMutex);
    pthread_cond_destroy(&turnCond);
}

bool showExitConfirmation()
{
    sf::RenderWindow confirmWindow(sf::VideoMode(300, 150), "Confirm Exit");
    confirmWindow.setPosition(sf::Vector2i(
        sf::VideoMode::getDesktopMode().width / 2 - 150,
        sf::VideoMode::getDesktopMode().height / 2 - 75));

    // Create confirmation message
    sf::Text message;
    message.setFont(secFont);
    message.setString("Exit Game?");
    message.setCharacterSize(24);
    message.setFillColor(sf::Color::Black);
    message.setPosition(100, 20);

    // Create Yes button
    sf::RectangleShape yesButton(sf::Vector2f(80, 40));
    yesButton.setFillColor(sf::Color::Green);
    yesButton.setPosition(50, 80);

    sf::Text yesText;
    yesText.setFont(secFont);
    yesText.setString("Yes");
    yesText.setCharacterSize(20);
    yesText.setFillColor(sf::Color::White);
    yesText.setPosition(70, 90);

    // Create No button
    sf::RectangleShape noButton(sf::Vector2f(80, 40));
    noButton.setFillColor(sf::Color::Red);
    noButton.setPosition(170, 80);

    sf::Text noText;
    noText.setFont(secFont);
    noText.setString("No");
    noText.setCharacterSize(20);
    noText.setFillColor(sf::Color::White);
    noText.setPosition(195, 90);

    while (confirmWindow.isOpen())
    {
        sf::Event event;
        while (confirmWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                confirmWindow.close();
                return false;
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(confirmWindow);

                    // Check Yes button click
                    if (yesButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        confirmWindow.close();
                        return true;
                    }
                    // Check No button click
                    else if (noButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        confirmWindow.close();
                        return false;
                    }
                }
            }
        }

        confirmWindow.clear(sf::Color::White);
        confirmWindow.draw(message);
        confirmWindow.draw(yesButton);
        confirmWindow.draw(noButton);
        confirmWindow.draw(yesText);
        confirmWindow.draw(noText);
        confirmWindow.display();
    }
    return false;
}

bool showRestartConfirmation()
{
    sf::RenderWindow confirmWindow(sf::VideoMode(300, 150), "Confirm Restart");
    confirmWindow.setPosition(sf::Vector2i(
        sf::VideoMode::getDesktopMode().width / 2 - 150,
        sf::VideoMode::getDesktopMode().height / 2 - 75));

    // Create confirmation message
    sf::Text message;
    message.setFont(secFont);
    message.setString("Restart Game?");
    message.setCharacterSize(24);
    message.setFillColor(sf::Color::Black);
    message.setPosition(90, 20);

    // Create Yes button
    sf::RectangleShape yesButton(sf::Vector2f(80, 40));
    yesButton.setFillColor(sf::Color::Green);
    yesButton.setPosition(50, 80);

    sf::Text yesText;
    yesText.setFont(secFont);
    yesText.setString("Yes");
    yesText.setCharacterSize(20);
    yesText.setFillColor(sf::Color::White);
    yesText.setPosition(70, 90);

    // Create No button
    sf::RectangleShape noButton(sf::Vector2f(80, 40));
    noButton.setFillColor(sf::Color::Red);
    noButton.setPosition(170, 80);

    sf::Text noText;
    noText.setFont(secFont);
    noText.setString("No");
    noText.setCharacterSize(20);
    noText.setFillColor(sf::Color::White);
    noText.setPosition(195, 90);

    while (confirmWindow.isOpen())
    {
        sf::Event event;
        while (confirmWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                confirmWindow.close();
                return false;
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(confirmWindow);

                    if (yesButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        confirmWindow.close();
                        return true;
                    }
                    else if (noButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        confirmWindow.close();
                        return false;
                    }
                }
            }
        }

        confirmWindow.clear(sf::Color::White);
        confirmWindow.draw(message);
        confirmWindow.draw(yesButton);
        confirmWindow.draw(noButton);
        confirmWindow.draw(yesText);
        confirmWindow.draw(noText);
        confirmWindow.display();
    }
    return false;
}
void cleanupThreads()
{
    // Signal all threads to stop
    gameRunning = false;

    // Wait for threads to finish
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        PLAYERS[i].continue_running = false;
        if (PLAYERS[i].playerThread)
        {
            pthread_join(PLAYERS[i].playerThread, NULL);
        }
    }

    // Join master thread
    pthread_join(masterThreadId, NULL);

    // Cleanup mutexes
    pthread_mutex_destroy(&turnMutex);
    pthread_mutex_init(&turnMutex, NULL);
}

void resetGameState()
{
    // Reset game variables
    Cycle = 0;
    gameRunning = true;

    // Reset players
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        PLAYERS[i].hasWon = false;
        PLAYERS[i].killCount = 0;
        PLAYERS[i].numTurns = 0;
        PLAYERS[i].continue_running = true;
        PLAYERS[i].diceValues.clear();

        // Reset tokens
        for (int j = 0; j < NO_TOKENS; j++)
        {
            PLAYERS[i].tokenPosition[j] = -1;
            PLAYERS[i].tokenDistance[j] = -1;
        }
    }

    // Reset counters display
    for (int i = 0; i < 4; i++)
    {
        killCounterTexts[i].setString("0");
        homeCounterTexts[i].setString("0");
    }
}

void restartGame()
{
    // Clean up existing threads
    cleanupThreads();

    // Reset game state
    resetGameState();

    // Create new player threads
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        int *value = new int(i);
        if (pthread_create(&PLAYERS[i].playerThread, NULL, turn, value) != 0)
        {
            cout << "Error recreating player thread " << i << endl;
            return;
        }
    }

    // Create new master thread
    if (pthread_create(&masterThreadId, NULL, MasterThread, NULL) != 0)
    {
        cout << "Error recreating master thread" << endl;
        return;
    }

    // Reset board display
    fillOnBoard();
    cout << "🔄 Game successfully restarted!" << endl;
}

void exitGame()
{
    // Signal all threads to stop
    gameRunning = false;

    // Clean up threads
    cleanupThreads();

    // Clean up resources
    cleanup();

    // Close window
    mainWindow.close();

    // Get current process ID and terminate gracefully
    pid_t pid = getpid();
    if (kill(pid, SIGTERM) != 0)
    {
        cout << "Error terminating process, using exit(0)" << endl;
        exit(0);
    }
}
// Update the home counter when a token reaches home
void updateHomeCounter(int playerIndex)
{
    int homeCount = 0;
    for (int i = 0; i < NO_TOKENS; i++)
    {
        if (PLAYERS[playerIndex].tokenPosition[i] == -100)
        {
            homeCount++;
        }
    }
    homeCounterTexts[playerIndex].setString(to_string(homeCount));
}

// Call updateHomeCounter in the appropriate place
void checkWinCondition()
{
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        updateHomeCounter(i);
    }
}
void initializeSynchronization()
{
    pthread_mutex_init(&turnMutex, NULL);
    pthread_mutex_init(&gridMutex, NULL);
    pthread_cond_init(&turnCond, NULL);
}
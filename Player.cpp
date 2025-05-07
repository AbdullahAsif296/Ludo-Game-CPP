#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "global.cpp"
using namespace std;

// Forward declaration of the Player class
class Player;

// External declarations
extern Player *PLAYERS;
extern int NO_PLAYERS;
extern sf::Text homeCounterTexts[4];

sf::Texture diceTexture;

class Player
{
public:
    string color;
    int numTurns;
    int killCount;
    int numTokens;
    int *tokenPosition;
    int *tokenDistance;
    int startingPoint;
    bool continue_running;
    sf::Texture tokenTexture;
    sf::RectangleShape symbolArea;
    vector<int> diceValues;
    int *tokenNumber;
    int nextToken;
    sf::RectangleShape *homeGrids;
    bool hasWon;
    pthread_t playerThread;
    int turnsSinceLastSix;
    int turnsSinceLastHit;
    bool isDisqualified;

    Player() : hasWon(false), turnsSinceLastSix(0), turnsSinceLastHit(0), isDisqualified(false) {}

    ~Player()
    {
        delete[] tokenPosition;
        delete[] tokenDistance;
        delete[] tokenNumber;
    }

    void initializePlayer(string color, int numTkn, int start, sf::RectangleShape *hmGrids, sf::RectangleShape *sym)
    {
        this->color = color;
        numTurns = 0;
        killCount = 0;
        numTokens = numTkn;
        tokenPosition = new int[numTokens];
        tokenDistance = new int[numTokens];
        tokenNumber = new int[numTokens];
        startingPoint = start;
        symbolArea = *sym;
        hasWon = false;

        for (int i = 0; i < numTokens; i++)
        {
            tokenPosition[i] = -1; // -1 indicates token is in starting yard
            tokenDistance[i] = -1; // -1 indicates no distance traveled
            tokenNumber[i] = i;
        }
        continue_running = true;
        nextToken = 0;
        homeGrids = hmGrids;

        string tokenPath = "images/";
        if (color == "Red")
            tokenPath += "heart_tkn.png";
        else if (color == "Green")
            tokenPath += "spade_tkn.png";
        else if (color == "Blue")
            tokenPath += "club_tkn.png";
        else if (color == "Yellow")
            tokenPath += "diamond_tkn.png";

        if (!tokenTexture.loadFromFile(tokenPath))
        {
            cout << "Error loading token texture: " << tokenPath << endl;
        }
    }

    // Check if player can enter home column based on kills
    bool canEnterHomeColumn()
    {
        return killCount > 0;
    }

    bool isSafeSquare(int position)
    {
        return find(SAFE_SQUARES.begin(), SAFE_SQUARES.end(), position) != SAFE_SQUARES.end();
    }

    bool isStartPosition(int position)
    {
        return find(START_POSITIONS, START_POSITIONS + 4, position) != START_POSITIONS + 4;
    }

    bool canStackOnPosition(int position)
    {
        return isSafeSquare(position) || isStartPosition(position);
    }

    bool isBlock(int position)
    {
        int blockCount = 0;
        for (int i = 0; i < numTokens; i++)
        {
            if (tokenPosition[i] == position && tokenPosition[i] != -1 && tokenPosition[i] != -100)
            {
                blockCount++;
                if (blockCount >= 2)
                    return true;
            }
        }
        return false;
    }

    bool isValidMove(int tokenIdx, int steps)
    {
        if (tokenIdx >= numTokens || tokenIdx < 0)
            return false;

        // Token in yard needs a 6
        if (tokenPosition[tokenIdx] == -1 && steps != 6)
        {
            cout << "Need a 6 to move token from yard!" << endl;
            return false;
        }

        // Calculate potential new distance
        int newDistance = tokenDistance[tokenIdx] + steps;

        // If token is already in home path (positions 101-105)
        if (tokenPosition[tokenIdx] >= 101)
        {
            int currentHomeStep = tokenPosition[tokenIdx] - 101;
            int newHomeStep = currentHomeStep + steps;
            if (newHomeStep > 5)
            {
                cout << "Move exceeds maximum home position!" << endl;
                return false;
            }
            if (newHomeStep == 5)
            {
                // Must land exactly on final position (distance 57)
                if (newDistance != 57)
                {
                    cout << "Must land exactly on final home position!" << endl;
                    return false;
                }
            }
            // Token in home path is always safe
            return true;
        }

        // Check if token would enter home path
        if (tokenDistance[tokenIdx] < 52 && newDistance >= 52)
        {
            if (killCount < 1)
            {
                cout << "Cannot enter home without killing at least one token!" << endl;
                return false;
            }
            // Calculate steps that would go into home path
            int stepsIntoHome = newDistance - 52;
            if (stepsIntoHome > 5)
            {
                cout << "Move would overshoot home path!" << endl;
                return false;
            }
        }

        // Calculate board position
        int newPos = (tokenPosition[tokenIdx] + steps) % GameConfig::BOARD_SIZE;

        // Check for blocks only if not entering home path
        if (newDistance < 52 && !canStackOnPosition(newPos))
        {
            for (int i = 0; i < NO_PLAYERS && PLAYERS != nullptr; i++)
            {
                if (PLAYERS[i].color != this->color && PLAYERS[i].isBlock(newPos))
                {
                    cout << "Position blocked by opponent's pieces!" << endl;
                    return false;
                }
            }
        }

        return true;
    }

    int throwDice()
    {
        int val = (rand() % 6) + 1;
        diceValues.push_back(val);
        cout << color << " rolled a " << val << endl;
        
        if (val == 6) {
            turnsSinceLastSix = 0;
        } else {
            turnsSinceLastSix++;
        }
        
        return val;
    }

    void printDiceValues()
    {
        cout << color << " Dice Values: ";
        for (int val : diceValues)
            cout << val << " ";
        cout << endl;
    }

    int selectDiceValue()
    {
        printDiceValues();

        if (diceValues.empty())
            return -1;

        while (true)
        {
            cout << "Enter dice value to use (";
            for (int val : diceValues)
                cout << val << " ";
            cout << "): ";

            int val;
            cin >> val;

            auto it = find(diceValues.begin(), diceValues.end(), val);
            if (it != diceValues.end())
            {
                diceValues.erase(it);
                return val;
            }
            cout << "Invalid dice value. Try again." << endl;
        }
    }

    void printTokenPositions()
    {
        cout << color << " Token Positions:" << endl;
        for (int i = 0; i < numTokens; i++)
        {
            string status;
            if (tokenPosition[i] == -1)
                status = "in yard";
            else if (tokenPosition[i] == -100)
                status = "in home";
            else
                status = "on board at " + to_string(tokenPosition[i]);

            cout << "Token " << i << ": " << status
                 << " (Distance=" << tokenDistance[i] << ")" << endl;
        }
    }

    bool checkInPlay()
    {
        for (int i = 0; i < numTokens; i++)
        {
            if (tokenPosition[i] != -1 && tokenPosition[i] != -100)
                return true;
        }
        return false;
    }

    bool tokenCheckInPlay(int tokenIdx)
    {
        return tokenPosition[tokenIdx] != -1 && tokenPosition[tokenIdx] != -100;
    }

    bool contains3Sixes()
    {
        int sixCount = count(diceValues.begin(), diceValues.end(), 6);
        return sixCount >= 3;
    }

    bool hasSix()
    {
        return find(diceValues.begin(), diceValues.end(), 6) != diceValues.end();
    }

    bool canMakeAnyMove(int diceValue)
    {
        // If it's a 6, check if any token can move from yard
        if (diceValue == 6)
        {
            for (int i = 0; i < numTokens; i++)
            {
                if (tokenPosition[i] == -1)
                    return true;
            }
        }

        // Check if any token in play can make a valid move
        for (int i = 0; i < numTokens; i++)
        {
            if (tokenPosition[i] != -1 && tokenPosition[i] != -100)
            {
                // For home entry, check kill requirement
                int newPos = tokenPosition[i] + diceValue;
                if (tokenDistance[i] >= 51 && killCount == 0)
                {
                    continue; // Skip this token if trying to enter home without a kill
                }

                // Check if move is valid for this token
                if (isValidMove(i, diceValue))
                {
                    return true;
                }
            }
        }

        // No valid moves available
        return false;
    }

    void selectTokenNumber(int dvalue)
    {
        printTokenPositions();

        if (dvalue == 6)
        {
            handleSixMove();
        }
        else
        {
            handleRegularMove(dvalue);
        }

        checkWinCondition();
    }

private:
    int calculateDistance(int currentPos, int startPos)
    {
        if (currentPos >= startPos)
        {
            return currentPos - startPos;
        }
        else
        {
            return (GameConfig::BOARD_SIZE - startPos) + currentPos;
        }
    }

private:
    // Helper function to check if token can enter home path
    bool canEnterHomePath(int oldPos, int newPos)
    {
        // For Red (starts at 40)
        if (color == "Red")
        {
            return (oldPos <= 44 && newPos > 44) ||                                                              // Normal case
                   (oldPos <= 44 && newPos >= GameConfig::BOARD_SIZE && (newPos % GameConfig::BOARD_SIZE) < 44); // Wrap around case
        }
        // For Green (starts at 1)
        else if (color == "Green")
        {
            return oldPos <= 11 && newPos > 11;
        }
        // For Blue (starts at 27)
        else if (color == "Blue")
        {
            return oldPos <= 31 && newPos > 31;
        }
        // For Yellow (starts at 14)
        else if (color == "Yellow")
        {
            return oldPos <= 24 && newPos > 24;
        }
        return false;
    }

    // Helper function to get the board position for home path squares
    int getHomePathPosition(int stepsIntoHome)
    {
        if (color == "Red")
        {
            // Red home column is the vertical red squares leading to center
            return 45 - stepsIntoHome; // Starts at 44 and goes up
        }
        else if (color == "Green")
        {
            // Green home column is the vertical green squares leading to center
            return 7 + stepsIntoHome; // Starts at 8 and goes down
        }
        else if (color == "Blue")
        {
            // Blue home column is the vertical blue squares leading to center
            return 32 - stepsIntoHome; // Starts at 31 and goes up
        }
        else if (color == "Yellow")
        {
            // Yellow home column is the vertical yellow squares leading to center
            return 20 + stepsIntoHome; // Starts at 21 and goes down
        }
        return -1;
    }

private:
    bool canEnterHomePath(int oldPos, int newPos, int totalDistance)
    {
        // Token can only enter home path if it has traveled exactly 52 steps
        return totalDistance >= 52;
    }

public:
    void moveToken(int tokenIdx, int distance)
    {
        int oldPos = tokenPosition[tokenIdx];

        // Handle token in yard
        if (oldPos == -1)
        {
            tokenPosition[tokenIdx] = startingPoint;
            tokenDistance[tokenIdx] = 0;
            playGroundGrids[startingPoint].setTexture(&tokenTexture);
            return;
        }

        // Calculate new distance
        int newDistance = tokenDistance[tokenIdx] + distance;

        // Clear old position textures
        if (oldPos >= 0 && oldPos < GameConfig::BOARD_SIZE)
        {
            playGroundGrids[oldPos].setTexture(nullptr);
        }
        else if (oldPos >= 101 && oldPos <= 105)
        {
            // Clear old home path position
            int oldHomeStep = oldPos - 101;
            int oldHomeGridPos = getHomePathPosition(oldHomeStep);
            playGroundGrids[oldHomeGridPos].setTexture(nullptr);

            // Also clear from home grids array
            if (color == "Red")
                redHome[oldHomeStep].setTexture(nullptr);
            else if (color == "Green")
                greenHome[oldHomeStep].setTexture(nullptr);
            else if (color == "Blue")
                blueHome[oldHomeStep].setTexture(nullptr);
            else if (color == "Yellow")
                yellowHome[oldHomeStep].setTexture(nullptr);
        }

        // Handle token already in home path
        if (oldPos >= 101)
        {
            int currentHomeStep = oldPos - 101;
            int newHomeStep = currentHomeStep + distance;

            if (newHomeStep == 5)
            {
                tokenPosition[tokenIdx] = -100; // Reached final home
                tokenDistance[tokenIdx] = 57;   // Final distance
                cout << color << " Token " << tokenIdx << " has reached home!" << endl;
                updateHomeCounter(getPlayerIndex());
            }
            else
            {
                tokenPosition[tokenIdx] = 101 + newHomeStep;
                tokenDistance[tokenIdx] = newDistance;

                // Update visual position in both board and home grids
                int homePathPos = getHomePathPosition(newHomeStep);
                playGroundGrids[homePathPos].setTexture(&tokenTexture);

                // Update home grid array
                if (color == "Red")
                    redHome[newHomeStep].setTexture(&tokenTexture);
                else if (color == "Green")
                    greenHome[newHomeStep].setTexture(&tokenTexture);
                else if (color == "Blue")
                    blueHome[newHomeStep].setTexture(&tokenTexture);
                else if (color == "Yellow")
                    yellowHome[newHomeStep].setTexture(&tokenTexture);
            }
            return;
        }

        // Handle entering home path (crossing distance 52)
        if (tokenDistance[tokenIdx] < 52 && newDistance >= 52)
        {
            if (killCount >= 1)
            {
                int stepsIntoHome = newDistance - 52;
                tokenPosition[tokenIdx] = 101 + stepsIntoHome;
                tokenDistance[tokenIdx] = newDistance;

                // Set visual position in both board and home grids
                int homePathPos = getHomePathPosition(stepsIntoHome);
                playGroundGrids[homePathPos].setTexture(&tokenTexture);

                // Update home grid array
                if (color == "Red")
                    redHome[stepsIntoHome].setTexture(&tokenTexture);
                else if (color == "Green")
                    greenHome[stepsIntoHome].setTexture(&tokenTexture);
                else if (color == "Blue")
                    blueHome[stepsIntoHome].setTexture(&tokenTexture);
                else if (color == "Yellow")
                    yellowHome[stepsIntoHome].setTexture(&tokenTexture);
            }
            else
            {
                // Continue on main board if no kills
                int newPos = (oldPos + distance) % GameConfig::BOARD_SIZE;
                tokenPosition[tokenIdx] = newPos;
                tokenDistance[tokenIdx] = newDistance;
                playGroundGrids[newPos].setTexture(&tokenTexture);
            }
        }
        else
        {
            // Regular movement on board
            int newPos = (oldPos + distance) % GameConfig::BOARD_SIZE;
            tokenPosition[tokenIdx] = newPos;
            tokenDistance[tokenIdx] = newDistance;
            playGroundGrids[newPos].setTexture(&tokenTexture);
        }
    }

    void handleSixMove()
    {
        while (true)
        {
            cout << "Enter token number to move (0-" << (numTokens - 1) << "): ";
            int tokenIdx;
            cin >> tokenIdx;

            if (tokenIdx >= numTokens || tokenIdx < 0)
            {
                cout << "Invalid token number." << endl;
                continue;
            }

            // Handle token in yard
            // In handleSixMove() method when token moves from yard to starting position:
            if (tokenPosition[tokenIdx] == -1)
            {
                tokenPosition[tokenIdx] = startingPoint;
                tokenDistance[tokenIdx] = 1; // Initial distance is 5 when token starts
                playGroundGrids[startingPoint].setTexture(&tokenTexture);
                cout << color << " Token " << tokenIdx << " moved from yard to starting position (Distance=1)" << endl;
                break;
            }
            // Handle token already in play
            if (isValidMove(tokenIdx, 6))
            {
                moveToken(tokenIdx, 6);
                break;
            }

            cout << "Invalid move. Try another token." << endl;
        }
    }

    void handleRegularMove(int dvalue)
    {
        while (true)
        {
            cout << "Enter token number to move (0-" << (numTokens - 1) << "): ";
            int tokenIdx;
            cin >> tokenIdx;

            if (tokenIdx >= numTokens || tokenIdx < 0)
            {
                cout << "Invalid token number." << endl;
                continue;
            }

            if (!tokenCheckInPlay(tokenIdx))
            {
                cout << "Token not in play. Need a 6 to start." << endl;
                continue;
            }

            if (isValidMove(tokenIdx, dvalue))
            {
                moveToken(tokenIdx, dvalue);
                break;
            }

            cout << "Invalid move. Try another token." << endl;
        }
    }

    void checkWinCondition()
    {
        int completedTokens = 0;
        for (int i = 0; i < numTokens; i++)
        {
            if (tokenPosition[i] == -100)
            {
                completedTokens++;
            }
        }
        if (completedTokens == numTokens)
        {
            hasWon = true;
            cout << "\n🎉 " << color << " has won the game! 🎉" << endl;
        }
    }

    // Helper method to get player index for updating counters
    int getPlayerIndex()
    {
        if (color == "Red")
            return 0;
        if (color == "Blue")
            return 1;
        if (color == "Yellow")
            return 2;
        if (color == "Green")
            return 3;
        return 0;
    }

    // Helper method to update home counter display
    void updateHomeCounter(int playerIdx)
    {
        extern sf::Text homeCounterTexts[4];
        int homeCount = 0;
        for (int i = 0; i < numTokens; i++)
        {
            if (tokenPosition[i] == -100)
            {
                homeCount++;
            }
        }
        homeCounterTexts[playerIdx].setString(std::to_string(homeCount));
    }

    void resetTurnCounters() {
        turnsSinceLastSix = 0;
        turnsSinceLastHit = 0;
    }
};
#include "raylib.h" // Ignore this error, it will be solved when compiling using gcc with raylib.
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "MachineLearningComponents/main_machine_learning_ai.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 850

#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 50

// Define page names.
typedef enum
{
        HOME,
        SINGLEPLAYER,
        MULTIPLAYER
} GameState;

typedef struct Move
{
        int row;
        int col;
} move;

// ############################# Tic tac toe game logic start. #############################
// Declare 3x3 tic tac toe grid.
#define GRID_SIZE 3

// Define tic tac toe each individual grid cell state.
typedef enum
{
        EMPTY = 0,
        CROSS = 1,
        CIRCLE = 2
} Cell;

// For minimax
void randomisePlayers();
move aiMakeMove(Cell grid[GRID_SIZE][GRID_SIZE]);
move ImperfectaiMakeMove(Cell grid[GRID_SIZE][GRID_SIZE]);
bool movesRemaining(Cell grid[GRID_SIZE][GRID_SIZE]);
move calculateBestMove(Cell grid[GRID_SIZE][GRID_SIZE], move bestMove);
move calculateImperfectMove(Cell grid[GRID_SIZE][GRID_SIZE], move ImperfectMove);
int miniMax(Cell grid[GRID_SIZE][GRID_SIZE], int depth, bool isMaximiserTurn);

bool playerXTurn;

// Function to set the symbol in the grid when a player makes their move.
void setCell(Cell grid[GRID_SIZE][GRID_SIZE], int row, int col, Cell value)
{
        grid[row][col] = value;
}

// Function to check if a player has won.
bool checkWin(Cell grid[GRID_SIZE][GRID_SIZE], Cell player)
{
        // Check rows (horizontal).
        for (int i = 0; i < GRID_SIZE; i++)
        {
                bool win = true;
                for (int j = 0; j < GRID_SIZE; j++)
                {
                        if (grid[i][j] != player)
                        {
                                win = false;
                                break;
                        }
                }
                if (win)
                {
                        return true;
                }
        }

        // Check columns (vertical).
        for (int j = 0; j < GRID_SIZE; j++)
        {
                bool win = true;
                for (int i = 0; i < GRID_SIZE; i++)
                {
                        if (grid[i][j] != player)
                        {
                                win = false;
                                break;
                        }
                }
                if (win)
                {
                        return true;
                }
        }

        // Check diagonal from top-left to bottom-right.
        bool win = true;
        for (int i = 0; i < GRID_SIZE; i++)
        {
                if (grid[i][i] != player)
                {
                        win = false;
                        break;
                }
        }
        if (win)
        {
                return true;
        }

        // Check diagonal from top-right to bottom-left
        win = true;
        for (int i = 0; i < GRID_SIZE; i++)
        {
                if (grid[i][GRID_SIZE - i - 1] != player)
                {
                        win = false;
                        break;
                }
        }
        return win;
}

// Function to draw the tic tac toe grid.
void drawGrid(Cell grid[GRID_SIZE][GRID_SIZE])
{
        int cellSize = 100;
        int xOffset = (SCREEN_WIDTH - (cellSize * GRID_SIZE)) / 2;
        int yOffset = (SCREEN_HEIGHT - (cellSize * GRID_SIZE)) / 2;

        // Draw tic tac toe board grid lines.
        for (int i = 0; i <= GRID_SIZE; i++)
        {
                DrawLine(xOffset + (cellSize * i), yOffset, xOffset + (cellSize * i), yOffset + (cellSize * GRID_SIZE), BLACK);
                DrawLine(xOffset, yOffset + (cellSize * i), xOffset + (cellSize * GRID_SIZE), yOffset + (cellSize * i), BLACK);
        }

        // Draw cells.
        for (int i = 0; i < GRID_SIZE; i++)
        {
                for (int j = 0; j < GRID_SIZE; j++)
                {
                        int x = xOffset + (cellSize * j) + (cellSize / 2);
                        int y = yOffset + (cellSize * i) + (cellSize / 2);

                        if (grid[i][j] == CROSS)
                        {
                                DrawLine(x - 30, y - 30, x + 30, y + 30, BLACK);
                                DrawLine(x - 30, y + 30, x + 30, y - 30, BLACK);
                        }
                        else if (grid[i][j] == CIRCLE)
                        {
                                DrawCircle(x, y, 30, BLACK);
                        }
                }
        }
}

// Function to print the current state of the tic-tac-toe grid on the terminal
void printGrid(Cell grid[GRID_SIZE][GRID_SIZE])
{
        for (int i = 0; i < GRID_SIZE; i++)
        {
                for (int j = 0; j < GRID_SIZE; j++)
                {
                        // Print the symbol based on the value in the grid
                        switch (grid[i][j])
                        {
                        case EMPTY:
                                printf(" ");
                                break;
                        case CROSS:
                                printf("X");
                                break;
                        case CIRCLE:
                                printf("O");
                                break;
                        }

                        // Print a separator between cells
                        if (j < GRID_SIZE - 1)
                        {
                                printf(" | ");
                        }
                }

                // Print a new line between rows
                printf("\n");

                // Print a horizontal line between rows
                if (i < GRID_SIZE - 1)
                {
                        for (int k = 0; k < GRID_SIZE * 4 - 1; k++)
                        {
                                printf("-");
                        }
                        printf("\n");
                }
        }

        printf("\n");
}

// ############################# Tic tac toe game logic end. #############################

int main(void)
{
        // Initialise Raylib.
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic Tac Toe");

        // Declare variables.
        char name[64] = "";        // Array to store user input player name for multiplayer mode.
        char player1[64] = "";     // Save name array value to player1 array accordingly.
        char player2[64] = "";     // Save name array value to player2 array accordingly.
        int playerNameCount = 1;   // Handle logic for asking user input names of 2 players before proceeding with the tic tac toe game in multiplayer mode.
        bool modalVisible = false; // Handle logic for displaying and hiding modal.
        // Initialize the tic tac toe grid.
        Cell grid[GRID_SIZE][GRID_SIZE] = {{EMPTY, EMPTY, EMPTY},
                                           {EMPTY, EMPTY, EMPTY},
                                           {EMPTY, EMPTY, EMPTY}};
        bool gameOver = false; // Detect whether game has ended.

        // Randomise which player gets the first turn.
        // For singleplayer mode, player 1 is human, player 2 is AI.
        randomisePlayers();

        int winner = 0; // Identify who won the game. 1=player 1 won. 2=player 2 won. 3=tied game nobody won.
        // Count number of times each player wins.
        int player1WinCounter = 0;
        int player2WinCounter = 0;
        // Variable to store user selected AI difficulty level. -1=placeholder value, 0=easy, 1=medium, 2=hard, 3=impossible.
        int aiDifficultySelected = -1;

        // Set state to HOME to display HOME page when program is first launched.
        GameState state = HOME;

        // Create and set the dimensions of each button.
        // Dimensions are dynamically calculated using the defined screen size.
        Rectangle singleplayerButton = {SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - BUTTON_HEIGHT / 2 - 50,
                                        BUTTON_WIDTH, BUTTON_HEIGHT};
        Rectangle multiplayerButton = {SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - BUTTON_HEIGHT / 2 + 50,
                                       BUTTON_WIDTH, BUTTON_HEIGHT};
        Rectangle quitButton = {10, SCREEN_HEIGHT - BUTTON_HEIGHT - 10, BUTTON_WIDTH, BUTTON_HEIGHT};
        Rectangle easyDifficultyButton = {SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - BUTTON_HEIGHT / 2 - 50,
                                          BUTTON_WIDTH, BUTTON_HEIGHT};
        Rectangle mediumDifficultyButton = {SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - BUTTON_HEIGHT / 2 + 50,
                                            BUTTON_WIDTH, BUTTON_HEIGHT};
        Rectangle hardDifficultyButton = {SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - BUTTON_HEIGHT / 2 + 150,
                                          BUTTON_WIDTH, BUTTON_HEIGHT};
        Rectangle impossibleDifficultyButton = {SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - BUTTON_HEIGHT / 2 + 250,
                                                BUTTON_WIDTH, BUTTON_HEIGHT};

        while (!WindowShouldClose())
        {
                BeginDrawing();
                ClearBackground(RAYWHITE);

                // Depending on which page is selected, display the following widgets accordingly.
                switch (state)
                {
                // HOME page.
                case HOME:
                        DrawText("Tic Tac Toe", SCREEN_WIDTH / 2 - MeasureText("Tic Tac Toe", 40) / 2, SCREEN_HEIGHT / 3, 40, BLACK);
                        DrawRectangleRec(singleplayerButton, DARKBLUE);
                        DrawRectangleRec(multiplayerButton, DARKBLUE);
                        DrawText("Singleplayer", singleplayerButton.x + BUTTON_WIDTH / 2 - MeasureText("Singleplayer", 20) / 2,
                                 singleplayerButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);
                        DrawText("Multiplayer", multiplayerButton.x + BUTTON_WIDTH / 2 - MeasureText("Multiplayer", 20) / 2,
                                 multiplayerButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);
                        break;
                // SINGLEPLAYER page, player vs ai tic tac toe game.
                case SINGLEPLAYER:
                        // If modal is hidden.
                        if (!modalVisible)
                        {
                                // Draw background color.
                                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

                                // Display win counter of player and AI respectively.
                                DrawText(TextFormat("You: %d | AI: %d", player1WinCounter, player2WinCounter), SCREEN_WIDTH / 2 - MeasureText("You: x | AI: x", 20), SCREEN_HEIGHT / 11, 40, BLACK);

                                // Display selected AI difficulty.
                                // If AI difficulty easy.
                                if (aiDifficultySelected == 0)
                                {
                                        DrawText(TextFormat("AI difficulty: Easy"), SCREEN_WIDTH / 2 - MeasureText("AI difficulty: Easy", 20), SCREEN_HEIGHT / 6, 40, BLACK);
                                }
                                // If AI difficulty medium.
                                else if (aiDifficultySelected == 1)
                                {
                                        DrawText(TextFormat("AI difficulty: Medium"), SCREEN_WIDTH / 2 - MeasureText("AI difficulty: Medium", 20), SCREEN_HEIGHT / 6, 40, BLACK);
                                }

                                // If AI difficulty hard.
                                else if (aiDifficultySelected == 2)
                                {
                                        DrawText(TextFormat("AI difficulty: Hard"), SCREEN_WIDTH / 2 - MeasureText("AI difficulty: Hard", 20), SCREEN_HEIGHT / 6, 40, BLACK);
                                }

                                // If AI difficulty impossible.
                                else if (aiDifficultySelected == 3)
                                {
                                        DrawText(TextFormat("AI difficulty: Impossible"), SCREEN_WIDTH / 2 - MeasureText("AI difficulty: Impossible", 20), SCREEN_HEIGHT / 6, 40, BLACK);
                                }

                                // Display when it is the players turn.
                                if (playerXTurn == true)
                                {
                                        DrawText(TextFormat("Your turn. Your symbol is CROSS."), SCREEN_WIDTH / 3, SCREEN_HEIGHT / 4, 25, BLACK);
                                }

                                // Display who won the game or if tied game.
                                // If player won the game.
                                if (gameOver == true && winner == 1)
                                {
                                        DrawText(TextFormat("You won. Press enter to play again."), SCREEN_WIDTH - 850, SCREEN_HEIGHT - 250, 25, BLACK);
                                }

                                // If AI won the game.
                                else if (gameOver == true && winner == 2)
                                {
                                        DrawText(TextFormat("You lost. Press enter to play again."), SCREEN_WIDTH - 850, SCREEN_HEIGHT - 250, 25, BLACK);
                                }

                                // If tied game, no winners.
                                else if (gameOver == true && winner == 3)
                                {
                                        DrawText(TextFormat("Tie. Press enter to play again."), SCREEN_WIDTH - 850, SCREEN_HEIGHT - 250, 25, BLACK);
                                }

                                // Display quitButton to return to HOME page if user clicks.
                                DrawRectangleRec(quitButton, DARKBLUE);
                                DrawText("Quit", quitButton.x + BUTTON_WIDTH / 2 - MeasureText("Quit", 20) / 2,
                                         quitButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);

                                // Singleplayer tic tac toe game begins.
                                drawGrid(grid); // Draw the tic tac toe table.
                                // Tic tac toe game logic.
                                if (!gameOver)
                                {
                                        // If player turn.
                                        if (playerXTurn)
                                        {
                                                // Listen for mouse click
                                                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                                {
                                                        int mouseX = GetMouseX();
                                                        int mouseY = GetMouseY();

                                                        // Determine mouse click coordinates on the board
                                                        if (mouseX >= 0 && mouseX < SCREEN_WIDTH && mouseY >= 0 && mouseY < SCREEN_HEIGHT)
                                                        {
                                                                int cellX = (mouseX - ((SCREEN_WIDTH - (GRID_SIZE * 100)) / 2)) / 100;
                                                                int cellY = (mouseY - ((SCREEN_HEIGHT - (GRID_SIZE * 100)) / 2)) / 100;

                                                                // Check if the clicked cell is empty
                                                                if (grid[cellY][cellX] == EMPTY)
                                                                {
                                                                        // Place player symbol "CROSS" on the cell
                                                                        setCell(grid, cellY, cellX, CROSS);

                                                                        playerXTurn = !playerXTurn; // Switch turn.

                                                                        // If player win.
                                                                        if (checkWin(grid, CROSS))
                                                                        {
                                                                                gameOver = true;        // End the game.
                                                                                player1WinCounter += 1; // Increment player win counter.
                                                                                winner = 1;             // Set winner as player.
                                                                        }

                                                                        else
                                                                        {
                                                                                bool allCellsFilled = true;

                                                                                // Check for any remaining empty cells on the board,
                                                                                // false value indicates that there are cells not yet occupied
                                                                                for (int i = 0; i < GRID_SIZE; i++)
                                                                                {
                                                                                        for (int j = 0; j < GRID_SIZE; j++)
                                                                                        {
                                                                                                if (grid[i][j] == EMPTY)
                                                                                                {
                                                                                                        allCellsFilled = false;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                        if (!allCellsFilled)
                                                                                                break;
                                                                                }

                                                                                // If all cells are filled and there is no winner, the game is tied.
                                                                                if (allCellsFilled)
                                                                                {
                                                                                        gameOver = true;
                                                                                        winner = 3; // Set winner as nobody because tied game.
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                        }

                                        // If AI turn.
                                        else
                                        {
                                                // Check for difficulty level selected
                                                switch (aiDifficultySelected)
                                                {
                                                // When it is "Easy" level
                                                case 0:
                                                        // Use while loop ensure  variables declared here a local and won't clash references with the ones below
                                                        while (1)
                                                        {
                                                                int row, col;
                                                                int numOfEmptyCells = 0, c = GRID_SIZE * GRID_SIZE;
                                                                int emptyCellIndices[c] = {};
                                                                // Loop thru every row and col of the grid to find empty cells
                                                                for (row = 0; row < GRID_SIZE; row++)
                                                                        for (col = 0; col < GRID_SIZE; col++)
                                                                        {
                                                                                if (grid[row][col] != EMPTY) continue;

                                                                                // Calculate and Add grid index into the emptyCellIndices
                                                                                c = row * GRID_SIZE + col;
                                                                                emptyCellIndices[numOfEmptyCells] = c;
                                                                                numOfEmptyCells++; // This will be used to make a makeshift list
                                                                        }

                                                                // Get a random number from 0 to the value of numOfEmptyCells
                                                                c = rand() % numOfEmptyCells;
                                                                // get gridNumber that is selected
                                                                c = emptyCellIndices[c];

                                                                // Convert gridNumber back to row and col
                                                                row = c / GRID_SIZE;
                                                                col = c - (row * GRID_SIZE);

                                                                // If position is not filled, place AI move then switch turns back to the player.
                                                                setCell(grid, row, col, CIRCLE);
                                                                playerXTurn = !playerXTurn; // Switch turn.
                                                                break;
                                                        }
                                                        break;

                                                // When it is "Medium" level
                                                case 1:
                                                        int row = 0, col = 0, *row_pointer = &row, *col_pointer = &col;
                                                        int gridAsInt[GRID_SIZE][GRID_SIZE] = {};

                                                        // Populate the enum values into the gridAsInt array
                                                        for (row = 0; row < GRID_SIZE; row++)
                                                                for (col = 0; col < GRID_SIZE; col++)
                                                                        gridAsInt[row][col] = (int)grid[row][col];

                                                        Decide_Next_Move(GRID_SIZE, gridAsInt, row_pointer, col_pointer); // Pass current board and get ai's next selected cell

                                                        setCell(grid, *row_pointer, *col_pointer, CIRCLE); // Select the cell
                                                        playerXTurn = !playerXTurn;                        // Switch turn.
                                                        break;

                                                // When it is "Hard" level
                                                case 2:
                                                        while (1)
                                                        {
                                                                move imperfectMove;
                                                                imperfectMove = ImperfectaiMakeMove(grid);

                                                                if (grid[imperfectMove.row][imperfectMove.col] == EMPTY) // This is the logic to check whether position is already filled.
                                                                {
                                                                        printf("Before AI move:\n");
                                                                        printGrid(grid);
                                                                        // If position not filled, place AI move then switch turns back to the player.
                                                                        // Otherwise recalculate algorithm and place another move at a position that is not yet filled.
                                                                        setCell(grid, imperfectMove.row, imperfectMove.col, CIRCLE);
                                                                        printf("After AI move:\n");
                                                                        printGrid(grid);
                                                                        playerXTurn = !playerXTurn; // Switch turn.
                                                                        break;                      // Exit infinite while loop.
                                                                }
                                                        }
                                                        break;
                                                // When it is "Impossible" level
                                                case 3:
                                                        while (1) // Infinite loop to ensure AI places symbol in empty cell
                                                        {
                                                                move bestMove;

                                                                // bestMove variable will hold the row and column of the best move
                                                                bestMove = aiMakeMove(grid);

                                                                if (grid[bestMove.row][bestMove.col] == EMPTY) // This is the logic to check whether position is already filled.
                                                                {
                                                                        // Print grid in terminal for checking
                                                                        printf("Before AI move:\n");
                                                                        printGrid(grid);

                                                                        // If position not filled, place AI move then switch turns back to the player.
                                                                        // Otherwise recalculate algorithm and place another move at a position that is not yet filled.
                                                                        setCell(grid, bestMove.row, bestMove.col, CIRCLE);

                                                                        // Print grid in terminal for checking
                                                                        printf("After AI move:\n");
                                                                        printGrid(grid);
                                                                        playerXTurn = !playerXTurn; // Switch turn.
                                                                        break;                      // Exit infinite while loop.
                                                                }
                                                        }
                                                        break;

                                                default:
                                                        printf("Unknown level %d selected.. This should not happen!\n", aiDifficultySelected);
                                                        break;
                                                }

                                                // If AI win.
                                                if (checkWin(grid, CIRCLE))
                                                {
                                                        gameOver = true;        // End the game.
                                                        player2WinCounter += 1; // Increment AI win counter.
                                                        winner = 2;             // Set winner as AI.
                                                }

                                                else
                                                {
                                                        bool allCellsFilled = true;

                                                        // Check for any remaining empty cells on the board,
                                                        // false value indicates that there are cells not yet occupied
                                                        for (int i = 0; i < GRID_SIZE; i++)
                                                        {
                                                                for (int j = 0; j < GRID_SIZE; j++)
                                                                {
                                                                        if (grid[i][j] == EMPTY)
                                                                        {
                                                                                allCellsFilled = false;
                                                                                break;
                                                                        }
                                                                }
                                                                if (!allCellsFilled)
                                                                        break;
                                                        }

                                                        // If all cells are filled and there is no winner, the game is tied.
                                                        if (allCellsFilled)
                                                        {
                                                                gameOver = true;
                                                                winner = 3; // Set winner as nobody because tied game.
                                                        }
                                                }
                                        }
                                }
                        }

                        // If modal is visible.
                        else if (modalVisible)
                        {
                                // Draw background color.
                                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                                // Display text and buttons to ask for user input to select AI difficulty.
                                DrawText("Select AI difficulty", SCREEN_WIDTH / 2 - MeasureText("Select AI difficulty", 40) / 2, SCREEN_HEIGHT / 3, 40, BLACK);
                                DrawRectangleRec(easyDifficultyButton, DARKBLUE);
                                DrawRectangleRec(mediumDifficultyButton, DARKBLUE);
                                DrawRectangleRec(hardDifficultyButton, DARKBLUE);
                                DrawRectangleRec(impossibleDifficultyButton, DARKBLUE);
                                DrawText("Easy", easyDifficultyButton.x + BUTTON_WIDTH / 2 - MeasureText("Easy", 20) / 2,
                                         easyDifficultyButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);
                                DrawText("Medium", mediumDifficultyButton.x + BUTTON_WIDTH / 2 - MeasureText("Medium", 20) / 2,
                                         mediumDifficultyButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);
                                DrawText("Hard", hardDifficultyButton.x + BUTTON_WIDTH / 2 - MeasureText("Hard", 20) / 2,
                                         hardDifficultyButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);
                                DrawText("Impossible", impossibleDifficultyButton.x + BUTTON_WIDTH / 2 - MeasureText("Impossible", 20) / 2,
                                         impossibleDifficultyButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);

                                // Display quitButton to return to HOME page if user clicks.
                                DrawRectangleRec(quitButton, DARKBLUE);
                                DrawText("Quit", quitButton.x + BUTTON_WIDTH / 2 - MeasureText("Quit", 20) / 2,
                                         quitButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);

                                // If user clicks on the respective difficulty button, assign accordingly.
                                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                {
                                        Vector2 mousePos = GetMousePosition();
                                        // If user clicks easy button.
                                        if (CheckCollisionPointRec(mousePos, easyDifficultyButton))
                                        {
                                                // Set AI to easy mode.
                                                aiDifficultySelected = 0;
                                                modalVisible = false; // Close the modal.
                                        }

                                        // If user clicks medium button.
                                        else if (CheckCollisionPointRec(mousePos, mediumDifficultyButton))
                                        {
                                                // Set AI to medium mode.
                                                Init_MachineLearning_AI();
                                                aiDifficultySelected = 1;
                                                modalVisible = false; // Close the modal.
                                        }

                                        // If user clicks hard button.
                                        else if (CheckCollisionPointRec(mousePos, hardDifficultyButton))
                                        {
                                                // Set AI to hard mode.
                                                aiDifficultySelected = 2;
                                                modalVisible = false; // Close the modal.
                                        }
                                        // If user clicks impossible button.
                                        else if (CheckCollisionPointRec(mousePos, impossibleDifficultyButton))
                                        {
                                                // Set AI to hard mode.
                                                aiDifficultySelected = 3;
                                                modalVisible = false; // Close the modal.
                                        }
                                }
                        }
                        break;
                // MULTIPLAYER page, player vs player tic tac toe game.
                case MULTIPLAYER:
                        // If modal is hidden.
                        if (!modalVisible)
                        {
                                // Draw background color.
                                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

                                // Display text of player names and their respective win counters.
                                DrawText(TextFormat("%s: %d | %s: %d", player1, player1WinCounter, player2, player2WinCounter), SCREEN_WIDTH / 3, SCREEN_HEIGHT / 11, 40, BLACK);

                                // Display which player's turn is it.
                                // If player 1 turn.
                                if (playerXTurn == true)
                                {
                                        DrawText(TextFormat("%s's turn. Your symbol is CROSS.", player1), SCREEN_WIDTH / 3, SCREEN_HEIGHT / 6, 25, BLACK);
                                }

                                // If player 2 turn.
                                else if (playerXTurn == false)
                                {
                                        DrawText(TextFormat("%s's turn. Your symbol is CIRCLE.", player2), SCREEN_WIDTH / 3, SCREEN_HEIGHT / 6, 25, BLACK);
                                }

                                // Display which player won the game or if tied game.
                                // If player 1 won the game.
                                if (gameOver == true && winner == 1)
                                {
                                        DrawText(TextFormat("%s won. Press enter to play again.", player1), SCREEN_WIDTH - 850, SCREEN_HEIGHT - 250, 25, BLACK);
                                }

                                // If player 2 won the game.
                                else if (gameOver == true && winner == 2)
                                {
                                        DrawText(TextFormat("%s won. Press enter to play again.", player2), SCREEN_WIDTH - 850, SCREEN_HEIGHT - 250, 25, BLACK);
                                }

                                // If tied game, no winners.
                                else if (gameOver == true && winner == 3)
                                {
                                        DrawText(TextFormat("Tie. Press enter to play again."), SCREEN_WIDTH - 850, SCREEN_HEIGHT - 250, 25, BLACK);
                                }

                                // Display quitButton to return to HOME page if user clicks.
                                DrawRectangleRec(quitButton, DARKBLUE);
                                DrawText("Quit", quitButton.x + BUTTON_WIDTH / 2 - MeasureText("Quit", 20) / 2,
                                         quitButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);

                                // Multiplayer tic tac toe game begins.
                                drawGrid(grid); // Draw the tic tac toe table.
                                // Tic tac toe game logic.
                                if (!gameOver)
                                {
                                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                        {
                                                int mouseX = GetMouseX();
                                                int mouseY = GetMouseY();

                                                if (mouseX >= 0 && mouseX < SCREEN_WIDTH && mouseY >= 0 && mouseY < SCREEN_HEIGHT)
                                                {
                                                        int cellX = (mouseX - ((SCREEN_WIDTH - (GRID_SIZE * 100)) / 2)) / 100;
                                                        int cellY = (mouseY - ((SCREEN_HEIGHT - (GRID_SIZE * 100)) / 2)) / 100;

                                                        if (grid[cellY][cellX] == EMPTY)
                                                        {
                                                                // If player 1 turn.
                                                                if (playerXTurn)
                                                                {
                                                                        setCell(grid, cellY, cellX, CROSS);
                                                                }

                                                                // If player 2 turn.
                                                                else
                                                                {
                                                                        setCell(grid, cellY, cellX, CIRCLE);
                                                                }

                                                                playerXTurn = !playerXTurn; // Switch turn to other player.

                                                                // If player 1 win.
                                                                if (checkWin(grid, CROSS))
                                                                {
                                                                        gameOver = true;        // End the game.
                                                                        player1WinCounter += 1; // Increment player 1 win counter.
                                                                        winner = 1;             // Set winner as player 1.
                                                                }

                                                                // If player 2 win.
                                                                else if (checkWin(grid, CIRCLE))
                                                                {
                                                                        gameOver = true;        // End the game.
                                                                        player2WinCounter += 1; // Increment player 2 win counter.
                                                                        winner = 2;             // Set winner as player 2.
                                                                }

                                                                else
                                                                {
                                                                        bool allCellsFilled = true;
                                                                        for (int i = 0; i < GRID_SIZE; i++)
                                                                        {
                                                                                for (int j = 0; j < GRID_SIZE; j++)
                                                                                {
                                                                                        if (grid[i][j] == EMPTY)
                                                                                        {
                                                                                                allCellsFilled = false;
                                                                                                break;
                                                                                        }
                                                                                }
                                                                                if (!allCellsFilled)
                                                                                        break;
                                                                        }

                                                                        // If no winners, tied game.
                                                                        if (allCellsFilled)
                                                                        {
                                                                                gameOver = true;
                                                                                winner = 3; // Set winner as nobody because tied game.
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }

                        // If modal is visible.
                        else if (modalVisible)
                        {
                                // Draw background color.
                                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

                                // Draw the label and user input text field.
                                Rectangle modalRec = {SCREEN_WIDTH / 2 - 235, SCREEN_HEIGHT / 2 - 100, 530, 120};
                                DrawRectangleRec(modalRec, RAYWHITE);
                                DrawRectangleLines(modalRec.x, modalRec.y, modalRec.width, modalRec.height, BLACK);
                                // Ask for user input player 1 name first followed by player 2 name.
                                if (playerNameCount == 1)
                                {
                                        DrawText("Please enter your name (player 1):", modalRec.x + 20, modalRec.y + 20, 25, BLACK);
                                }

                                else if (playerNameCount == 2)
                                {
                                        DrawText("Please enter your name (player 2):", modalRec.x + 20, modalRec.y + 20, 25, BLACK);
                                }
                                Rectangle textBoxRec = {modalRec.x + 20, modalRec.y + 50, modalRec.width - 40, 30};
                                DrawRectangleLines(textBoxRec.x, textBoxRec.y, textBoxRec.width, textBoxRec.height, BLACK);
                                DrawText("Press the enter key to continue", modalRec.x + 20, modalRec.y + 80, 17, BLACK);

                                // Logic for detecting what key the user presses and append it to the name array for storing.
                                int key = GetKeyPressed();
                                // If the user types something, append it to the name array accordingly.
                                if (key >= 32 && key <= 125)
                                {
                                        int len = strlen(name);
                                        if (len < 63)
                                        {
                                                name[len] = (char)key;
                                                name[len + 1] = '\0';
                                        }
                                }

                                // If the user presses the backspace key, delete the most recent char from the name array.
                                else if (key == KEY_BACKSPACE)
                                {
                                        int len = strlen(name);
                                        if (len > 0)
                                        {
                                                name[len - 1] = '\0';
                                        }
                                }

                                // If user presses the enter key, save name array value to the respective player name array.
                                else if (key == KEY_ENTER && playerNameCount == 1)
                                {
                                        strcpy(player1, name);
                                        name[0] = '\0'; // Clear name array to prepare for storing of player 2 name.
                                        playerNameCount += 1;
                                }

                                else if (key == KEY_ENTER && playerNameCount == 2)
                                {
                                        strcpy(player2, name);
                                        modalVisible = false; // Close the modal.
                                }

                                // Dynamically update the user input text field to display what the user types.
                                DrawText(name, textBoxRec.x + 5, textBoxRec.y + 5, 20, BLACK);
                        }

                        // Display quitButton to return to HOME page if user clicks.
                        DrawRectangleRec(quitButton, DARKBLUE);
                        DrawText("Quit", quitButton.x + BUTTON_WIDTH / 2 - MeasureText("Quit", 20) / 2,
                                 quitButton.y + BUTTON_HEIGHT / 2 - 10, 20, WHITE);
                        break;
                }

                // Check if user clicks a button or presses a key on keyboard.
                if (state == HOME && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                        Vector2 mousePos = GetMousePosition();
                        // If user clicks singleplayerButton on HOME page, navigate to SINGLEPLAYER page.
                        if (CheckCollisionPointRec(mousePos, singleplayerButton))
                        {
                                modalVisible = true; // Display modal to ask for user input to select AI difficulty.
                                state = SINGLEPLAYER;
                        }

                        // If user clicks multiplayerButton on HOME page, navigate to MULTIPLAYER page.
                        else if (CheckCollisionPointRec(mousePos, multiplayerButton))
                        {
                                modalVisible = true; // Display modal to ask for user input player names.
                                state = MULTIPLAYER;
                        }
                }

                // If user presses enter key on SINGLEPLAYER or MULTIPLAYER page after tic tac toe game ends, reset the tic tac toe game and play again.
                if ((state == SINGLEPLAYER || state == MULTIPLAYER) && gameOver == true && GetKeyPressed() == KEY_ENTER)
                {
                        // Clear and reset the tic tac toe grid to prepare for the next game.
                        for (int i = 0; i < GRID_SIZE; i++)
                        {
                                for (int j = 0; j < GRID_SIZE; j++)
                                {
                                        grid[i][j] = EMPTY;
                                }
                        }
                        winner = 0;       // Reset winner.
                        gameOver = false; // Reset game.
                }

                // If user clicks quitButton on SINGLEPLAYER or MULTIPLAYER page, navigate to HOME page.
                if ((state == SINGLEPLAYER || state == MULTIPLAYER) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                        Vector2 mousePos = GetMousePosition();
                        if (CheckCollisionPointRec(mousePos, quitButton))
                        {
                                name[0] = '\0';      // Clear name array.
                                playerNameCount = 1; // Reset logic for asking user input names of 2 players.
                                // Clear and reset the tic tac toe grid.
                                for (int i = 0; i < GRID_SIZE; i++)
                                {
                                        for (int j = 0; j < GRID_SIZE; j++)
                                        {
                                                grid[i][j] = EMPTY;
                                        }
                                }
                                // Reset win counters.
                                player1WinCounter = 0;
                                player2WinCounter = 0;
                                winner = 0;               // Reset winner.
                                aiDifficultySelected = -1; // Reset AI difficulty.
                                gameOver = false;         // Reset game.
                                state = HOME;
                                randomisePlayers(); // Randomise player start after they quit a game
                        }
                }
                EndDrawing();
        }

        // Clean up resources then exit program.
        CloseWindow();
        return 0;
}

// The function will randomise which player to start
void randomisePlayers()
{
        int randInt = rand() % 2; // Generate a random number either 0 or 1.
        if (randInt == 0)
        {
                playerXTurn = true; // Player 1 (CROSS) gets the first turn.
        }

        else if (randInt == 1)
        {
                playerXTurn = false; // Player 1 (CIRCLE) gets the second turn.
        }
}

// Function for the AI to determine its best move
move aiMakeMove(Cell grid[GRID_SIZE][GRID_SIZE])
{
        move bestMove;

        // calulateBestMove calculates the best possible move in the current position by passing in the variable "grid"
        bestMove = calculateBestMove(grid, bestMove);
        return bestMove;
}

move ImperfectaiMakeMove(Cell grid[GRID_SIZE][GRID_SIZE])
{
        move imperfectMove;
        double randomDouble;
        imperfectMove = calculateImperfectMove(grid, imperfectMove);
        return imperfectMove;
}

// Minimax algorithm is implemented in this function
int miniMax(Cell grid[GRID_SIZE][GRID_SIZE], int depth, bool isMaximiserTurn)
{
        int score = 0;

        // If the minimiser has won the game, return the evaluated score
        if (checkWin(grid, CROSS))
        {
                score = -10;
                return score - depth;
        }

        // If the maximiser has won the game, return the evaluated score
        else if (checkWin(grid, CIRCLE))
        {
                score = 10;
                return score - depth;
        }

        // If there are no empty cells remaining, the game is a tie and return 0
        if (movesRemaining(grid) == false)
                return 0;

        // If condition for the maximiser's turn
        if (isMaximiserTurn)
        {
                int bestScore = -1000;

                // Iterate over every cell on the board
                for (int i = 0; i < 3; i++)
                {
                        for (int j = 0; j < 3; j++)
                        {

                                // Determine if the cell is empty
                                if (grid[i][j] == EMPTY)
                                {
                                        grid[i][j] = CIRCLE;

                                        // miniMax function is called recursively
                                        // up until the AI finishes the game and returns a score
                                        int moveScore = miniMax(grid, depth + 1, !isMaximiserTurn);

                                        // The higher value is chosen in the case of the maximiser
                                        if (moveScore > bestScore)
                                        {
                                                bestScore = moveScore;
                                        }

                                        // Remove the symbol placed
                                        grid[i][j] = EMPTY;
                                }
                        }
                }
                return bestScore;
        }

        // else condition for the minimiser's turn
        else
        {
                int bestScore = 1000;

                // Iterate over every cell on the board
                for (int i = 0; i < 3; i++)
                {
                        for (int j = 0; j < 3; j++)
                        {

                                // Determine if the cell is empty
                                if (grid[i][j] == EMPTY)
                                {
                                        // Make the move
                                        grid[i][j] = CROSS;

                                        // miniMax function is called recursively
                                        // up until the AI finishes the game and returns a score
                                        int moveScore = miniMax(grid, depth + 1, !isMaximiserTurn);

                                        // The lower value is chosen in the case of the minimiser
                                        if (moveScore < bestScore)
                                        {
                                                bestScore = moveScore;
                                        }

                                        // Remove the symbol placed
                                        grid[i][j] = EMPTY;
                                }
                        }
                }
                return bestScore;
        }
}

// Function for for perfect minimax algorithm
move calculateBestMove(Cell grid[GRID_SIZE][GRID_SIZE], move bestMove)
{

        int bestScore = -1000;

        // Set row and column values
        bestMove.row = -1;
        bestMove.col = -1;

        // Iterate over every empty cell and evaluate the move if symbol is placed
        // there using the minimax function
        for (int i = 0; i < 3; i++)
        {
                for (int j = 0; j < 3; j++)
                {
                        // Find the first occurence of an empty cell and play the move
                        if (grid[i][j] == EMPTY)
                        {
                                // Make a move and evaluate it using minimax algorithm
                                grid[i][j] = CIRCLE;
                                int moveScore = miniMax(grid, 0, false);

                                // Remove the symbol placed
                                grid[i][j] = EMPTY;

                                // The if conidition determines if the score of the move played is higher than the best
                                if (moveScore > bestScore)
                                {
                                        // If true, set the coordinates for the best move
                                        bestMove.row = i;
                                        bestMove.col = j;
                                        bestScore = moveScore;
                                }
                        }
                }
        }
        return bestMove;
}

// imperfect minimax
move calculateImperfectMove(Cell grid[GRID_SIZE][GRID_SIZE], move imperfectMove)
{
        // Introduce randomness with a probability of 50%
        if (rand() % 2 == 0)
        {
                // Choose a random empty cell
                int emptyCells[GRID_SIZE * GRID_SIZE];
                int emptyCount = 0;
                for (int i = 0; i < GRID_SIZE; i++)
                {
                        for (int j = 0; j < GRID_SIZE; j++)
                        {
                                if (grid[i][j] == EMPTY)
                                {
                                        emptyCells[emptyCount++] = i * GRID_SIZE + j;
                                }
                        }
                }

                if (emptyCount > 0)
                {
                        int randomIndex = rand() % emptyCount;
                        int randomCell = emptyCells[randomIndex];
                        imperfectMove.row = randomCell / GRID_SIZE;
                        imperfectMove.col = randomCell % GRID_SIZE;

                        return imperfectMove;
                }
        }

        // If not choosing a random move, perform the minimax algorithm as before
        int bestVal = -1000;
        imperfectMove.row = -1;
        imperfectMove.col = -1;

        // Traverse all cells, evaluate minimax function for
        // all empty cells. And return the cell with optimal
        // value.
        for (int i = 0; i < 3; i++)
        {
                for (int j = 0; j < 3; j++)
                {
                        // Check if cell is empty
                        if (grid[i][j] == EMPTY)
                        {
                                // Make the move for the player
                                grid[i][j] = CIRCLE;

                                // compute evaluation function for this move.
                                int moveValue = miniMax(grid, 0, false);

                                // Undo the move
                                grid[i][j] = EMPTY;

                                // If the value of the current move is
                                // more than the best value, then update
                                // best/
                                if (moveValue > bestVal)
                                {
                                        imperfectMove.row = i;
                                        imperfectMove.col = j;
                                        bestVal = moveValue;
                                }
                        }
                }
        }

        return imperfectMove;
}

bool movesRemaining(Cell grid[GRID_SIZE][GRID_SIZE])
{
        for (int i = 0; i < 3; i++)
        {
                for (int j = 0; j < 3; j++)
                {
                        if (grid[i][j] == EMPTY)
                        {
                                return true;
                        }
                }
        }

        return false;
}

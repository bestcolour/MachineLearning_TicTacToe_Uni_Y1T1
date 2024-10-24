// The file that communicates with the game system on the next move the AI is going to make.
#include <stdio.h>
#include "PrepareData/prepare_data.h"
#include "TrainModel/train_model.h"
#include "training_settings.h"

#pragma region === Defitinition & Declaration ===

// Determines if this ML AI is initialised or not. 1 = true, 0 = false
static int _isInitialised = 0;

// Initializes the machine learning ai values and model. Will retrain model if model is missing.
void Init_MachineLearning_AI();

// Pass in current gamestate and returns chosen tile via pointers
void Decide_Next_Move(int gridSize, int currentBoard[gridSize][gridSize], int *x, int *y);
#pragma endregion

void Init_MachineLearning_AI()
{
    // Machine learning ai already initialised, return code flow
    if (_isInitialised == 1)
        return;

    _isInitialised = 1;

    // Initialise data prep
    // we will use the symbol from the settings ('O' in this case)
    Init_Data_Prep(SETTINGS_NUM_OF_DATALINES, SETTINGS_DATAFILE_PATH,SETTINGS_SYMBOL_THAT_AI_USE); 

    // Initialise Model Trainer and check for any missing values (1 means nothing wrong)
    if (Init_Model_Trainer(SETTINGS_NUM_OF_DATALINES, GAMESTATE_AS_INT_SIZE, SETTINGS_LEARNING_RATE, SETTINGS_ITERATIONS, SETTINGS_TRAINED_MODEL_FILEPATH) == 1)
        return;

    //--- Values missing from Model Trainer ---
    TicTacData *trainingData_pointer = Read_Data();                                                 // Grab data from data file
    FlatTicTacData flatData = Flatten_TicTac_Data(trainingData_pointer, SETTINGS_NUM_OF_DATALINES); // Make data usable for training model
    Retrain_Model(flatData.allGameStateAsInt_pointer, flatData.allIsPositive_pointer);              // Retrain model

    free(trainingData_pointer); // Free memory just incase
}

void Decide_Next_Move(int gridSize, int currentBoard[gridSize][gridSize], int *x, int *y)
{
    int row, col, i;
    int bestMoveToMake = -1;
    int originalSetOfThree[3] = {0, 0, 0};
    double bestProbability = -1, tempProbability;
    int flatCurrBoardData[GAMESTATE_AS_INT_SIZE] = {}; // holds the current board data in model trainable form

#pragma region Flatten currentBoard Data
    // Using the knowledge that 0 = blank, 1 = cross and 2 = circle,
    // we can convert this data into the data needed for the model prediction
    for (row = 0; row < gridSize; row += 1)
    {
        for (col = 0; col < gridSize; col += 1)
        {
            // printf("Grid[%d][%d] = %d\n", row, col, currentBoard[row][col]); //debug
            i = (row * gridSize + col) * 3; // To get the correct element index in the flattened array data

            switch (currentBoard[row][col])
            {

            case 0: // when we see currentBoard[row][col] = 0 (blank), we assign a value of 0,0,1 to our flattened data array
                flatCurrBoardData[i] = 0;
                flatCurrBoardData[i + 1] = 0;
                flatCurrBoardData[i + 2] = 1;
                break;

            case 1:
                // when we see currentBoard[row][col] = 1 (cross), we assign a value of 1,0,0 to our flattened data array
                flatCurrBoardData[i] = 1;
                flatCurrBoardData[i + 1] = 0;
                flatCurrBoardData[i + 2] = 0;
                break;

            case 2:
                // when we see currentBoard[row][col] = 2 (circle), we assign a value of 0,1,0 to our flattened data array
                flatCurrBoardData[i] = 0;
                flatCurrBoardData[i + 1] = 1;
                flatCurrBoardData[i + 2] = 0;
                break;

            default:
                printf("Unrecognised case: %d !\n", currentBoard[row][col]);
                break;
            }
        }
    }

#pragma endregion

#pragma region Deciding the Best Move to Make
    // Loop thru all sets of 3 in the current gamestate
    for (i = 0; i < GAMESTATE_AS_INT_SIZE; i += 3)
    {
        // Since i increments by 3 every loop, i will be the index of the first set of 3
        // Check if current cell is blank (representation of blank is 001)
        if (flatCurrBoardData[i] != 0 || flatCurrBoardData[i + 1] != 0 || flatCurrBoardData[i + 2] != 1)
        {
            continue;
        }

        // Record the discovered blank cell
        originalSetOfThree[0] = flatCurrBoardData[i];
        originalSetOfThree[1] = flatCurrBoardData[i + 1];
        originalSetOfThree[2] = flatCurrBoardData[i + 2];

        // --- Create a scenario where the current blank cell in the loop is chosen ---
        // Change the set of 3 starting from 'i' into the symbol which the AI uses which is O, which is represented by 0,1,0
        flatCurrBoardData[i] = 0;
        flatCurrBoardData[i + 1] = 1;
        flatCurrBoardData[i + 2] = 0;

        // --- Predict using the new gamestate to see if it is the best chance of winning ---
        // Call Predict() and compare the probability returned if it is higher than the current one
        tempProbability = Predict(flatCurrBoardData);
        // if the new prediction probability of winning is better than the previous one,
        if (tempProbability > bestProbability)
        {
            // set that as the best probability to win
            bestProbability = tempProbability;
            // Record the current best gamestate to move
            bestMoveToMake = i;
        }

        // Revert the board to its original scenario
        flatCurrBoardData[i] = originalSetOfThree[0];
        flatCurrBoardData[i + 1] = originalSetOfThree[1];
        flatCurrBoardData[i + 2] = originalSetOfThree[2];
    }
#pragma endregion

    // reverse the equation i = (row * gridSize + col) * 3 to find the cell number
    bestMoveToMake /= 3;
    *y = bestMoveToMake % gridSize;        // col index = remainder of cell index / gridSize (in this case gridSize = 3)
    *x = (bestMoveToMake - *y) / gridSize; // row index = (cell index - remainder) gridSize
}

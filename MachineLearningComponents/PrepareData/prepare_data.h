// Reads the data from the file and converts it into an usable data array. Returns the pointer to the array
#pragma region === Declarations & Definitions ===
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#pragma region--- Constants ---
// Tic tac toe constants
#define NUM_OF_TICTAC_BOXES 9                          // Number of tic tac toe boxes in a grid is 3 x 3 = 9
#define NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE 3 // The number of symbols a square could have in (could be X, O or blank)
#define GAMESTATE_AS_INT_SIZE 27                       // Size is 9 x 3 = 27, read GameStateAsIntegers for explaination
#define CLASSIFICATION_OUTPUT_TRUE "positive"          // What label constitutes a value of 1 else it will be 0

// --- String Constants ---
#define MAX_BUFFER_SIZE 256
#pragma endregion

#pragma region--- Static Vars ---
// Number of data instances in the data file you are trying to extract from
static int _numOfDataRows;

// Returns the number of data instances in the data file you are trying to extract from
int Get_Num_Of_DataRows();
int Get_Num_Of_DataRows() { return _numOfDataRows; }

// Data file path that we need to extract data from
static char _dataFilePath[MAX_BUFFER_SIZE];
// The symbol that the ai uses to make its move in tic tac toe. Must be either 'O' or 'X'
static char _symbolThatAIUse;
#pragma endregion

#pragma region--- Struct Definitions ---
// A struct that holds the readible version of the data struct for each line of the datafile
typedef struct
{
    // If label of the data instance is "positive", the value will be 1 else -1
    int IsPositive;
    /*GameState represents the board's current placement of x, o and blanks. GameStateAsIntegers represents the GameState in the form integers.

    Each element in the array 'GameStateAsIntegers' will contain a set of 27 numbers:

    'GameStateAsIntegers = {0,1,0, 1,0,0, ...}'

    where the first 3 elements of GameStateAsIntegers '0,1,0' represents the symbol on the first square, '1,0,0' represents the symbol on the 2nd square and so on until the 9th square (which results in this array being 9x3 = 27 elements long)

    The set of 3 numbers represents the symbol being used for that square:

    1,0,0 being X
    0,1,0 being O
    0,0,1 being blank
   */
    int GameStateAsIntegers[GAMESTATE_AS_INT_SIZE];
} TicTacData;

/*
The struct that holds the setA_pointer and setB_pointer that has been randomly selected without any duplicate elements in each of the two arrays.
*/
typedef struct
{
    // setA_pointer is the set of data which is the ratioA portion of the full dataset
    TicTacData *setA_pointer;
    // setB_pointer is the set of data which is the ratioB portion of the full dataset
    TicTacData *setB_pointer;
} SplitTicTacDatas;

/*
The version of TicTacData which was flattened for the training model to be able to read it easily.
*/
typedef struct
{
    // The int array that keeps all of the IsPositives from a dataset
    int *allIsPositive_pointer;
    // The int array that keeps all of the GameStateAsInt from a dataset
    int *allGameStateAsInt_pointer;
} FlatTicTacData;
#pragma endregion

#pragma endregion

#pragma region === Function Prototypes  ===
// Reads the data from the file and converts it into an usable data array. Returns the pointer to the array
TicTacData *Read_Data();
#pragma region--- Conversion Methods ---

/*
Converts an array ot TicTacData into two arrays of int (returns value via the passed in pointers).

For example:
TicTacData * dataSet_pointer (with 3 elements inside) will be converted into two arrays of int with:

- {0,1,0, 0,1,0, 0,1,0, ..., until the end of 3 x 27 elements} <- GameStateAsIntegers of 3 elements
- {1, 0, 1} <- IsPositive of 3 elements

*/
FlatTicTacData Flatten_TicTac_Data(TicTacData *dataset, int size);

#pragma region Splitting Methods
/*
Returns a struct that holds 2 sets of TicTacData arrays that when merged together gives the full dataset.

Ratio A: The portion of the full dataset which will belong to set A while everything else goes to set B
*/
SplitTicTacDatas Get_Split_Datasets(float ratioA);

// Shuffles an array of the TicTacData type
void _shuffle_data_array(TicTacData array[]);
#pragma endregion

#pragma region Ease For Reading Methods
// Converts a symbol "x", "o" or "b" into an array of values {1,0,0} , {0,1,0} , {0,0,1} respectively. Value is returned in the array passed in as parameter
void _convert_tictactoe_symbol_to_value(int boxSymbolAsValue[NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE], char symbol);

// Converts an array of values {1,0,0} , {0,1,0} , {0,0,1} to "x", "o" or "b" respectively. Char value is returned.
char _convert_tictactoe_value_to_symbol(int boxSymbolAsValue[NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE]);

// Prints an array of TicTacData in readible form, usually used for debugging
void _print_tictacdata_array(TicTacData ticTacData[], int size);

// Prints TicTacData in readible form
void _print_tictacdata(TicTacData ticTacData);

#pragma endregion

#pragma endregion

#pragma endregion

void Init_Data_Prep(int numOfDataRows, char *dataFilePath, char symbolAIUse)
{
    // Set values of the static vars
    _numOfDataRows = numOfDataRows;
    strcpy(_dataFilePath, dataFilePath);
    _symbolThatAIUse = symbolAIUse;
}

TicTacData *Read_Data()
{
    // --- Initialise Vars ---
    TicTacData *ticTacData_pointer = malloc(sizeof(TicTacData) * _numOfDataRows);

    int i, j, k;
    int tempArray[NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE] = {0, 0, 0}; // Using tempArray to assign values to GameStateAsIntegers in sets of 3. Read GameStateAsIntegers for explaination of tempArray

    char stringBuffer[MAX_BUFFER_SIZE]; // The temp variable allocated to the data to be loaded into for printf later on

    // --- Reading File ---
    FILE *dataFile_pointer = fopen(_dataFilePath, "r");

    // --- Guard Ifs ---
    if (dataFile_pointer == NULL) // Exit function if no file exists
    {
        printf("No data read as there is no such file called \"%s\"\n", _dataFilePath);
        return NULL;
    }

    // --- Read and Store Data ---
    // For every line of data in the tic-tac-toe.data file,
    for (i = 0; i < _numOfDataRows; i++)
    {
        // Load next line of text data file, if next line is NULL, exit loop
        if (fgets(stringBuffer, MAX_BUFFER_SIZE, dataFile_pointer) == NULL)
        {
            break;
        }

#pragma region Reading for First Set of Elements
        // This code is not inside the for loop as the function "strtok" uses different parameters for the first Set of Elements than the rest

        // Split the current string text based on "." (and load it into the strtok function)
        char *token = strtok(stringBuffer, ",");
        _convert_tictactoe_symbol_to_value(tempArray, token[0]); // Store the 1st symbol found from line of data
        // Assign tempArray values to the first 3 elements of GameStateAsIntegers
        ticTacData_pointer[i].GameStateAsIntegers[0] = tempArray[0];
        ticTacData_pointer[i].GameStateAsIntegers[1] = tempArray[1];
        ticTacData_pointer[i].GameStateAsIntegers[2] = tempArray[2];
#pragma endregion

        for (j = 1; j < NUM_OF_TICTAC_BOXES; j++) // make sure that the first tictacbox is skipped by setting j = 1
        {
            // Get next symbol that was split by ","
            token = strtok(NULL, ",");
            _convert_tictactoe_symbol_to_value(tempArray, token[0]); // Store the symbol found from line of data

            // printf(" %s\n", token); // printing each token

            // Assign the respective GameStateAsIntegers elements with the values of tempArray
            for (k = 0; k < NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE; k++)
                ticTacData_pointer[i].GameStateAsIntegers[j * NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE + k] = tempArray[k];
        }

        // Get final string that was split by "," (which will be the positive or negative)
        token = strtok(NULL, ",");

        //Since the ai trains on data where the ai is the player that uses 'X', we can just treat positive as 1 and negative as -1
        if (_symbolThatAIUse == 'X')
        {
            // if the final string text is 'positive', then set IsPositive to true else false
            ticTacData_pointer[i].IsPositive = strcmp(token, CLASSIFICATION_OUTPUT_TRUE) == 0 ? 1 : -1;
        }
        //Since the ai trains on data where the ai is the player that uses 'X', we need to flip the label values to simulate the ai is the player 'O'
        else
        {
            // if the final string text is 'positive', then set IsPositive to false else true
            ticTacData_pointer[i].IsPositive = strcmp(token, CLASSIFICATION_OUTPUT_TRUE) == 0 ? -1 : 1;
        }

        // Debug
        // printf("%s", stringBuffer);
    }

    // debug
    // _print_tictacdata(ticTacData_pointer);

    fclose(dataFile_pointer); // Close file stream
    return ticTacData_pointer;
}

#pragma region === Conversion Methods ===

FlatTicTacData Flatten_TicTac_Data(TicTacData *dataset, int size)
{
    FlatTicTacData flatData;
    flatData.allGameStateAsInt_pointer = malloc(sizeof(int) * size * GAMESTATE_AS_INT_SIZE);
    flatData.allIsPositive_pointer = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < GAMESTATE_AS_INT_SIZE; j++)
        {
            // Copy all of the gamestate as integers into allGameStateAsInt_pointer array
            flatData.allGameStateAsInt_pointer[i * GAMESTATE_AS_INT_SIZE + j] = dataset[i].GameStateAsIntegers[j];
        }

        // Copy all of the isPositive values into allIsPositive_pointer array
        flatData.allIsPositive_pointer[i] = dataset[i].IsPositive;
    }

    return flatData;
}

#pragma region--- Splitting Methods ---
SplitTicTacDatas Get_Split_Datasets(float ratioA)
{
    SplitTicTacDatas datasets;                 // Create dataset to return
    TicTacData *dataset_pointer = Read_Data(); // Read from "tic-tac-toe.data" and store the values into an array
    _shuffle_data_array(dataset_pointer);      // Shuffle the array

    int i, j,
        range = ratioA * _numOfDataRows; // Get the number of elements in set A

    // --- Populate set A array ---
    datasets.setA_pointer = malloc(sizeof(TicTacData) * range); // Allocate memory for set A
    for (i = 0; i < range; i++)
        datasets.setA_pointer[i] = dataset_pointer[i]; // Copy over the dataset element into set A

    // --- Populate set B array ---
    range = _numOfDataRows - range;                             // Get the number of elements in the set B
    datasets.setB_pointer = malloc(sizeof(TicTacData) * range); // Allocate memory for set B

    for (j = 0; j < range; i++, j++)                   // Start i after the previous element of dataset_pointer that was copied to set A
        datasets.setB_pointer[j] = dataset_pointer[i]; // Copy over the dataset element into set B

    free(dataset_pointer);
    return datasets;
}

void _shuffle_data_array(TicTacData array[])
{
    srand(time(NULL));
    int i, j;
    TicTacData temp;
    for (i = _numOfDataRows - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        // Swap array[i] and array[j]
        TicTacData temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
#pragma endregion

#pragma region--- Ease For Reading Methods ---
void _convert_tictactoe_symbol_to_value(int boxSymbolAsValue[NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE], char symbol)
{
    boxSymbolAsValue[0] = 0;
    boxSymbolAsValue[1] = 0;
    boxSymbolAsValue[2] = 0;
    switch (symbol)
    {
    case 'x':
        boxSymbolAsValue[0] = 1;
        break;
    case 'o':
        boxSymbolAsValue[1] = 1;
        break;

    case 'b':
        boxSymbolAsValue[2] = 1;
        break;

    default:
        printf("Unrecognised symbol in loading data from datafile!: \'%c\' \n", symbol);
        break;
    }
}

char _convert_tictactoe_value_to_symbol(int boxSymbolAsValue[NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE])
{
    if (boxSymbolAsValue[0] == 1)
        return 'x';

    if (boxSymbolAsValue[1] == 1)
        return 'o';

    if (boxSymbolAsValue[2] == 1)
        return 'b';

    printf("Unrecognised symbol in loading data from datafile!: [%d, %d, %d] \n"), boxSymbolAsValue[0], boxSymbolAsValue[1], boxSymbolAsValue[2];
}

void _print_tictacdata_array(TicTacData ticTacData[], int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("--- Values for Data Row (%d) ---\n", i + 1);
        _print_tictacdata(ticTacData[i]);
    }
}

void _print_tictacdata(TicTacData ticTacData)
{
    char stringBuffer[MAX_BUFFER_SIZE];                                      // Declare buffer for holding the line to be printed
    char charToString[2] = {'c', '\0'};                                      // Declare charToString to convert a character to a minimum string
    int tempArray[NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE] = {0, 0, 0}; // Using tempArray extract values from GameStateAsIntegers in sets of 3. Read GameStateAsIntegers for explaination of tempArray
    printf("The board picture is:\n");

    for (int j = 0; j < NUM_OF_TICTAC_BOXES; j++)
    {
        for (int i = 0; i < NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE; i++)
        {
            tempArray[i] = ticTacData.GameStateAsIntegers[j * NUM_OF_POSSIBLE_SYMBOLS_FOR_TICTACTOE_SQUARE + i];
        }

        // Convert char to string
        charToString[0] = _convert_tictactoe_value_to_symbol(tempArray); // We must -1 here because j starts from 1 to allow for the modulo check below
        strcat(stringBuffer, charToString);                              // Add char to string to be printed
        strcat(stringBuffer, " ");                                       // Add space to string to be printed

        if ((j + 1) % 3 == 0) // If j is divisible by 3, we print current line and go to next line
        {
            strcat(stringBuffer, "\n");
            printf(stringBuffer);   // print out a line
            stringBuffer[0] = '\0'; // Clear buffer
        }
    }

    // Print isWin value
    charToString[0] = ticTacData.IsPositive == 1 ? '+' : '-'; // Print out '+' if label is positive else '-'
    printf("Is Positive value: %c\n", charToString[0]);
}
#pragma endregion

#pragma endregion

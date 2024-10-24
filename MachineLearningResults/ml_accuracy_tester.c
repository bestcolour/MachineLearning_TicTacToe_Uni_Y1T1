#include <stdio.h>
#include "../MachineLearningComponents/PrepareData/prepare_data.h"
#include "../MachineLearningComponents/TrainModel/train_model.h"
#include "../MachineLearningComponents/training_settings.h"

// --- Test & Training Set Lengths ---
#define SPLIT_TEST_SET_RATIO 0.2
#define ACCURACY_TESTER_TRAINED_MODEL_PATH "./accuracy_test_trained_model.txt"
#define AI_SYMBOL 'X'
#define MODIFIED_DATAFILE_PATH "../MachineLearningComponents/PrepareData/tic-tac-toe.data"

int Get_Test_Set_Length();
int Get_Test_Set_Length() { return SETTINGS_NUM_OF_DATALINES * SPLIT_TEST_SET_RATIO; }
// Returns the number of rows designated for training the machine learning model.
int Get_Training_Set_Length();
int Get_Training_Set_Length() { return SETTINGS_NUM_OF_DATALINES - Get_Test_Set_Length(); }

/*
Prints get the probability that the data instances from the flattenedData_pointer are good/bad moves and the results.
   This function will print out the following:
   - Probability of error
   - Accuracy of prediction
   - Number of True Positive
   - Number of True Negative
   - Number of False Positive
   - Number of False Negative

*/
void _print_accuracy(int *flattenedData_pointer, int count);

int main()
{
    int i;

    // We will assume that the ai is player 'X' as the dataset is used with the intention that the ai is 'X'

    // Initialise data prepare
    Init_Data_Prep(SETTINGS_NUM_OF_DATALINES, MODIFIED_DATAFILE_PATH, AI_SYMBOL);

    // Get the full dataset and split it into 2 sets according to test set ratio
    // Set A is Test Set and Set B is Training Set
    SplitTicTacDatas splitDataSets = Get_Split_Datasets(SPLIT_TEST_SET_RATIO);

    // Initialise Model Trainer and get the returned value to check if retraining of the model is needed
    Init_Model_Trainer(SETTINGS_NUM_OF_DATALINES, GAMESTATE_AS_INT_SIZE, SETTINGS_LEARNING_RATE, SETTINGS_ITERATIONS, ACCURACY_TESTER_TRAINED_MODEL_PATH);
    FlatTicTacData flatData = Flatten_TicTac_Data(splitDataSets.setB_pointer, Get_Training_Set_Length()); // Make data usable for training model
    Retrain_Model(flatData.allGameStateAsInt_pointer, flatData.allIsPositive_pointer);                    // Retrain model

#pragma region ===== Accuracy of Prediction on Test Set =====

    // Calculate number of data instances in test set
    i = Get_Test_Set_Length();
    // Flatten test dataset (Set A is Test Set) to make data usable for training model
    flatData = Flatten_TicTac_Data(splitDataSets.setA_pointer, i);

    printf("========== Prediction Accuracy of Test Set ========== \n");
    _print_accuracy(flatData.allGameStateAsInt_pointer, i); // Print accuracy
#pragma endregion

#pragma region ===== Accuracy of Prediction on Training Set =====

    // Calculate number of data instances in test set
    i = Get_Training_Set_Length();
    // Flatten test dataset (Set B is training Set) to make data usable for training model
    flatData = Flatten_TicTac_Data(splitDataSets.setB_pointer, i);

    printf("========== Prediction Accuracy of Training Set ========== \n");
    _print_accuracy(flatData.allGameStateAsInt_pointer, i); // Print accuracy
#pragma endregion
    printf("Press Enter to exit\n");
    getchar();
    return 0;
}

void _print_accuracy(int *flattenedData_pointer, int count)
{

    double probability;
    int gameBoardAsInt[27];
    char predictionLabel[9];
    char actualLabel[9];
    int
        correctPredictions = 0,
        wrongPredictions = 0,
        truePosPredictions = 0,
        trueNegPredictions = 0,
        falsePosPredictions = 0,
        falseNegPredictions = 0;
    int percentage;

    // Create a loop to test the trained weights using the predict function and test dataset
    for (size_t i = 0; i < count; i++) // For every test set's data instance
    {
        // Copy the gameStateAsInt array element into temp array
        for (size_t j = 0; j < GAMESTATE_AS_INT_SIZE; j++)
        {
            gameBoardAsInt[j] = flattenedData_pointer[i * GAMESTATE_AS_INT_SIZE + j];
        }

        // Predict the test data instance
        probability = Predict(gameBoardAsInt);
        // We consider the predicted label to be positive if the probability is higher than 50%
        if (probability >= 0.5)
            strcpy(predictionLabel, "positive");
        else
            strcpy(predictionLabel, "negative");

        // Print out the actual label based on the value
        if (flattenedData_pointer[i] == 1)
            strcpy(actualLabel, "positive");
        else
            strcpy(actualLabel, "negative");

#pragma region------ Determining Results ------
        // If prediction label  and actual label are both the same,
        if (strcmp(predictionLabel, actualLabel) == 0)
        {
            // It will either be a true positive or true negative scenario, so we check if the prediction label is 'positive'
            if (strcmp(predictionLabel, "positive") == 0)
                truePosPredictions++; // This means that it is a true positive scenario
            else
                trueNegPredictions++; // Else it means it is a true negative scenario
        }
        else
        {
            // It will either be a false positive or false negative scenario, so we check if the prediction label is 'positive'
            if (strcmp(predictionLabel, "positive") == 0)
                falsePosPredictions++; // This means that it is a false positive scenario
            else
                falseNegPredictions++; // Else it means it is a false negative scenario
        }
#pragma endregion

    }

    correctPredictions = trueNegPredictions + truePosPredictions;
    wrongPredictions = count - correctPredictions;
    printf("                                                  Actual Values                     \n");
    printf("                                                                                    \n");
    printf("                                       Positive                   Negative          \n");
    printf("                              ------------------------------------------------------\n");
    printf("                              |                         |                          |\n");
    printf("                              |      True Positive      |       False Positive     |\n");
    printf("                    Positive  |            =            |             =            |\n");
    printf("                                           %d                         %d            \n", truePosPredictions, falsePosPredictions);
    printf("       Predicted              |                         |                          |\n");
    printf("        Values                --------------------------|---------------------------\n");
    printf("                              |                         |                          |\n");
    printf("                              |      True Negative      |       False Negative     |\n");
    printf("                    Negative  |            =            |             =            |\n");
    printf("                                           %d                         %d            \n", trueNegPredictions, falseNegPredictions);
    printf("                              |                         |                          |\n");
    printf("                              ------------------------------------------------------\n");

    percentage=(((float)wrongPredictions)/count)*100;
    printf("The probability of error is %d/%d = %d\%\n", wrongPredictions,count , percentage);
    percentage=100-percentage;
    printf("The accuracy is %d/%d = %d\%\n", correctPredictions, count,percentage);
    printf("The Number of True Positive is %d\n", truePosPredictions);
    printf("The Number of True Negative is %d\n", trueNegPredictions);
    printf("The Number of False Positive is %d\n", falsePosPredictions);
    printf("The Number of False Negative is %d\n", falseNegPredictions);
}
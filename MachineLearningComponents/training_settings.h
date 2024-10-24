/*
This file holds constants values that most likely be changed by the user. 
*/


// ============== Prep Data ================
// There is 958 lines in the data file. Change this if number of lines changes.
#define SETTINGS_NUM_OF_DATALINES 958
//The path to the tic tac toe data
#define SETTINGS_DATAFILE_PATH "MachineLearningComponents/PrepareData/tic-tac-toe.data"

/*
The symbol that the ai uses, must be 'O' or 'X'

By changing this value, you have to delete the SETTINGS_TRAINED_MODEL_FILEPATH file and retrain the model again (Call Init_MachineLearning_AI() will do the trick)
*/
#define SETTINGS_SYMBOL_THAT_AI_USE 'O'

//============== Machine Learning =================
//By changing any of these values, you have to delete the SETTINGS_TRAINED_MODEL_FILEPATH file and retrain the model again (Call Init_MachineLearning_AI() will do the trick)
#define SETTINGS_LEARNING_RATE 0.0000005
#define SETTINGS_ITERATIONS 1000
#define SETTINGS_TRAINED_MODEL_FILEPATH "MachineLearningComponents/TrainModel/trained_model.txt"
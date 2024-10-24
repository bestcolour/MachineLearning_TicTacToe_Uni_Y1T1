#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#pragma region === Definition & Declaration ===
static float _learningRate  = 0.001;
static int _iterations =1000;

// Number of features that is expected
static int _featureSize = 27;

// Number of samples that is expected
static int _sampleSize = 2;

//"Array" that holds the values of the trained weights that is a result of iterating over the data samples.
static double *_trainedWeights_pointer = NULL;

static char _trainedModelFilePath[] = "./trained_model.txt";

/*
Initializes the static variables of the script. Must be called everytime you want to train a new logistic regression model with different sample size or feature size

Returns 0 if trained weights is missing, proceed with Retrain_Model() to retrain the weights, else return 1
*/
int Init_Model_Trainer(int sampleSize, int featureSize,float learningRate,int iterations, char * trainedModelFilePath);



// Returns a probability that indicates whether the inputed feature is "good". The closer the probability is to 1, the better.
double Predict(int *features);

// Returns a probability that indicates whether the inputed feature is "good". The closer the probability is to 1, the better.
double _predict(int *features, double *weights);

#pragma region--- Training Functions ---
// Function to calculate the sigmoid function used in training the logistic regression
double _sigmoid(double z);

// Function to initialize weights with small random values
void _initialize_weights(double *weights);

// Function to train the logistic regression model
void _train_model(int *features, int *labels, double *weights);

// Ensures that the model has been trained and asccessible
void Retrain_Model(int *features, int *labelData);
#pragma endregion

#pragma region--- Saving & Loading ---
// Writes the weights value into a .txt file
void _save_weights(int sampleSize, int featureSize, double *weights);

// Loads the weights into a static variable from .txt file . Returns 1 if file is found and successfully loaded, else return 0 if error occurs
int _load_weights();
#pragma endregion

#pragma endregion

int Init_Model_Trainer(int sampleSize, int featureSize,float learningRate,int iterations, char * trainedModelFilePath)
{
    _sampleSize = sampleSize;
    _featureSize = featureSize;
    _learningRate = learningRate;
    _iterations = iterations;
    strcpy(_trainedModelFilePath,trainedModelFilePath);

    // Check if model is missing or not
    return _load_weights();
}

double Predict(int *features)
{
    return _predict(features, _trainedWeights_pointer);
}

double _predict(int *features, double *weights)
{
    double prediction = 0.0;
    for (int feature = 0; feature < _featureSize; ++feature)
    {
        prediction += weights[feature] * features[feature];
    }

    return _sigmoid(prediction);
}


#pragma region --- Training Functions ---
void Retrain_Model(int *features, int *labelData)
{
    printf("Retraining model... Please wait moment\n");

    // Else if trained_model.txt doesnt exists re-train model and save it into .txt before loading it
    //  Initialize weights
    _initialize_weights(_trainedWeights_pointer);

    // Train the logistic regression model
    _train_model(features, labelData, _trainedWeights_pointer);

    // Write the model into a .txt file
    _save_weights(_sampleSize, _featureSize, _trainedWeights_pointer);
}


double _sigmoid(double z)
{
    return 1.0 / (1.0 + exp(-z));
}

void _initialize_weights(double *weights)
{
    for (int i = 0; i < _featureSize; ++i)
    {
        weights[i] = ((double)rand() / RAND_MAX) * 0.1;
    }
}

void _train_model(int *features, int *labels, double *weights)
{
    for (int iter = 0; iter < _iterations; ++iter)
    {
        for (int sample = 0; sample < _sampleSize; ++sample) // For each data row,
        {
            double prediction = 0.0;
            for (int feature = 0; feature < _featureSize; ++feature)
            {
                prediction += weights[feature] * features[sample * _featureSize + feature]; // Get the summation of each feature * weight
            }
            double output = _sigmoid(prediction);   // and push it into the sigmoid equation
            double error = labels[sample] - output; // then calculate error base on classification label

            // Update weights using gradient descent
            for (int feature = 0; feature < _featureSize; ++feature)
            {
                weights[feature] += _learningRate * error * output * (1.0 - output) * features[sample * _featureSize + feature];
            }
        }
    }
}
#pragma endregion


#pragma region --- Saving & Loading Functions ---

void _save_weights(int sampleSize, int featureSize, double *weights)
{
    FILE *file_pointer = fopen(_trainedModelFilePath, "w"); // Open or create file path
    char tempString[255];

    const char TRAINED_WEIGHT_FORMAT_SPECIFIER[] = "%.15f";

    for (int i = 0; i < featureSize; i++)
    {
        int stringLength = snprintf(NULL, 0, TRAINED_WEIGHT_FORMAT_SPECIFIER, weights[i]); // Check string length of weight

        snprintf(tempString, stringLength, TRAINED_WEIGHT_FORMAT_SPECIFIER, weights[i]); // converts weight[i] into string

        tempString[stringLength - 1] = '\n'; // Replace '\0' with '\n' skip one line
        tempString[stringLength] = '\0';     // Make sure there is no other char to be written into line after this

        fprintf(file_pointer, tempString); // Write the string into file

        // printf("%d) Weight: %0.15f Size: %d\n", i, weights[i], stringLength);
    }

    fclose(file_pointer); // Close file
}

int _load_weights()
{
    // Free array if there is any
    free(_trainedWeights_pointer);
    _trainedWeights_pointer = malloc(sizeof(double) * _featureSize); // realloc memory for this array

    // Reading files
    FILE *file_pointer = fopen(_trainedModelFilePath, "r");
    char buffer[255];

    // No file found
    if (file_pointer == NULL)
    {
        printf("There is no such file called \"%s\"\n", _trainedModelFilePath);
        return 0; // Return false
    }

    // Read up to 255 chars into the buffer array. Will continue down the line with every fgets call until it returns null
    for (size_t i = 0; i < _featureSize; i++)
    {
        // If the end of the .txt has been reached, exit
        if (fgets(buffer, 255, file_pointer) == NULL)
            break;

        sscanf(buffer, "%lf", &_trainedWeights_pointer[i]);
        // printf("%.15f\n",_trainedWeights_pointer[i]);

        // printf("%s", buffer);
    }

    fclose(file_pointer);

    return 1; // return true
}
#pragma endregion

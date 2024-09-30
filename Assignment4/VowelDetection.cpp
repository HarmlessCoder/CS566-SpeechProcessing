#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <cstring>
using namespace std;

#define LPC_ORDER 12  // Order of LPC
#define CONST_PI 3.1428
#define FRAME_SIZE 320  // Number of samples per frame
#define NUM_TOTAL_FRAMES 5  // Number of frames in your example (adjust as needed)

// Global variables
int totalSampleCount = 0;
double processedSignal[NUM_TOTAL_FRAMES][FRAME_SIZE];
double autoCorrelation[NUM_TOTAL_FRAMES][LPC_ORDER + 1];
double LPC_Coefficients[NUM_TOTAL_FRAMES][LPC_ORDER + 1];
double cepstralCoefficients[NUM_TOTAL_FRAMES][LPC_ORDER + 1];  // Cepstral coefficients
double weightedCepstralCoeffs[NUM_TOTAL_FRAMES][LPC_ORDER + 1];
double savedCepstralCoeffs[NUM_TOTAL_FRAMES][LPC_ORDER + 1];
double inputSignal[20000] = {0.0};
double tokhuraWeightFactors[LPC_ORDER] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};
double frameDistances[NUM_TOTAL_FRAMES];

// Function to apply Hamming window to a frame
void ApplyHamming(int frameIndex) {
    for (int n = 0; n < FRAME_SIZE; n++) {
        processedSignal[frameIndex][n] *= 0.54 - 0.46 * cos(2 * CONST_PI * n / (FRAME_SIZE - 1));
    }
}

// Function to compute autocorrelation
void ComputeAutoCorr(int frameIndex) {
    for (int k = 0; k <= LPC_ORDER; k++) {
        autoCorrelation[frameIndex][k] = 0.0;
        for (int n = 0; n < FRAME_SIZE - k; n++) {
            autoCorrelation[frameIndex][k] += processedSignal[frameIndex][n] * processedSignal[frameIndex][n + k];
        }
    }
}

// Function to correct DC offset
void AdjustDCOffset() {
    double avg = 0.0;
    for (int i = 0; i < totalSampleCount; i++) {
        avg += inputSignal[i];
    }
    avg /= totalSampleCount;

    for (int i = 0; i < totalSampleCount; i++) {
        inputSignal[i] -= avg;
    }
}

// Function to normalize signal values
void ScaleSignal() {
    double minValue = inputSignal[0];
    double maxValue = inputSignal[0];

    for (int i = 1; i < totalSampleCount; i++) {
        if (inputSignal[i] < minValue) {
            minValue = inputSignal[i];
        }
        if (inputSignal[i] > maxValue) {
            maxValue = inputSignal[i];
        }
    }

    for (int i = 0; i < totalSampleCount; i++) {
        inputSignal[i] = -5000 + (((inputSignal[i] - minValue) / (maxValue - minValue)) * 10000);
    }
}

// Function to select steady state frames
void IdentifySteadyStateFrames() {
    double highestValue = inputSignal[0];
    int maxIdx = 0;

    for (int i = 1; i < totalSampleCount; i++) {
        if (inputSignal[i] > highestValue) {
            highestValue = inputSignal[i];
            maxIdx = i;
        }
    }

    for (int i = 0; i < NUM_TOTAL_FRAMES; i++) {
        for (int j = 0; j < FRAME_SIZE; j++) {
            processedSignal[i][j] = inputSignal[maxIdx + j + FRAME_SIZE * (i - 2)];
        }
    }
}

// Function to compute LPC coefficients using Levinson-Durbin recursion
void ComputeLPC_Coeffs(int frameIndex) {
    double error[LPC_ORDER + 1] = {0.0};
    double reflection[LPC_ORDER + 1] = {0.0};
    double predictError[LPC_ORDER + 1][LPC_ORDER + 1] = {0.0};

    error[0] = autoCorrelation[frameIndex][0];

    for (int i = 1; i <= LPC_ORDER; i++) {
        double sum = 0.0;

        for (int j = 1; j < i; j++) {
            sum += predictError[j][i - 1] * autoCorrelation[frameIndex][i - j];
        }
        reflection[i] = (autoCorrelation[frameIndex][i] - sum) / error[i - 1];

        predictError[i][i] = reflection[i];
        for (int j = 1; j < i; j++) {
            predictError[j][i] = predictError[j][i - 1] - reflection[i] * predictError[i - j][i - 1];
        }

        error[i] = (1 - reflection[i] * reflection[i]) * error[i - 1];
    }

    for (int i = 1; i <= LPC_ORDER; i++) {
        LPC_Coefficients[frameIndex][i] = predictError[i][LPC_ORDER];
    }
}

// Function to compute Cepstral Coefficients from LPC coefficients
void ComputeCepstralCoeffs(int frameIndex) {
    cepstralCoefficients[frameIndex][0] = 0.0;
    weightedCepstralCoeffs[frameIndex][0] = 0.0;

    for (int n = 1; n <= LPC_ORDER; n++) {
        cepstralCoefficients[frameIndex][n] = LPC_Coefficients[frameIndex][n];
        double sum = 0.0;
        for (int k = 1; k < n; k++) {
            if (n - k >= 0) {
                sum += k * LPC_Coefficients[frameIndex][n - k] * cepstralCoefficients[frameIndex][k];
            }
        }
        cepstralCoefficients[frameIndex][n] += sum / n;
        //Raised Sine Window
        weightedCepstralCoeffs[frameIndex][n] += cepstralCoefficients[frameIndex][n] * (1 + (LPC_ORDER / 2) * sin(CONST_PI * n / LPC_ORDER));
    }
}

// Function to read signal data from file
void LoadSignalFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    std::string buffer;
    for (int i = 0; i < 10; i++) {
        file >> buffer;
    }

    while (file >> inputSignal[totalSampleCount] && totalSampleCount < 20000) {
        totalSampleCount++;
    }

    file.close();
}

// Function to write weighted cepstral coefficients to file
void SaveCepstralToFile(const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_TOTAL_FRAMES; i++) {
        for (int j = 1; j <= LPC_ORDER; j++) {
            weightedCepstralCoeffs[i][j] /= 20;
            file << weightedCepstralCoeffs[i][j] << " ";
            weightedCepstralCoeffs[i][j] = 0;
        }
        file << std::endl;  // New line after each row
    }

    file.close();
}

// Function to read a matrix from a file
void LoadMatrixFromFile(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_TOTAL_FRAMES; i++) {
        for (int j = 1; j <= LPC_ORDER; j++) {
            if (!(file >> savedCepstralCoeffs[i][j])) {
                std::cerr << "Error reading data at [" << i << "][" << j << "]\n";
                file.close();
                exit(EXIT_FAILURE);
            }
        }
    }

    file.close();
}

// Function to calculate Tokhura distance
double CalcTokhuraDistance(double vec1[LPC_ORDER + 1], double vec2[LPC_ORDER + 1]) {
    double distance = 0.0;
    for (int i = 1; i <= LPC_ORDER; i++) {
        double diff = vec1[i] - vec2[i];
        distance += tokhuraWeightFactors[i - 1] * diff * diff;
    }
    return distance;
}

// Function to compute average Tokhura distance
double AvgTokhuraDistance() {
    double totalDist = 0.0;

    for (int i = 0; i < NUM_TOTAL_FRAMES; i++) {
        double dist = CalcTokhuraDistance(savedCepstralCoeffs[i], weightedCepstralCoeffs[i]);
        totalDist += dist;
    }

    return totalDist / NUM_TOTAL_FRAMES;
}

// Function to compare two files using Tokhura distance
void CompareFilesWithTokhura(const char *testFilename, const char *referenceFilename) {
    LoadMatrixFromFile(referenceFilename);
    LoadMatrixFromFile(testFilename);
    double tokhuraDistance = AvgTokhuraDistance();
    std::cout << "Average Tokhura Distance: " << tokhuraDistance << std::endl;
}

// Function to find index of minimum distance
int MinDistanceIndex() {
    int minIdx = 0;
    for (int i = 1; i < NUM_TOTAL_FRAMES; i++) {
        if (frameDistances[i] < frameDistances[minIdx]) {
            minIdx = i;
        }
    }
    return minIdx;
}

// Function to reset weighted cepstral coefficients
void ResetCepstralCoeffs() {
    for (int i = 0; i < NUM_TOTAL_FRAMES; i++) {
        for (int j = 1; j <= LPC_ORDER; j++) {
            weightedCepstralCoeffs[i][j] = 0;
        }
    }
}

int main() {

    //Training Phase
    char inputFilePath[100] = "";
    char outputFilePath[100] = "";

    char vowelSet[5] = {'a', 'e', 'i', 'o', 'u'};
    for (int v = 0; v < 5; v++) {
        for (int i = 1; i < 21; i++) {
            totalSampleCount = 0;

            // Prepare file names
            sprintf(inputFilePath, "train_data/210101041_%c_%d.txt", vowelSet[v], i);
            // cout<<"hello"<<endl;

            // Read the signal data from the file
            LoadSignalFromFile(inputFilePath);

            // Correct DC offset
            AdjustDCOffset();

            // Normalize signal
            ScaleSignal();

            // Select steady state frames
            IdentifySteadyStateFrames();

            // Apply Hamming window
            for (int j = 0; j < NUM_TOTAL_FRAMES; j++) {
                // Reduce Spectral Leakage
                ApplyHamming(j);
            }

            // Calculate autocorrelation and LPC coefficients for each frame
            for (int j = 0; j < NUM_TOTAL_FRAMES; j++) {
                ComputeAutoCorr(j);// i*i+k
                ComputeLPC_Coeffs(j);
                ComputeCepstralCoeffs(j);
            }
        }

        sprintf(outputFilePath, "ci_values/210101041_%c.txt", vowelSet[v]);
        SaveCepstralToFile(outputFilePath);
    }

    cout<<"Training Done"<<endl;

    // Testing
    for (int v = 0; v < 5; v++) {
        for (int i = 21; i < 31; i++) {
            sprintf(inputFilePath, "test_data/210101041_%c_%d.txt", vowelSet[v], i);
            totalSampleCount = 0;

            // Read the signal data from the file
            LoadSignalFromFile(inputFilePath);

            // Correct DC offset
            AdjustDCOffset();

            // Normalize signal
            ScaleSignal();

            // Select steady state frames
            IdentifySteadyStateFrames();

            // Apply Hamming window
            for (int j = 0; j < NUM_TOTAL_FRAMES; j++) {
                ApplyHamming(j);
            }

            // Calculate autocorrelation and LPC coefficients for each frame
            for (int j = 0; j < NUM_TOTAL_FRAMES; j++) {
                ComputeAutoCorr(j);
                ComputeLPC_Coeffs(j);
                ComputeCepstralCoeffs(j);
            }

            for (int k = 0; k < 5; k++) {
                sprintf(outputFilePath, "ci_values/210101041_%c.txt", vowelSet[k]);
                LoadMatrixFromFile(outputFilePath);

                frameDistances[k] = AvgTokhuraDistance();
            }

            int bestMatchIndex = MinDistanceIndex();
            printf("Test file predicted vowel is %c.\n", vowelSet[bestMatchIndex]);
            ResetCepstralCoeffs();
        }
        printf("\n");
    }
	return 0;
}

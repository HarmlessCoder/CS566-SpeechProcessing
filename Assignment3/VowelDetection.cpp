#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iomanip> // For std::fixed and std::setprecision

using namespace std;

const int SAMPLE_COUNT = 320;
const int FRAME_SIZE = 64; 
const int STEADY_FRAMES = 5;
const int MAX_AMPLITUDE = 5000;
const double THRESHOLD = 0.25*5000*5000; // Placeholder for energy threshold

// Function to calculate the DC shift
double calculateDCShift(const vector<double>& samples) {
    if (samples.empty()) {
        throw runtime_error("Error: Empty sample vector in calculateDCShift.");
    }
    
    double sum = 0.0;
    for(int i=0;i<400;i++)
    {
        sum+=samples[i];
    }
    // for (double sample : samples) {
    //     sum += sample;
    // }
    return sum / 400;
}

// Function to normalize the samples
void normalizeSamples(vector<double>& samples) {
    if (samples.empty()) {
        throw runtime_error("Error: Empty sample vector in normalizeSamples.");
    }

    double dcShift = calculateDCShift(samples);
    
    double maxSample = *max_element(samples.begin(), samples.end());
    if (maxSample == 0) {
        throw runtime_error("Error: Maximum sample value is zero in normalizeSamples.");
    }
    

    for(int i=0;i<samples.size();i++)
    {
        samples[i] = ((samples[i] - dcShift) * 5000.0) / maxSample;
    }

}

// Function to calculate energy of a frame
double calculateEnergy(const vector<double>& frame) {
    if (frame.empty()) {
        throw runtime_error("Error: Empty frame vector in calculateEnergy.");
    }
    
    double energy = 0.0;
    for (double sample : frame) {
        energy += sample * sample;
    }
    return energy / frame.size();
}


// Function to extract steady frames based on energy threshold
vector<double> extractSteadyFrames(const vector<double>& samples, double threshold) {
    if (samples.empty()) {
        throw runtime_error("Error: Empty sample vector in extractSteadyFrames.");
    }
    
    vector<double> steadyFrames;
    int totalFrames = samples.size() / FRAME_SIZE;
    cout<<"Total Frames:"<<totalFrames<<endl;
    for (int i = 0; i < totalFrames; ++i) {
        vector<double> frame(samples.begin() + i * FRAME_SIZE, samples.begin() + (i + 1) * FRAME_SIZE);
        double energy = calculateEnergy(frame);

        cout<<i+1<<" Frame No: "<<energy<<" "<<threshold<<endl;
        cout<<"Energy of Frame: "<<energy/(5000.0*5000.0)<<endl;
        
        if (energy > threshold && steadyFrames.size() < SAMPLE_COUNT) {
            // steadyFrames.push_back(frame);
            cout<<"Frame No.: "<<i+1<<endl;
            for(int j=0;j<FRAME_SIZE;j++)
            {
                steadyFrames.push_back(frame[j]);
            }
            if (steadyFrames.size() >= SAMPLE_COUNT) break; // Stop after collecting enough steady frames
        }
    }
    
    if (steadyFrames.size() < SAMPLE_COUNT) {
        throw runtime_error("Error: Not enough steady frames found in extractSteadyFrames.");
    }
    
    return steadyFrames;
}

// Function to compute Ri values (Auto-correlation)
vector<double> computeRi(const vector<double>& frame, int p) {
    if (frame.size() < p) {
        throw runtime_error("Error: Frame size is smaller than LPC order in computeRi.");
    }
    
    vector<double> Ri(p + 1, 0.0);
    
    for (int i = 0; i <= p; ++i) {
        for (int j = 0; j < frame.size() - i; ++j) {
            Ri[i] += (double)frame[j] * frame[j + i];
        }
    }
    
    return Ri;
}

// Function to compute Ai values (LPC coefficients) using Levinson-Durbin recursion
vector<double> computeAi(const vector<double>& Ri, int p) {
    if (Ri.size() < p + 1) {
        throw runtime_error("Error: Ri vector size is smaller than LPC order in computeAi.");
    }

    vector<vector<double>> alpha(p + 1, vector<double>(p + 1, 0.0)); // 2D vector to store alpha coefficients
    vector<double> Ai(p + 1, 0.0);
    vector<double> E(p + 1, 0.0);
    vector<double> K(p + 1, 0.0);
    
    E[0] = Ri[0];
    
    for (int i = 1; i <= p; ++i) {
        double sum = 0.0;
        for (int j = 1; j < i; ++j) {
            sum += alpha[i-1][j] * Ri[i - j];
        }
        
        K[i] = (Ri[i] - sum) / E[i - 1];
        alpha[i][i] = K[i];
        
        for (int j = 1; j < i; ++j) {
            alpha[i][j] = alpha[i - 1][j] - K[i] * alpha[i - 1][i - j];
        }
        
        E[i] = (1 - K[i] * K[i]) * E[i - 1];
    }
    
    for (int j = 1; j <= p; ++j) {
        Ai[j] = alpha[p][j];
    }
    
    return Ai;
}



void processSteadyFrames(const vector<double>& frame, ofstream& outputFile, int p) {
    vector<double> Ri = computeRi(frame, p);
    vector<double> Ai = computeAi(Ri, p);

    // Set precision for output
    outputFile << fixed << setprecision(6);
    
    // Write Ri values to the output file
    // outputFile << "Frame " << frameIndex + 1 << " Ri values:\n";
    outputFile <<"Ri values: ";
    for (double r : Ri) {
        outputFile << r << " ";
    }
    outputFile << "\n\n";

    outputFile << "Ai values: ";
    for (int i = 1; i < Ai.size(); ++i) {  // Start from index 1 to skip the first value
        outputFile << Ai[i] << " ";
    }
    outputFile << "\n";
}


void processVowelUtterance(const string& inputFileName, const string& outputFileName, int p) {
    // vector<double> samples(SAMPLE_COUNT);
    
    vector<double> samples;

    // Load samples from file
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Could not open file '" << inputFileName << "'." << endl;
        return;
    }

    double value;
    while (inputFile >> value) {
        samples.push_back(value);
    }

    if (inputFile.bad()) {
        cerr << "Error: Failed to read the file '" << inputFileName << "'." << endl;
        return;
    }

    inputFile.close();
    
    try {
        
        // Normalize the samples
        normalizeSamples(samples);
        
        // Extract steady frames
        vector<double> steadyFrames = extractSteadyFrames(samples, THRESHOLD);
        
        // Open the output file
        ofstream outputFile(outputFileName);
        if (!outputFile) {
            cerr << "Error: Could not open output file '" << outputFileName << "' for writing." << endl;
            return;
        }

        processSteadyFrames(steadyFrames, outputFile, p);
        
        outputFile.close();
        cout << "Processing complete for " << inputFileName << "." << endl;
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return;
    }
}

// Updated main function
int main() {
    vector<string> vowels = {"a", "e", "i", "o", "u"};
    int utteranceCount = 20;  // Number of utterances per vowel
    int p = 12;  // Order of LPC (Linear Predictive Coding)

    for (const string& vowel : vowels) {
        for (int utterance = 1; utterance <= utteranceCount; ++utterance) {
            // Construct input and output filenames
            string inputFileName = "vowel_samples/210101041_" + vowel + "_" + to_string(utterance) + ".txt";
            string outputFileName = "output_files/210101041_" + vowel + "_" + to_string(utterance) + "_ai.txt";
            
            // Process the vowel utterance
            processVowelUtterance(inputFileName, outputFileName, p);
        }
    }

    // Validation for the given sample
    // vector<int>samples;
    vector<double> samples(SAMPLE_COUNT);
    string inputFileName = "A3_files/test.txt";
    // Load samples from file
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Could not open file '" << inputFileName << "'." << endl;
        return 1;
    }
    
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        if (!(inputFile >> samples[i])) {
            cerr << "Error: Failed to read sample at index " << i << " from file '" << inputFileName << "'." << endl;
            inputFile.close();
            return 1;
        }
    }
    inputFile.close();
    string outputFileName = "output_files/210101041_test__ai_ri.txt";
    // Open the output file
    ofstream outputFile(outputFileName);
    if (!outputFile) {
        cerr << "Error: Could not open output file '" << outputFileName << "' for writing." << endl;
        return 1;
    }
    processSteadyFrames(samples,outputFile, p);

    return 0;
}



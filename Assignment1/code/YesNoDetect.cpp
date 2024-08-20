#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

// Constants
const int SAMPLE_RATE = 16000;
const int FRAME_SIZE = 400; // 25 ms for 16 kHz sampling rate
const int FRAME_SHIFT = 160; // 10 ms for 16 kHz sampling rate
const string AMBIENT_NOISE_FILE = "ambient_noise.txt";

// Function to calculate energy of a frame
double calculateEnergy(const vector<int>& frame) {
    double energy = 0.0;
    for (size_t i = 0; i < frame.size(); ++i) {
        energy += frame[i] * frame[i];
    }
    return energy / frame.size();
}

// Function to calculate Zero Crossing Rate (ZCR)
double calculateZCR(const vector<int>& frame) {
    int zeroCrossings = 0;
    for (size_t i = 1; i < frame.size(); i++) {
        if ((frame[i - 1] >= 0 && frame[i] < 0) || (frame[i - 1] < 0 && frame[i] >= 0)) {
            zeroCrossings++;
        }
    }
    return static_cast<double>(zeroCrossings) / frame.size();
}

// Function to read amplitude data from file
vector<int> readAmplitudeData(const string& filename) {
    ifstream file(filename.c_str());
    vector<int> data;

    // Check if the file opened successfully
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return data; // Return an empty vector if the file couldn't be opened
    }

    string line;
    // Skip the first five lines
    for (int i = 0; i < 5 && getline(file, line); ++i) {
        // Do nothing, just skip the line
    }

    int value;
    // Read the remaining lines and store the amplitude values
    while (file >> value) {
        data.push_back(value);
    }

    return data;
}

// Function to segment the word based on noise floor
pair<int, int> segmentWord(const vector<int>& data, double noiseFloor) {
    int start = 0, end = data.size() - 1;
    int threshold = noiseFloor + 200;
    // Find word start
    for (size_t i = 0; i < data.size(); i++) {
        if (abs(data[i]) > threshold) {
            start = i;
            break;
        }
    }
    // Find word end
    for (size_t i = data.size() - 1; i >= 0; i--) {
        if (abs(data[i]) > threshold) {
            end = i;
            break;
        }
    }

    return make_pair(start, end);
}

// Function to compute ambient noise (noise floor)
double computeNoiseFloor(const vector<int>& data) {
    double sumSquares = 0.0;
    size_t count = 0;

    //for (size_t i = 0; i < data.size(); ++i) {
    //    if (abs(data[i]) < 100) { // Assuming 100 as a threshold for silence
    //        sumSquares += data[i] * data[i];
    //        count++;
    //    }
    //}

    // Compute sum of squares of samples for the first 5000 amplitudes
    for (int i = 0;i < 5000 && i < data.size();i++) {
        // cout<<sample[i]<<endl;
        sumSquares += data[i] * data[i];
        count++;
    }

    // Return RMS value if there are valid samples, else return 0
    return count > 0 ? sqrt(sumSquares / count) : 0.0;
}

// Function to calculate DC shift value from ambient noise data
long double calculateDCShift() {
    ifstream inputFile(AMBIENT_NOISE_FILE);

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file " << AMBIENT_NOISE_FILE << endl;
        return -1;
    }

    int value;
    long double sum = 0;
    int count = 0;

    // Calculate the mean (DC shift) of the noise samples
    while (inputFile >> value) {
        sum += value;
        count++;
    }

    inputFile.close();
    return sum / count;
}

// Function to calculate normalization value (maximum amplitude) from the word file
int calculateNormalizationValue(const string& filename) {
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return -1;
    }

    int value;
    int maxAmplitude = INT_MIN;

    // Find the maximum absolute amplitude value for normalization
    while (inputFile >> value) {
        maxAmplitude = max(maxAmplitude, abs(value));
    }

    inputFile.close();
    return maxAmplitude;
}

// Function to classify word as "Yes" or "No"
string classifyWord(double energy, double zcr) {
    // Simplistic classification based on ZCR
    if (zcr >= 0.17) {
        return "Yes";
    }
    else {
        return "No";
    }
}

int main() {
    string fileName;
    cout << "Enter the file name: ";
    cin >> fileName;
    cout << endl;
    // Read the speech data from file
    vector<int> data = readAmplitudeData(fileName);

    // Compute the noise floor
    double noiseFloor = computeNoiseFloor(data);

    cout << "Noise Floor: " << noiseFloor << endl;

    // Segment the word based on noise floor
    pair<int, int> segment = segmentWord(data, noiseFloor);
    int start = segment.first;
    int end = segment.second;

    //cout << "Start of the word: " << start << endl;
    //cout << "End of the word: " << end << endl;

    // Process the segmented word
    vector<int> wordSegment(data.begin() + start, data.begin() + end + 1);

    // Divide the word segment into frames and calculate features
    double totalEnergy = 0.0;
    double totalZCR = 0.0;
    int frameCount = 0;
    for (size_t i = 0; i + FRAME_SIZE < wordSegment.size(); i += FRAME_SHIFT) {
        vector<int> frame(wordSegment.begin() + i, wordSegment.begin() + i + FRAME_SIZE);
        totalEnergy += calculateEnergy(frame);
        totalZCR += calculateZCR(frame);
        frameCount++;
    }

    double avgEnergy = totalEnergy / frameCount;
    double avgZCR = totalZCR / frameCount;

    cout << "Average Energy: " << avgEnergy << endl;
    cout << "Average ZCR: " << avgZCR << endl;

    // Classify the word as "Yes" or "No"
    string decision = classifyWord(avgEnergy, avgZCR);

    // Echo the decision
    cout << endl;
    cout << "The word is: " << decision << endl;
    cout << endl;

    return 0;
}

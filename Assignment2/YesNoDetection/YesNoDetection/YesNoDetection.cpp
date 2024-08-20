#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <Windows.h>
#include <algorithm>  // For std::max

using namespace std;

// Constants
const int SAMPLE_RATE = 16000;
const int FRAME_SIZE = 400; // 25 ms for 16 kHz sampling rate
const int FRAME_SHIFT = 160; // 10 ms for 16 kHz sampling rate
const int NUMPTS = 16025 * 3;   // 3 seconds
short int waveIn[NUMPTS];
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

// Function to segment the word based on noise floor
pair<int, int> segmentWord(const vector<int>& data, double noiseFloor) {
    int n = data.size();
    int start = 0, end = n - 1;
    int threshold = static_cast<int>(noiseFloor) + 200;

    // Find word start
    for (int i = 0; i < n; i++) {
        if (abs(data[i]) > threshold) {
            start = i;
            break;
        }
    }

    // Find word end
    for (int i = n-1; i >= 0; i--) {
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

    // Compute sum of squares of samples for the first 10000 amplitudes
    for (int i = 0; i < 10000 && i < data.size(); i++) {
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
    int maxAmplitude = 0;

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
    if (zcr >= 0.14) {
        return "Yes";
    }
    else {
        return "No";
    }
}

void StartRecord() {
    int sampleRate = 16025;
    HWAVEIN hWaveIn;
    MMRESULT result;
    WAVEFORMATEX pFormat;
    pFormat.wFormatTag = WAVE_FORMAT_PCM;  // simple, uncompressed format
    pFormat.nChannels = 1;                 //  1=mono, 2=stereo
    pFormat.nSamplesPerSec = sampleRate;   // 8.0 kHz, 11.025 kHz, 22.05 kHz, and 44.1 kHz
    pFormat.nAvgBytesPerSec = sampleRate * 2; // =  nSamplesPerSec × nBlockAlign
    pFormat.nBlockAlign = 2;               // = (nChannels × wBitsPerSample) / 8
    pFormat.wBitsPerSample = 16;           //  16 for high quality, 8 for telephone-grade
    pFormat.cbSize = 0;

    // Specify recording parameters
    result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);
    WAVEHDR WaveInHdr;

    // Set up and prepare header for input
    WaveInHdr.lpData = (LPSTR)waveIn;
    WaveInHdr.dwBufferLength = NUMPTS * 2;
    WaveInHdr.dwBytesRecorded = 0;
    WaveInHdr.dwUser = 0L;
    WaveInHdr.dwFlags = 0L;
    WaveInHdr.dwLoops = 0L;
    waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

    // Insert a wave input buffer
    result = waveInAddBuffer(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

    // Commence sampling input
    result = waveInStart(hWaveIn);
    cout << "Recording for 3 seconds(first 0.5 sec for noise calculation)..." << endl;
    Sleep(3 * 1000);

    // Stop recording and close the device
    waveInClose(hWaveIn);
}

int main() {
    // Record audio from the microphone
    StartRecord();

    // Convert recorded data to vector<int>
    vector<int> data(waveIn, waveIn + NUMPTS);

    //cout << data.size() << endl;

    // Compute the noise floor
    double noiseFloor = computeNoiseFloor(data);
    cout << endl;
    cout << "Noise Floor: " << noiseFloor << endl;

    // Segment the word based on noise floor
    pair<int, int> segment = segmentWord(data, noiseFloor);
    int start = segment.first;
    int end = segment.second;

    if (start == 0) {
        cout << "You have not said any word...Exiting" << endl;
        return 0;
    }

    double startTime = start / 16000.0;
    double endTime = end / 16000.0;

    cout << "Start Time of the Word(in seconds): " << startTime << endl;
    cout << "End Time of the Word(in seconds): " << endTime << endl;
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

# A4: Vowel Recognition System

## Overview

This project implements a vowel recognition system using Linear Predictive Coding (LPC) and Cepstral analysis. The program processes vowel recordings, computes Ri, Ai, and Ci values, and uses Tokhura's distance to classify test vowel recordings based on a set of reference files.

## How to Run

1. Prepare your training and test data using the naming convention.
2. Compile the code:
   ```
   g++ VowelDetection.cpp -o VowelDetection
   ```
3. Run the program:

   ```
   ./VowelDetection
   ```

   Ensure your input files are placed in the correct directories (`train_data/` and `test_data/`).

4. The program will first generate the reference files and then proceed to recognize the test vowels.

### Key Steps in the Process:

1. Preprocessing (DC Shift and Normalization)
2. Steady-State Frame Selection
3. LPC Analysis (Autocorrelation, LPC Coefficients)
4. Cepstral Coefficients Calculation
5. Tokhura's Distance Calculation for Vowel Classification

## Prerequisites

The following input files are required:

1. A `test.txt` file containing 320 sample points for vowel sounds.
2. Files for validation:

   - `dump_Ai_test.txt`: Ai values.
   - `dump_Ci_test.txt`: Ci values with and without Hamming window.
   - `dump_Ri_test.txt`: Ri values.

   These files should be compared against the generated outputs to ensure correctness.

## Files & Directory Structure

- `train_data/`: Contains the training data files for different vowels.
- `test_data/`: Contains the test data files for vowel recognition.
- `ci_values/`: Stores the averaged Cepstral values used as reference for each vowel.

### File Naming Convention

Each vowel recording is named using the following format:

```
<RollNo>_<vowel>_<utteranceNo>.txt
```

For example: `210101041_a_1.txt` for the first recording of vowel 'a'. This is essential for organizing training and test data.

## Procedure

### Generating Reference Files for Vowels

1. **DC Shift and Normalization**: Each vowel recording is processed to remove any DC bias (DC shift) and normalize the amplitude of the signal.
2. **Frame Selection**: Select 5 frames from the steady part of the vowel signal. The steady frames correspond to the most stable region of the vowel sound.
3. **Autocorrelation Calculation (Ri)**: The Ri values are computed for each frame to capture the temporal structure of the signal.
4. **LPC Coefficients Calculation (Ai)**: Using Levinson-Durbin recursion, LPC coefficients (Ai) are computed based on the autocorrelation values. These coefficients model the vocal tract during vowel production.
5. **Cepstral Coefficients Calculation (Ci)**: Compute the LPC-based Cepstral coefficients and apply a raised sine window to smooth the values.
6. **Averaging Cepstral Coefficients**: For each vowel, 20 recordings are processed, yielding 100 sets of Ci values (5 frames per recording × 20 recordings). The values from corresponding frames are averaged, resulting in 5 rows of Cepstral coefficients per vowel.
7. **Saving the Reference Files**: The averaged Ci values for each vowel are stored in a text file in the `ci_values/` directory.

### Testing & Vowel Recognition

1. **Preprocessing**: Similar to training, each test file undergoes DC shift, normalization, and steady-state frame selection.
2. **Computing Ri, Ai, and Ci**: The autocorrelation, LPC, and Cepstral coefficients are computed for the test frames.
3. **Tokhura's Distance Calculation**: Tokhura's distance is calculated between the Ci values of the test file and each reference file. The formula for Tokhura's distance is:
   ```
   D = Σ [ Wi × (Ci_test - Ci_ref)^2 ]
   ```
   where Wi are the Tokhura weight factors, and `Ci_test` and `Ci_ref` are the Cepstral coefficients from the test and reference files, respectively.
4. **Vowel Classification**: The reference vowel with the smallest average Tokhura distance is selected as the recognized vowel. This process is repeated for 10 test files per vowel.

## Detailed Function Description

### 1. **Preprocessing**

- **AdjustDCOffset**: Corrects the DC shift by subtracting the mean value from the signal.
- **ScaleSignal**: Normalizes the signal to ensure it lies within a specified amplitude range.

### 2. **Frame Selection**

- **IdentifySteadyStateFrames**: Identifies the most stable frames of the vowel sound by selecting the highest amplitude region.

### 3. **LPC and Cepstral Analysis**

- **ComputeAutoCorr**: Computes the autocorrelation values for each frame.
- **ComputeLPC_Coeffs**: Uses Levinson-Durbin recursion to calculate the LPC coefficients.
- **ComputeCepstralCoeffs**: Converts LPC coefficients into Cepstral coefficients and applies a raised sine window.

### 4. **Tokhura's Distance Calculation**

- **CalcTokhuraDistance**: Computes the Tokhura distance between two sets of Cepstral coefficients.
- **AvgTokhuraDistance**: Averages the Tokhura distance over multiple frames to give a final distance for classification.

### 5. **File Handling**

- **LoadSignalFromFile**: Loads the input signal from a file.
- **SaveCepstralToFile**: Saves the computed Cepstral coefficients to a file.
- **LoadMatrixFromFile**: Loads saved Cepstral coefficients from reference files for Tokhura distance comparison.

## Tokhura's Weights

The Tokhura weights used in the distance calculation are:

```
[1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0]
```

## Output

### Training Phase

The reference files containing averaged Cepstral coefficients for each vowel will be stored in the `ci_values/` directory. Each file contains 5 rows of 12 Cepstral coefficients.

### Testing Phase

For each test file, the program outputs the recognized vowel based on the smallest Tokhura distance.

### Example Output:

```
Test file predicted vowel is a.
Test file predicted vowel is e.
...
```

## Notes

- Ensure that the input files strictly follow the format and naming convention mentioned.
- Modify the constants like `FRAME_SIZE`, `NUM_TOTAL_FRAMES`, etc., if necessary to fit the data you are working with.
- The raised sine window applied to the Cepstral coefficients helps in weighting the higher order coefficients.

## Conclusion

This project provides a vowel recognition system that classifies vowel sounds based on LPC analysis and Cepstral coefficients. By computing the Tokhura distance, it identifies the vowel with the smallest distance as the correct classification. This method is robust and effective for vowel sound recognition tasks.

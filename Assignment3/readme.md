# **A3: Calculate Ai's from a Speech Signal for Vowel Analysis**

## **Overview**

This project focuses on analyzing the five primary vowel sounds (`a`, `e`, `i`, `o`, `u`) using Linear Predictive Coding (LPC) methods. The goal is to process audio samples, perform DC shift and normalization, extract steady frames, and compute LPC coefficients (Ai) and auto-correlation values (Ri) for those frames. The project steps involve capturing vowel samples, preprocessing the audio data, and applying LPC analysis.

## How to Run the Code

1. **Open the Terminal:**

   - Open terminal and move to the code directory which contains code and input files.

2. **Compile C++ file**

   - Compile the VowelDetection.cpp file using the below command
   - g++ VowelDetection.cpp

3. **Run the output file**

   - In terminal run this command: ./a.out (for linux) and ./a.exe (for windows)

4. The program processes the vowel sample files located in the `vowel_samples/` directory and writes the results to the `output_files/` directory. The program will also process a test file (`A3_files/test.txt`) for validation purposes.

---

### **Project Steps**

1. **Record 20 Samples of Each Vowel**: Record vowel sounds and save them in text files where each sample corresponds to an utterance of the vowel sound.
2. **Manual Trimming**: Using an audio editing tool like CoolEdit, trim the audio to isolate the vowel sound. Keep approximately 400 samples of silence before and after the vowel.
3. **Preprocessing**: The first vowel recording undergoes preprocessing:
   - **DC Shift**: Calculate and remove the DC bias from the samples.
   - **Normalization**: Scale the samples to a consistent amplitude.
4. **Steady Frame Selection**: From each vowel recording, select 5 steady frames where the energy is consistently high. This indicates the stable part of the vowel sound.
5. **Compute LPC Coefficients (Ai) and Auto-correlation (Ri)**: For each steady frame, compute the LPC coefficients and auto-correlation values. Store these values in output files following the correct naming convention.
6. **Repeat for All Vowel Recordings**: Perform the same processing steps for all 20 recordings of each vowel.

### **File Structure**

- **`vowel_samples/`**: Contains the vowel sample text files. Each file follows the naming convention `210101041_<vowel>_<utterance>.txt`.
- **`output_files/`**: Stores the output files containing LPC coefficients (Ai) and auto-correlation values (Ri) for each vowel sample. The output files are named as `210101041_<vowel>_<utterance>_ai.txt`.

### **Explanation of Code**

1. **Main Function**:

   - The main function iterates over all vowels and utterances.
   - For each vowel and utterance, it constructs the input and output filenames and then calls `processVowelUtterance()` to perform the required analysis.
   - The main function also contains a validation block for processing a single test file.

2. **`processVowelUtterance()`**:

   - This function handles the entire process of reading samples, preprocessing, selecting steady frames, and writing results.
   - It first loads the sample data from the specified file.
   - After normalization, it extracts the steady frames based on energy levels.
   - Finally, it calls `processSteadyFrames()` to calculate and write LPC coefficients and auto-correlation values to an output file.

3. **`normalizeSamples()`**:

   - This function normalizes the audio samples by removing the DC bias and scaling the amplitude to a consistent level.

4. **`calculateDCShift()`**:

   - Calculates the DC shift by averaging the initial 400 samples (assumed to be silence) and subtracting this from all samples.

5. **`extractSteadyFrames()`**:

   - This function selects steady frames based on energy calculations. It identifies frames where the energy exceeds a certain threshold, which indicates stable vowel sound portions.

6. **`processSteadyFrames()`**:

   - Computes the auto-correlation (Ri) and LPC coefficients (Ai) for each steady frame and writes them to an output file.

7. **LPC Calculation**:
   - **`computeRi()`** calculates auto-correlation values for the steady frame.
   - **`computeAi()`** uses the Levinson-Durbin recursion to compute LPC coefficients.

### **How to Add Your Own Data**

1. **Recording New Samples**: Record your vowel samples, ensuring you have 20 recordings for each vowel.
2. **Manual Trimming**: Use CoolEdit or another audio tool to trim the vowel sound, keeping a few milliseconds of silence before and after the sound.
3. **File Naming**: Save each sample as `210101041_<vowel>_<utterance>.txt`, where `<vowel>` is one of `a`, `e`, `i`, `o`, `u` and `<utterance>` is the recording number from 1 to 20.

### **Phonetic Pronunciation of Vowels**

- `/a/` - Bath - अ
- `/e/` - Am - ए
- `/i/` - See - ई
- `/o/` - Boat - ओ
- `/u/` - Boot - ऊ

### **Additional Notes**

- **Steady Frames**: The frames selected for LPC analysis are those where the energy remains relatively high and stable, indicating a consistent vowel sound.
- **Threshold**: The threshold for energy is manually set based on analyzing the first and last few silence frames. Adjust this threshold as necessary depending on your audio data.

### **Error Handling**

The code contains error checks for:

- Empty sample vectors.
- File read/write issues.
- Insufficient steady frames detected.

If any of these errors occur, the program will output an appropriate error message and continue with the next file.

### **Future Improvements**

- **Automated Threshold Selection**: Implement an automatic method to select the energy threshold for steady frame extraction.
- **Graphical Interface**: A GUI could be added to allow users to visualize the audio signal and select steady frames more easily.

### **Conclusion**

This project demonstrates how to perform basic LPC analysis on vowel sounds. By following the steps outlined here, you can analyze your own vowel recordings and extract LPC coefficients to study the characteristics of these sounds.

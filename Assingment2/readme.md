
---

# Yes/No Speech Detection using C++

**Author:** Gautam Juneja  
**Roll No.:** 210101041  
**Course:** B.Tech in Computer Science (Final Year)  

## Table of Contents

- [Project Overview](#project-overview)
- [How to Run the Code](#how-to-run-the-code)
- [Features](#features)
- [Requirements](#requirements)
- [Approach](#approach)
  - [Step 1: Reading the Speech Data](#step-1-reading-the-speech-data)
  - [Step 2: Computing the Noise Floor](#step-2-computing-the-noise-floor)
  - [Step 3: Segmenting the Word](#step-3-segmenting-the-word)
  - [Step 4: Processing the Segmented Word](#step-4-processing-the-segmented-word)
  - [Step 5: Frame-Based Feature Extraction](#step-5-frame-based-feature-extraction)
  - [Step 6: Word Classification](#step-6-word-classification)
- [Output](#output)
- [Conclusion](#conclusion)

## Project Overview

This project involves the classification of a recorded speech segment as either "Yes" or "No" using C++. The code processes a speech signal stored in a `.txt` file, computes relevant features like energy and Zero Crossing Rate (ZCR), and then classifies the word based on these features.

---

## How to Run the Code

1. **Open the Terminal:**

   - Open terminal and move to the code directory which contains code and input files.

2. **Compile C++ file**

   - Compile the YesNoDetect file using the below command
   - g++ YesNoDetect.cpp

3. **Run the output file**

   - In terminal run this command: ./a.out (for linux) and ./a.exe (for windows)

4. **Provide the Input File:**

   - In the terminal window that appears, you will be prompted with the message `Enter file name:`.
   - Enter the name of the text file containing the speech data you want to process. You can use one of the sample files such as `yes1.txt`, `yes2.txt`, `yes3.txt`, `no1.txt`, `no2.txt`, or `no3.txt`.

5. **View the Output:**
   - After entering the file name, the program will process the input file and display the results, including the noise floor, word segment start and end, average energy, average ZCR, and the final classification ("Yes" or "No").

---

## Features

- **Noise Floor Calculation:** Utilizes the RMS (Root Mean Square) method to estimate ambient noise.
- **Word Segmentation:** Identifies the start and end of the word based on the calculated noise floor.
- **Feature Extraction:** Computes Energy and Zero Crossing Rate (ZCR) for each frame of the word.
- **Calculate DCShift:** Calculates the DC shift value, which is the mean of the amplitude values from the ambient noise data file.
- **Calculate Normalization Value:** Classifies the word as either "Yes" or "No" based on the extracted features.
- **Word Classification:** Classifies the word as either "Yes" or "No" based on the extracted features.

## Requirements

- C++ compiler (e.g., g++)
- A text file containing the amplitude values of the speech signal (e.g., `speech.txt`)

## Approach

### Step 1: Reading the Speech Data

The program starts by reading amplitude data from the input file. The first five lines are ignored, allowing for any metadata or comments. The remaining data is stored in a vector for further processing.

### Step 2: Computing the Noise Floor

The noise floor represents the ambient noise level. This is calculated using the RMS (Root Mean Square) method. The program sums the squares of all samples that are considered silence (below a certain threshold) and then computes the square root of the average.

### Step 3: Segmenting the Word

With the noise floor calculated, the program then identifies the start and end positions of the word within the signal. These are the points where the signal amplitude exceeds the noise floor.

### Step 4: Processing the Segmented Word

The identified word segment is extracted from the original signal. This segment is then divided into frames of 25 ms duration with a 10 ms overlap, corresponding to typical speech analysis practices.

### Step 5: Frame-Based Feature Extraction

For each frame, the program calculates two features:

- **Energy:** Measures the power of the signal in the frame.
- **Zero Crossing Rate (ZCR):** Counts the number of times the signal changes sign, indicating the presence of higher frequencies.

### Step 6: Word Classification

The average energy and ZCR across all frames are used to classify the word. A simple heuristic is used where a high ZCR suggests the word is "Yes," and a lower ZCR suggests "No."

## Output

Upon successful execution, the program outputs:

- The calculated noise floor.
- The start and end indices of the word segment.
- The average energy and ZCR of the word segment.
- The final classification of the word as either "Yes" or "No."

## Conclusion

This project demonstrates a basic yet effective approach to classifying simple words based on speech signal analysis. By computing key features like energy and ZCR, the program can distinguish between "Yes" and "No" in a recorded speech signal. The project serves as a foundation for further exploration into more advanced speech processing and recognition techniques.

## Note

I have not uploaded the Visual Studio Project Folder because the zip file was exceeding 10MB after deleting all the unnecessary files.

---

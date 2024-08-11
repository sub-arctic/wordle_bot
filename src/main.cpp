#include "wordlist.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

enum class Color
{
    Gray = 0,   // Incorrect letter
    Yellow = 1, // Correct letter in wrong position
    Green = 2,  // Correct letter in the correct position
};

struct WordleGuess
{
    std::string word;
    std::vector<Color> colors;

    // Constructor to initialize the struct
    WordleGuess(const std::string &w, const std::vector<Color> &c)
        : word(w), colors(c)
    {
    }
    // Method to get gray letters
    std::vector<char> getGrayLetters() const
    {
        std::vector<char> grayLetters;
        for (size_t i = 0; i < colors.size(); ++i)
        {
            if (colors[i] == Color::Gray)
            {
                grayLetters.push_back(word[i]);
            }
        }
        return grayLetters;
    }

    // Method to get yellow letters with their indices
    std::vector<std::pair<char, size_t>> getYellowLetters() const
    {
        std::vector<std::pair<char, size_t>> yellowLetters;
        for (size_t i = 0; i < colors.size(); ++i)
        {
            if (colors[i] == Color::Yellow)
            {
                yellowLetters.emplace_back(word[i], i);
            }
        }
        return yellowLetters;
    }

    // Method to get green letters with their indices
    std::vector<std::pair<char, size_t>> getGreenLetters() const
    {
        std::vector<std::pair<char, size_t>> greenLetters;
        for (size_t i = 0; i < colors.size(); ++i)
        {
            if (colors[i] == Color::Green)
            {
                greenLetters.emplace_back(word[i], i);
            }
        }
        return greenLetters;
    }
};

int randomIntInRange(const unsigned int &seed, const int &min, const int &max)
{
    std::mt19937 eng(seed);
    std::uniform_int_distribution<> distr(min, max);
    return distr(eng);
}

bool checkWordInWordlist(const std::string &word,
                         const std::vector<std::string> &wordList)
{
    for (const std::string &string : wordList)
    {
        if (word == string)
        {
            return true;
        }
    }
    return false;
}

void clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string toLowercase(const std::string &input)
{
    std::string lowercased = input; // Create a copy of the input string
    std::transform(lowercased.begin(), lowercased.end(), lowercased.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowercased; // Return the lowercase string
}

std::vector<Color> readGuessResult(const std::string &word)
{
    std::vector<Color> results;

    int result = 0;
    std::cout << "0 = gray, 1 = yellow, 2 = green" << std::endl;
    for (const char &c : word)
    {
        while (true)
        {
            std::cout << "Enter the result for letter " << c << ": ";
            std::cin >> result;

            // Check if result was successful or not
            if (std::cin.fail())
            {
                std::cout << "Invalid input. Please enter a valid integer."
                          << std::endl;
                clearInputBuffer();
            }
            else
            {
                if (result != 0 && result != 1 && result != 2)
                {
                    std::cout << "Invalid input. Please enter either 0, 1 or 2"
                              << std::endl;
                }
                else
                {
                    break;
                }
            }
        }
        switch (result)
        {
        case 0:
            results.push_back(Color::Gray);
        case 1:
            results.push_back(Color::Yellow);
        case 2:
            results.push_back(Color::Green);
        }
    }
    return results;
}

WordleGuess readInWord(std::vector<WordleGuess> &guesses)
{
    std::string word;

    while (true)
    {
        std::cout << "Please enter a 5-letter guess: ";
        std::cin >> word;

        // Check if the length is 5 letters
        if (word.length() == 5)
        {
            word = toLowercase(word);
            bool pass = checkWordInWordlist(word, wordle::wordList);
            if (pass)
            {
                std::vector<Color> colors = readGuessResult(word);
                WordleGuess guess(word, colors);
                guesses.push_back(guess);
                return guess;
            }
            else
            {
                std::cout << "Word not found." << std::endl;
                clearInputBuffer();
            }
        }
        else
        {
            std::cout << "Invalid input. Please enter a 5 letter word."
                      << std::endl;

            clearInputBuffer();
        }
    }
}


// Oops we broke something in the prune function, fix later
void filterWords(std::vector<std::string>& wordList, const WordleGuess &guess)
{
    std::cout << "Initial word list size: " << wordList.size() << std::endl;

    // Get gray letters
    std::vector<char> grayLetters = guess.getGrayLetters();

    // Get yellow letters with indices
    std::vector<std::pair<char, size_t>> yellowLetters = guess.getYellowLetters();

    // Get green letters with indices
    std::vector<std::pair<char, size_t>> greenLetters = guess.getGreenLetters();
    
    // Filter out words with gray letters
    wordList.erase(
        std::remove_if(
            wordList.begin(), wordList.end(),
            [&grayLetters](const std::string &word)
            {
                // Check if any gray letter is in the word
                for (char gray : grayLetters)
                {
                    if (word.find(gray) != std::string::npos)
                    {
                        return true; // Found a gray letter in the word
                    }
                }
                return false; // No gray letters found in the word
            }),
        wordList.end());

    // Filter out words that have yellow letters at the specified indices
    wordList.erase(
        std::remove_if(
            wordList.begin(), wordList.end(),
            [&yellowLetters](const std::string &word)
            {
                // Check each yellow letter and its index
                for (const auto &pair : yellowLetters)
                {
                    char yellowLetter = pair.first;
                    size_t index = pair.second;

                    // Check if the index is valid for the current word
                    if (index < word.size() && word[index] == yellowLetter)
                    {
                        return true; // Found a yellow letter at the specified index
                    }
                    // Check if the yellow letter is not present in the word at all
                    if (word.find(yellowLetter) == std::string::npos)
                    {
                        return true; // Yellow letter must be present in the word
                    }
                }
                return false; // No yellow letters found at specified indices
            }),
        wordList.end());

    // Filter out words that do not have green letters at the specified indices
    wordList.erase(
        std::remove_if(
            wordList.begin(), wordList.end(),
            [&greenLetters](const std::string &word)
            {
                // Check if the word does not have the green letter at the specified index
                for (const auto &pair : greenLetters)
                {
                    char greenLetter = pair.first;
                    size_t index = pair.second;

                    // If the index is valid and the character does not match the green letter, return true to remove
                    if (index < word.size() && word[index] != greenLetter)
                    {
                        return true; // This word does not have the green letter at the specified index
                    }
                }
                return false; // All checks passed, keep the word
            }),
        wordList.end());

    std::cout << "Filtered word list size: " << wordList.size() << std::endl;
    if (!wordList.empty()) {
        std::cout << "First candidate: " << wordList[0] << std::endl;
    } else {
        std::cout << "No candidates found." << std::endl;
    }
}

// Simulate every possible answer given every possible guess to determine outcomes:
// This is extremely slow, and a terrible method. Execution takes quite literally forever,
// which could be mitigated with parralel_for loops, but we can do better.
void simulateOutcomes(const std::vector<std::string>& wordList) {
    std::vector<std::vector<std::pair<char, int>>> results;

    for (const std::string& word : wordList) {
        std::vector<std::pair<char, int>> result;

        for (const std::string& answer : wordList) {
            std::vector<int> answerCount(26, 0); // Count of letters in the answer
            std::vector<int> feedback(word.size(), 0); // Feedback array

            // First pass: count letters in the answer and mark greens
            for (size_t i = 0; i < answer.size(); ++i) {
                answerCount[answer[i] - 'a']++; // Count letters
                if (word[i] == answer[i]) {
                    feedback[i] = 2; // Green
                    answerCount[word[i] - 'a']--; // Decrease count for green letters
                }
            }

            // Second pass: determine yellows and grays
            for (size_t i = 0; i < word.size(); ++i) {
                if (feedback[i] == 0) { // Only check if not already green
                    if (answerCount[word[i] - 'a'] > 0) {
                        feedback[i] = 1; // Yellow
                        answerCount[word[i] - 'a']--; // Decrease count for yellow letters
                    } else {
                        feedback[i] = 0; // Gray
                    }
                }
            }

            // Build the result vector based on feedback
            for (size_t i = 0; i < word.size(); ++i) {
                result.push_back(std::make_pair(word[i], feedback[i]));
            }
        }
        results.push_back(result);
    }

    for (const auto& res : results) {
        for (const auto& pair : res) {
            std::cout << "(" << pair.first << ", " << pair.second << ") ";
        }
        std::cout << std::endl;
    }
}

// TODO: update return type to a better struct or similar
// Post mortem: this has the same problem as the previous result: efficiency
std::string calculateResult(const std::string& guess, const std::string& target)
{
    std::string feedback(guess.length(), 'N'); // initialize feedback with N (gray)
    std::vector<bool> targetUsed(target.length(), false);

    // Green
    for (size_t i = 0; i < guess.length(); ++i)
    {
	if (guess[i] == target[i])
	{
	    feedback[i] = 'G';
	    targetUsed[i] = true;
	}
    }

    for (size_t i = 0; i < guess.length(); ++i)
    {
	if (feedback[i] == 'N')
	{
	    for(size_t j = 0; j < target.length(); ++j)
	    {
		if (guess[i] == target[j] && !targetUsed[j])
		{
		    feedback[i] = 'Y';
		    targetUsed[j] = true;
		    break;
		}
	    }
	}
    }
    return feedback;
}

double calculateEntropy(const std::string& guess, const std::vector<std::string>& wordList)
{
    std::unordered_map<std::string, std::vector<std::string>> outcomes;

    for (const std::string& word : wordList)
    {
	std::string result = calculateResult(word, guess);
	outcomes[result].push_back(word);
    }

    double entropy = 0.0;
    double totalWords = wordList.size();

    for (const auto& outcome : outcomes)
    {
	double p = outcome.second.size() / totalWords;
	if (p > 0)
	{
	    entropy -= p * std::log2(p);
	}
    }
    return entropy;
}


// Outcomes given as 3^5 = 243
// BRIEF RUNDOWN OF ENTROPY
// The probability (p) of finding a word with a certain property can be calculated by dividing the toal number of words containing A (represented as Ma) by the number of all words (M).
// So p = Ma / M.
// At the same time, the information (I), meaning "The word contains an A," reduces the sapce of all possibilties (M) by the factor of (1/2)^I.
// It can be presented as Ma = (1/2)^i * M.
//
// VERDICT:
//
// p = (1/2)^I * M / M
// I = -log2p


int main(int argc, char *argv[])
{
    std::vector<WordleGuess> guesses;

    /* const std::string answer =
        wordle::wordList[randomIntInRange(std::stoi(argv[1]), 0,
    wordle::wordCount)]; std::cout << answer << std::endl; */

    /* for (const auto& word : wordle::wordList)
    {
	double entropy = calculateEntropy(word, wordle::wordList);
	if (entropy > maxEntropy) {
	    maxEntropy = entropy;
	    bestGuess = word;
	    std::cout << bestGuess << std::endl;
	}
	++i;
    } */


    std::vector<std::string> wordList = wordle::wordList;
    for (int i = 0; i < 5; ++i)
    {
        WordleGuess word = readInWord(guesses);
        filterWords(wordList, word);
	// simulateOutcomes(wordList);
    }
    return 0;
}

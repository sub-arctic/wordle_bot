#include "wordlist.h"
#include "feedback.h"
#include <future>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

enum class LetterColor
{
    Correct,   // Letter is correct and in the correct position
    Misplaced, // Letter is in the word but in the wrong position
    Incorrect  // Letter is not in the word
};

// Struct to represent each letter and its color
struct LetterResult
{
    char letter;       // The letter itself
    LetterColor color; // The color/status of the letter

    LetterResult(char l, LetterColor c) : letter(l), color(c) {}
};

// Struct to represent the entire guess
struct GuessResult
{
    std::string guess;                       // The guessed word
    std::vector<LetterResult> letterResults; // Results for each letter

    // Constructor to initialize the guess and letter results
    GuessResult(const std::string &g, const std::vector<LetterResult> &lr)
        : guess(g), letterResults(lr)
    {
    }

    // Method to return the guess string
    std::string getGuess() const { return guess; }
};

struct Feedback
{
    char data[6];
    int count;

    Feedback(const char* initialData)
    {
	std::strncpy(data, initialData, sizeof(data) - 1);
	data[sizeof(data) - 1] = '\0'; // ensure null termination
	count = 0;
    }

    void incrementCount()
    {
	count++;
    }
    bool matches(const char* str) const
    {
        return strncmp(data, str, sizeof(data)) == 0;
    }
    std::pair<const char*, int> getFeedback() const
    {
        return std::make_pair(data, count);
    }
};

void addOrIncrementFeedback(std::vector<Feedback>& feedbacks, const char* feedback)
{
    for (auto& feedbackEntry : feedbacks)
    {
        if (feedbackEntry.matches(feedback))
	{
            feedbackEntry.incrementCount();
            return; // Exit the function after incrementing
        }
    }
    // If not found, add a new entry
    feedbacks.emplace_back(feedback);
}

std::pair<std::string, std::vector<Feedback>> calculateFeedbackForWord(const std::string& word, const std::vector<std::string>& wordList) {
    std::vector<Feedback> feedbacks;
    for (const std::string& guess : wordList) {
        char* feedback = getFeedback(word, guess);
        addOrIncrementFeedback(feedbacks, feedback);
    }
    return std::make_pair(word, feedbacks);
}

std::vector<std::pair<std::string, std::vector<Feedback>>> calculateFeedback(const std::vector<std::string>& wordList) {
    std::vector<std::future<std::pair<std::string, std::vector<Feedback>>>> futures;
    size_t totalWords = wordList.size();

    for (const std::string& word : wordList) {
        futures.push_back(std::async(std::launch::async, calculateFeedbackForWord, word, wordList));
    }

    std::vector<std::pair<std::string, std::vector<Feedback>>> feedbackList;
    for (size_t i = 0; i < futures.size(); ++i) {
        feedbackList.push_back(futures[i].get());

    }

    return feedbackList;
}


bool checkWordInWordlist(const std::string &word,
                         const std::vector<std::string> &wordList)
{
    return std::find(wordList.begin(), wordList.end(), word) != wordList.end();
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

std::vector<LetterResult> readGuessResult(const std::string &word)
{
    std::vector<LetterResult> results;
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
            results.push_back(LetterResult(c, LetterColor::Incorrect));
            break;
        case 1:
            results.push_back(LetterResult(c, LetterColor::Misplaced));
            break;
        case 2:
            results.push_back(LetterResult(c, LetterColor::Correct));
            break;
        }
    }
    return results;
}

GuessResult readInWord(std::vector<GuessResult> &guesses)
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
                std::vector<LetterResult> colors = readGuessResult(word);
                GuessResult guess(word, colors);
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

void filterWords(std::vector<std::string> &wordList, const GuessResult &guess)
{
    std::vector<std::string> candidates;
    std::cout << "Initial word list size: " << wordList.size() << std::endl;

    // Get gray letters
    std::vector<char> grayLetters;
    for (const auto &letterResult : guess.letterResults)
    {
        if (letterResult.color == LetterColor::Incorrect)
        {
            grayLetters.push_back(letterResult.letter);
        }
    }

    for (const std::string &word : wordList)
    {
        bool valid = true;

        // Check for gray letters
        for (const char &grayLetter : grayLetters)
        {
            if (word.find(grayLetter) != std::string::npos)
            {
                valid = false;
                break;
            }
        }

        // Check for misplaced and correct letters
        for (size_t i = 0; i < guess.letterResults.size(); ++i)
        {
            const LetterResult &letterResult = guess.letterResults[i];
            if (letterResult.color == LetterColor::Correct)
            {
                if (word[i] != letterResult.letter)
                {
                    valid = false;
                    break;
                }
            }
            else if (letterResult.color == LetterColor::Misplaced)
            {
                if (word[i] == letterResult.letter ||
                    word.find(letterResult.letter) == std::string::npos)
                {
                    valid = false;
                    break;
                }
            }
        }

        if (valid)
        {
            candidates.push_back(word);
        }
    }

    std::cout << "Filtered word list size: " << candidates.size() << std::endl;
    if (!candidates.empty())
    {
        std::cout << "First candidate: " << candidates[0]
                  << std::endl;
    }
    else
    {
        std::cout << "No candidates found." << std::endl;
    }
    wordList = candidates;
}

int main(int argc, char *argv[])
{
    // DO UNIT TESTING
    // std::vector<GuessResult> guesses;
    //
    // std::vector<std::string> wordList = wordle::wordList;
    // std::cout << "First initial guess: " << wordList[0] << std::endl;
    // size_t wordCount = wordList.size();
    // while (wordCount != 1)
    // {
    //     GuessResult guess = readInWord(guesses);
    //     filterWords(wordList, guess);
    // }
    // std::cout << "The answer is " << wordList[0] << std::endl;

    std::vector<std::string> wordList = {
	"raise",
	"slate",
	"arise",
	"stare",
	"trace",
	"arose",
	"crane",
	"alert",
	"least",
	"later",
	"alter"
    };

    std::vector<std::pair<std::string, std::vector<Feedback>>> feedbacks = calculateFeedback(wordle::wordList);
    for (const auto& pair : feedbacks) {
        const std::string& word = pair.first;
        const std::vector<Feedback>& feedbacksForWord = pair.second;

        std::cout << "Word: " << word << std::endl;
        for (const auto& feedback : feedbacksForWord) {
            auto result = feedback.getFeedback();
            std::cout << "  Feedback: " << result.first << ", Count: " << result.second << std::endl;
        }
    }

    return 0;
}

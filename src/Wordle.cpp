#include "Wordle.h"
#include "Utility.h"
#include "Wordlist.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace Wordle
{

bool checkWordInWordList(const std::string &word,
                         const std::vector<std::string> &wordList)
{
    return std::find(wordList.begin(), wordList.end(), word) != wordList.end();
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

        if (word.length() == 5)
        {
            word = toLowercase(word);
            bool pass = checkWordInWordList(word, Wordle::wordList);
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

        for (const char &grayLetter : grayLetters)
        {
            if (word.find(grayLetter) != std::string::npos)
            {
                valid = false;
                break;
            }
        }

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
        std::cout << "First candidate: " << candidates[0] << std::endl;
    }
    else
    {
        std::cout << "No candidates found." << std::endl;
    }
    wordList = candidates;
}
} // namespace Wordle

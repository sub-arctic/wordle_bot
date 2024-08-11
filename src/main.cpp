#include <iostream>
#include <random>
#include <vector>
#include <limits>
#include <cctype>
#include <string>
#include <algorithm>
#include "wordlist.h"

enum class Color {
    Gray = 0, // Incorrect letter
    Yellow = 1, // Correct letter in wrong position
    Green = 2, // Correct letter in the correct position
};

struct WordleGuess
{
    std::string word;
    std::vector<Color> colors;

    // Constructor to initialize the struct
    WordleGuess(const std::string& w, const std::vector<Color>& c)
        : word(w), colors(c) {}
};

int randomIntInRange(const unsigned int& seed, const int& min, const int& max)
{
    std::mt19937 eng(seed);
    std::uniform_int_distribution<> distr(min, max);
    return distr(eng);
}

bool checkWordInWordlist(const std::string& word, const std::vector<std::string>& wordList)
{
    for (const std::string& string : wordList)
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

std::string toLowercase(const std::string& input) {
    std::string lowercased = input; // Create a copy of the input string
    std::transform(lowercased.begin(), lowercased.end(), lowercased.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowercased; // Return the lowercase string
}

std::vector<Color> readGuessResult(const std::string& word)
{
    std::vector<Color> results;

    int result = 0;
    std::cout << "0 = gray, 1 = yellow, 2 = green" << std::endl;
    for (const char& c : word)
    {
	while (true)
	{
	    std::cout << "Enter the result for letter " << c << ": ";
	    std::cin >> result;

	    // Check if result was successful or not
	    if (std::cin.fail())
	    {
		std::cout << "Invalid input. Please enter a valid integer." << std::endl;
		clearInputBuffer();
	    }
	    else
	    {
		if (result != 0 && result != 1 && result != 2)
		{
		    std::cout << "Invalid input. Please enter either 0, 1 or 2" << std::endl;
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

WordleGuess readInWord(std::vector<WordleGuess>& guesses) 
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
	    if (pass) {
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
	    std::cout << "Invalid input. Please enter a 5 letter word." << std::endl;
	    
	    clearInputBuffer();
    	}
    }
}

int main(int argc, char* argv[])
{
    std::vector<WordleGuess> guesses;

    /* const std::string answer = 
	wordle::wordList[randomIntInRange(std::stoi(argv[1]), 0, wordle::wordCount)];
    std::cout << answer << std::endl; */

    WordleGuess word = readInWord(guesses);
    return 0;
}

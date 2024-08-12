#ifndef WORDLE_H
#define WORDLE_H

#include <string>
#include <vector>

namespace Wordle
{
enum class LetterColor
{
    Correct,
    Misplaced,
    Incorrect
};
struct LetterResult
{
    char letter;
    LetterColor color;

    LetterResult(char l, LetterColor c) : letter(l), color(c) {}
};

struct GuessResult
{
    std::string guess;
    std::vector<LetterResult> letterResults;

    GuessResult(const std::string &g, const std::vector<LetterResult> &lr)
        : guess(g), letterResults(lr)
    {
    }

    std::string getGuess() const { return guess; }
};

bool checkWordInWordList(const std::string &word,
                         const std::vector<std::string> &wordList);

std::vector<LetterResult> readGuessResult(const std::string &word);

GuessResult readInWord(std::vector<GuessResult> &guesses);

void filterWords(std::vector<std::string> &wordlist, const GuessResult &guess);
} // namespace Wordle

#endif

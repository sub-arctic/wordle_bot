#include "Entropy.h"
#include "Wordle.h"
#include "Wordlist.h"
#include <algorithm>
#include <iostream>

int main(int argc, char *argv[])
{
    std::vector<Wordle::GuessResult> guesses;

    std::vector<std::string> wordList = Wordle::wordList;
    std::cout << "First initial guess: " << wordList[0] << std::endl;
    size_t wordCount = wordList.size();
    while (wordCount != 1)
    {
        Wordle::GuessResult guess = readInWord(guesses);
        filterWords(wordList, guess);
        std::vector<std::pair<std::string, std::vector<Entropy::Feedback>>>
            feedbacks = Entropy::calculateFeedback(wordList);
        std::vector<std::pair<std::string, double>> entropyValues;

        for (const auto &pair : feedbacks)
        {
            const std::string &word = pair.first;
            const std::vector<Entropy::Feedback> &feedbacksForWord =
                pair.second;

            double entropy = calculateEntropy(feedbacksForWord);

            entropyValues.emplace_back(word, entropy);
        }

        std::sort(entropyValues.begin(), entropyValues.end(),
                  [](const std::pair<std::string, double> &a,
                     const std::pair<std::string, double> &b)
                  { return a.second > b.second; });

        std::cout << "Top entropy:" << std::endl;
        for (size_t i = 0; i < std::min(entropyValues.size(), size_t(2)); ++i)
        {
            std::cout << "Word: " << entropyValues[i].first
                      << ", Entropy: " << entropyValues[i].second << std::endl;
        }
    }
    std::cout << "The answer is " << wordList[0] << std::endl;

    return 0;
}

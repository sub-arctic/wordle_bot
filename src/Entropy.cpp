#include "Entropy.h"
#include <cmath>
#include <future>
#include <unordered_map>

namespace Entropy
{
void addOrIncrementFeedback(std::vector<Feedback> &feedbacks,
                            const char *feedback)
{
    for (auto &feedbackEntry : feedbacks)
    {
        if (feedbackEntry.matches(feedback))
        {
            feedbackEntry.incrementCount();
            return;
        }
    }
    feedbacks.emplace_back(feedback);
}
std::pair<std::string, std::vector<Feedback>>
calculateFeedbackForWord(const std::string &word,
                         const std::vector<std::string> &wordList)
{
    std::vector<Feedback> feedbacks;
    for (const std::string &guess : wordList)
    {
        char *feedback = getFeedback(word, guess);
        addOrIncrementFeedback(feedbacks, feedback);
    }
    return std::make_pair(word, feedbacks);
}

std::vector<std::pair<std::string, std::vector<Feedback>>>
calculateFeedback(const std::vector<std::string> &wordList)
{
    std::vector<std::future<std::pair<std::string, std::vector<Feedback>>>>
        futures;
    size_t totalWords = wordList.size();

    for (const std::string &word : wordList)
    {
        futures.push_back(std::async(std::launch::async,
                                     calculateFeedbackForWord, word, wordList));
    }

    std::vector<std::pair<std::string, std::vector<Feedback>>> feedbackList;
    for (size_t i = 0; i < futures.size(); ++i)
    {
        feedbackList.push_back(futures[i].get());
    }

    return feedbackList;
}

double calculateEntropy(const std::vector<Feedback> &feedbacks)
{
    double entropy = 0.0;
    int totalCount = 0;

    for (const auto &feedback : feedbacks)
    {
        totalCount += feedback.count;
    }

    for (const auto &feedback : feedbacks)
    {
        if (feedback.count > 0)
        {
            double probability =
                static_cast<double>(feedback.count) / totalCount;
            entropy -= probability * std::log2(probability);
        }
    }

    return entropy;
}

char *getFeedback(const std::string &guess, const std::string &target)
{
    static char feedback[6];

    std::unordered_map<char, int> targetLetterCount;

    std::memset(feedback, 'N', sizeof(feedback) - 1);
    feedback[5] = '\0';

    for (size_t i = 0; i < guess.length(); ++i)
    {
        if (guess[i] == target[i])
        {
            feedback[i] = 'G';
        }
        else
        {
            targetLetterCount[target[i]]++;
        }
    }

    for (size_t i = 0; i < guess.length(); ++i)
    {
        if (feedback[i] == 'N')
        {
            if (targetLetterCount[guess[i]] > 0)
            {
                feedback[i] = 'Y';
                targetLetterCount[guess[i]]--;
            }
        }
    }

    return feedback;
}

} // namespace Entropy

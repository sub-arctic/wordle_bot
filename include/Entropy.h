#ifndef ENTROPY_H
#define ENTROPY_H

#include <cstring>
#include <string>
#include <utility>
#include <vector>

namespace Entropy
{
struct Feedback
{
    char data[6];
    int count;

    Feedback(const char *initialData)
    {
        std::strncpy(data, initialData, sizeof(data) - 1);
        data[sizeof(data) - 1] = '\0';
        count = 0;
    }

    void incrementCount() { count++; }
    bool matches(const char *str) const
    {
        return strncmp(data, str, sizeof(data)) == 0;
    }
    std::pair<const char *, int> getFeedback() const
    {
        return std::make_pair(data, count);
    }
};
void addOrIncrementFeedback(std::vector<Feedback> &feedbacks,
                            const char *feedback);
std::pair<std::string, std::vector<Feedback>>
calculateFeedbackForWord(const std::string &word,
                         const std::vector<std::string> &wordList);

std::vector<std::pair<std::string, std::vector<Feedback>>>
calculateFeedback(const std::vector<std::string> &wordList);

double calculateEntropy(const std::vector<Feedback> &feedbacks);

char *getFeedback(const std::string &guess, const std::string &target);

} // namespace Entropy

#endif

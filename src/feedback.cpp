#include "feedback.h"

char* getFeedback(const std::string& guess, const std::string& target)
{
    static char feedback[6]; // Static array to hold the feedback

    std::unordered_map<char, int> targetLetterCount;

    // Initialize feedback array
    std::memset(feedback, 'N', sizeof(feedback) - 1); // Default all to 'N'
    feedback[5] = '\0'; // Ensure null termination

    for (size_t i = 0; i < guess.length(); ++i)
    {
        if (guess[i] == target[i])
        {
            feedback[i] = 'G'; // Correct letter in the correct position
        }
        else
        {
            targetLetterCount[target[i]]++; // Count letters in target
        }
    }

    for (size_t i = 0; i < guess.length(); ++i)
    {
        if (feedback[i] == 'N') // Only check letters that are not 'G'
        {
            if (targetLetterCount[guess[i]] > 0)
            {
                feedback[i] = 'Y'; // Correct letter in the wrong position
                targetLetterCount[guess[i]]--; // Decrement the count
            }
        }
    }

    return feedback; // Return pointer to the static array
}


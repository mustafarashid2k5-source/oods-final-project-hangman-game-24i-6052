#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <stack>
#include <set>

using namespace std;

const int MAX_WORDS = 200;
const int MAX_LENGTH = 30;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

const char hangmanFig[8][8] = {
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', 'O', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', 'O', ' ', ' ', ' ', ' '},
    {' ', ' ', '/', 'O', '\\', ' ', ' ', ' '},
    {' ', ' ', '/', 'O', '\\', ' ', ' ', ' '},
    {' ', ' ', '/', 'O', '\\', ' ', ' ', ' '},
    {' ', ' ', '/', 'O', '\\', ' ', ' ', ' '},
    {' ', ' ', '/', 'O', '\\', ' ', ' ', ' '}
};

char toLowerCase(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void drawHangman(int attemptsLeft);
void displayGameState(const string& displayWord, const set<char>& usedLetters,
    const set<char>& correctGuesses, const set<char>& wrongGuesses);
bool loadWords(const string& filename, char wordList[MAX_WORDS][MAX_LENGTH], int& count);
void generateHints(const string& word, queue<string>& hints);
void undoLastGuess(stack<char>& guessHistory, set<char>& usedLetters, string& displayWord,
    const string& secretWord, int& attemptsLeft, set<char>& wrongGuesses);

int main() {
    srand(time(0));

    char words[MAX_WORDS][MAX_LENGTH];
    int wordCount = 0;
    if (!loadWords("hangman.txt", words, wordCount) || wordCount == 0) {
        cerr << "Error: Couldn't load words from hangman.txt. Make sure the file exists!" << endl;
        return 1;
    }
    string secretWord = words[rand() % wordCount];
    for (int i = 0; i < secretWord.length(); i++) {
        secretWord[i] = toLowerCase(secretWord[i]);
    }

    string displayWord(secretWord.length(), '_');
    const int MAX_ATTEMPTS = 7;
    int attemptsLeft = MAX_ATTEMPTS;
    char guess;
    string input;

    queue<string> hints;
    generateHints(secretWord, hints);

    stack<char> guessHistory;
    set<char> usedLetters;
    set<char> correctGuesses;
    set<char> wrongGuesses;

    cout << "=== HANGMAN GAME ===" << endl;
    cout << "Try to guess the word! You have " << MAX_ATTEMPTS << " wrong guesses allowed." << endl;
    cout << "Type 'hint' for a clue (costs 1 attempt)" << endl;
    cout << "Type 'undo' to take back your last guess" << endl;
    while (attemptsLeft > 0 && displayWord != secretWord) {
        clearScreen();
        drawHangman(attemptsLeft);
        displayGameState(displayWord, usedLetters, correctGuesses, wrongGuesses);

        cout << "\nYour guess (letter, 'hint', or 'undo'): ";
        getline(cin, input);

        if (input.empty()) continue;
        for (int i = 0; i < input.length(); i++) {
            input[i] = toLowerCase(input[i]);
        }
        if (input == "hint") {
            if (!hints.empty()) {
                attemptsLeft--;
                cout << "\n** HINT ** (attempts left: " << attemptsLeft << ")" << endl;
                cout << hints.front() << endl;
                hints.pop();
            }
            else {
                cout << "\nNo more hints available!" << endl;
            }
            cout << "\nPress Enter to continue...";
            cin.get();
            continue;
        }
        if (input == "undo") {
            undoLastGuess(guessHistory, usedLetters, displayWord, secretWord, attemptsLeft, wrongGuesses);
            cout << "\nPress Enter to continue...";
            cin.get();
            continue;
        }
        if (input.length() != 1 || !isLetter(input[0])) {
            cout << "Invalid! Please enter a single letter." << endl;
            continue;
        }

        guess = input[0];
        if (usedLetters.count(guess)) {
            cout << "You already tried '" << guess << "'!" << endl;
            continue;
        }

        guessHistory.push(guess);
        usedLetters.insert(guess);
        bool found = false;
        for (int i = 0; i < secretWord.length(); i++) {
            if (secretWord[i] == guess) {
                displayWord[i] = guess;
                found = true;
            }
        }

        if (found) {
            cout << "Nice! '" << guess << "' is in the word!" << endl;
            correctGuesses.insert(guess);
        }
        else {
            cout << "Nope! '" << guess << "' is not in the word." << endl;
            attemptsLeft--;
            wrongGuesses.insert(guess);
        }
    }
    clearScreen();
    drawHangman(attemptsLeft);
    displayGameState(displayWord, usedLetters, correctGuesses, wrongGuesses);

    if (displayWord == secretWord) {
        cout << "\n****************************************" << endl;
        cout << "YOU WON! The word was: " << secretWord << endl;
        cout << "****************************************" << endl;
    }
    else {
        cout << "\n****************************************" << endl;
        cout << "GAME OVER!" << endl;
        cout << "The word was: " << secretWord << endl;
        cout << "****************************************" << endl;
        cout << "\nPress Enter to exit...";
        cin.get();
    }

    return 0;
}

bool loadWords(const string& filename, char wordList[MAX_WORDS][MAX_LENGTH], int& count) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    string word;
    count = 0;
    while (file >> word && count < MAX_WORDS) {
        if (word.length() < MAX_LENGTH) {
            for (int i = 0; i < word.length(); i++) {
                wordList[count][i] = word[i];
            }
            wordList[count][word.length()] = '\0';
            count++;
        }
    }
    file.close();
    return true;
}

void generateHints(const string& word, queue<string>& hints) {
    hints.push("The word has " + to_string(word.length()) + " letters.");
    hints.push("First letter is: " + string(1, word[0]));

    if (word.length() > 1) {
        hints.push("Last letter is: " + string(1, word[word.length() - 1]));
    }
}

void drawHangman(int attemptsLeft) {
    int errors = 7 - attemptsLeft;

    cout << "\n  +---+" << endl;
    cout << "  |   |" << endl;
    if (errors >= 1) {
        cout << "  O   |" << endl;
    }
    else {
        cout << "      |" << endl;
    }
    if (errors == 3) {
        cout << "  /|  |" << endl;
    }
    else if (errors >= 4) {
        cout << "  /|\\ |" << endl;
    }
    else if (errors >= 2) {
        cout << "  |   |" << endl;
    }
    else {
        cout << "      |" << endl;
    }
    if (errors == 5) {
        cout << "  /   |" << endl;
    }
    else if (errors >= 6) {
        cout << "  / \\ |" << endl;
    }
    else {
        cout << "      |" << endl;
    }

    cout << "      |" << endl;
    cout << "=========" << endl;
    cout << "Attempts left: " << attemptsLeft << endl;
}

void displayGameState(const string& displayWord, const set<char>& usedLetters,
    const set<char>& correctGuesses, const set<char>& wrongGuesses) {
    cout << "\nWord: ";
    for (int i = 0; i < displayWord.length(); i++) {
        cout << displayWord[i] << " ";
    }
    cout << endl;

    cout << "Used letters: ";
    for (char c : usedLetters) {
        cout << c << " ";
    }
    cout << endl;

    cout << "Correct: ";
    for (char c : correctGuesses) {
        cout << c << " ";
    }
    cout << endl;

    cout << "Wrong: ";
    for (char c : wrongGuesses) {
        cout << c << " ";
    }
    cout << endl;
}

void undoLastGuess(stack<char>& guessHistory, set<char>& usedLetters, string& displayWord,
    const string& secretWord, int& attemptsLeft, set<char>& wrongGuesses) {
    if (guessHistory.empty()) {
        cout << "\nNothing to undo!" << endl;
        return;
    }

    char lastGuess = guessHistory.top();
    guessHistory.pop();
    usedLetters.erase(lastGuess);

    cout << "\n** UNDO ** Taking back: '" << lastGuess << "'" << endl;
    if (secretWord.find(lastGuess) == string::npos) {
        attemptsLeft++;
        wrongGuesses.erase(lastGuess);
        cout << "Attempt restored! (" << attemptsLeft << " left)" << endl;
    }
    else {
        for (int i = 0; i < secretWord.length(); i++) {
            if (secretWord[i] == lastGuess) {
             
                bool stillRevealed = false;
                for (char c : usedLetters) {
                    if (secretWord[i] == c) {
                        stillRevealed = true;
                        break;
                    }
                }

                if (!stillRevealed) {
                    displayWord[i] = '_';
                }
            }
        }
        cout << "Letter hidden again!" << endl;
    }
}
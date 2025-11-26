bool loadW(const string& f_name, char w_list[MAX_W][MAX_LEN], int& w_count) {
    ifstream file(f_name);
    if (!file.is_open()) {
        return false;
    }
    string word;
    w_count = 0;
    while (file >> word && w_count < MAX_W) {
        if (word.length() < MAX_LEN) {
            strcpy(w_list[w_count], word.c_str());
            w_count++;
        }
    }
    file.close();
    return true;
}

void genHints(const string& s_word, queue<string>& h_q) {
    h_q.push("The word has " + to_string(s_word.length()) + " letters.");
    h_q.push("The first letter of the word is '" + string(1, s_word[0]) + "'.");
    if (s_word.length() > 1) {
        string secondLetter = string(1, s_word[1]);
        if (s_word.find_last_of(secondLetter) != 1) {
            h_q.push("The letter '" + secondLetter + "' appears more than once.");
        } else {
            h_q.push("The last letter of the word is '" + string(1, s_word.back()) + "'.");
        }
    }
}
void drawMan(int a_left) {
    int errorCount = 7 - a_left;
    cout << "\n--- HANGMAN ---" << endl;
    cout << "  +---+" << endl;
    cout << "  |   |" << endl;

    
    if (errorCount >= 1) {
        cout << "  " << HANGMAN_FIGURE[1][3] << "   |" << endl;
    } else {
        cout << "      |" << endl;
    }

    if (errorCount == 3) {
        cout << "  /O  |" << endl;
    } else if (errorCount == 4) {
        cout << "  /O\\ |" << endl; 
    } else if (errorCount >= 2) {
        cout << "   " << HANGMAN_FIGURE[2][3] << "  |" << endl; 
    } else {
        cout << "      |" << endl;
    }

    if (errorCount == 5) {
        cout << "  /   |" << endl; 
    } else if (errorCount >= 6) {
        cout << "  / \\ |" << endl; 
    } else {
        cout << "      |" << endl;
    }

    cout << "      |" << endl;
    cout << "=========" << endl;
    cout << "Attempts Left: " << a_left << endl;
}

void showState(const string& d_word, const set<char>& used, const set<char>& c_set, const set<char>& w_set) {
    cout << "\nWord: ";
    for (char c : d_word) {
        cout << c << " ";
    }
    cout << endl;

    cout << "-----------------------------------------------" << endl;
    cout << "Used Letters (Total): ";
    for (char c : used) {
        cout << c << " ";
    }
    cout << endl;

    cout << "Correct Guesses: ";
    for (char c : c_set) {
        cout << c << " ";
    }
    cout << endl;

    cout << "Wrong Guesses: ";
    for (char c : w_set) {
        cout << c << " ";
    }
    cout << endl;
    cout << "-----------------------------------------------" << endl;
}
void undoCheat(stack<char>& g_hist, set<char>& used, string& d_word, const string& s_word, int& a_left, set<char>& w_set) {
    if (g_hist.empty()) {
        cout << "\n[FAILED] No previous guesses to undo." << endl;
        return;
    }

    char lastGuess = g_hist.top();
    g_hist.pop();
    used.erase(lastGuess);

    cout << "\n[CHEAT] undoing last guess: '" << lastGuess << "'" << endl;
    if (s_word.find(lastGuess) == string::npos) {
        a_left++;
        w_set.erase(lastGuess); 
        cout << "Attempt restored! Attempts left: " << a_left << endl;
    } else {
        cout << "Correct letter reverted. The word display has changed." << endl;
        for (size_t i = 0; i < s_word.length(); ++i) {
            if (s_word[i] == lastGuess) {
                bool isStillGuessedByOtherLetters = false;
                for (char currentCorrect : used) {
                    if (s_word[i] == currentCorrect) {
                        isStillGuessedByOtherLetters = true;
                        break;
                    }
                }
                if (!isStillGuessedByOtherLetters) {
                    d_word[i] = '_';
                }
            }
        }
    }
}
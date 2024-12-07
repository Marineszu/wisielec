#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <map>

using namespace std;

// Struktura do przechowywania wyników
struct HighScore {
    string name;
    int score;
};

// Funkcja do wczytywania s³ów z pliku
vector<string> loadWords(const string& filename) {
    vector<string> words;
    ifstream file(filename);
    string word;

    if (!file.is_open()) {
        cerr << "Blad: Nie mozna otworzyc pliku z slowami!" << endl;
        exit(1);
    }

    while (file >> word) {
        words.push_back(word);
    }

    file.close();
    return words;
}

// Funkcja do wczytywania najlepszych wyników z pliku
vector<HighScore> loadHighScores(const string& filename) {
    vector<HighScore> scores;
    ifstream file(filename);
    string name;
    int score;

    if (file.is_open()) {
        while (file >> name >> score) {
            scores.push_back({ name, score });
        }
        file.close();
    }
    return scores;
}

// Funkcja do zapisywania najlepszych wyników do pliku
void saveHighScores(const string& filename, const vector<HighScore>& scores) {
    ofstream file(filename);

    if (file.is_open()) {
        for (const auto& hs : scores) {
            file << hs.name << " " << hs.score << endl;
        }
        file.close();
    }
    else {
        cerr << "Blad: Nie mozna zapisac najlepszych wynikow!" << endl;
    }
}

// Wyœwietlanie najlepszych wyników
void displayHighScores(const vector<HighScore>& scores) {
    cout << "\n=== Lista Najlepszych Graczy ===\n";
    for (size_t i = 0; i < scores.size(); ++i) {
        cout << i + 1 << ". " << scores[i].name << " - " << scores[i].score << " pkt" << endl;
    }
    cout << "===============================\n";
}

// Losowanie s³owa z listy
string chooseRandomWord(const vector<string>& words) {
    srand(time(nullptr));
    return words[rand() % words.size()];
}

// Klasa gry w Wisielca
class HangmanGame {
private:
    vector<string> words;
    vector<HighScore> highScores;
    string highScoresFile;
    string word;
    string guessedWord;
    map<char, bool> guessedLetters;
    int attemptsLeft;
    time_t startTime;

public:
    HangmanGame(const vector<string>& words, const string& highScoresFile)
        : words(words), highScoresFile(highScoresFile), attemptsLeft(6) {
        highScores = loadHighScores(highScoresFile);
    }

    // Obs³uga wejœcia
    char handleInput() {
        char guess;
        cout << "Pozostale proby: " << attemptsLeft << ". Podaj litere: ";
        cin >> guess;
        return guess;
    }

    // Aktualizacja logiki gry
    void update(char guess) {
        if (guessedLetters[guess]) {
            cout << "Juz zgadywales te litere!" << endl;
            return;
        }

        guessedLetters[guess] = true;

        bool correct = false;
        for (size_t i = 0; i < word.size(); ++i) {
            if (word[i] == guess) {
                guessedWord[i] = guess;
                correct = true;
            }
        }

        if (correct) {
            cout << "Dobrze! Slowo: " << guessedWord << endl;
        }
        else {
            cout << "Zle! Litery nie ma w slowie." << endl;
            attemptsLeft--;
        }
    }

    // Renderowanie stanu gry
    void render() const {
        cout << "Slowo: " << guessedWord << endl;
    }

    // Sprawdzenie warunków zakoñczenia gry
    bool isGameOver() const {
        return (attemptsLeft <= 0 || guessedWord == word);
    }

    // Uruchomienie gry
    void startGame() {
        word = chooseRandomWord(words);
        guessedWord = string(word.size(), '_');
        guessedLetters.clear();
        attemptsLeft = 6;
        startTime = time(nullptr);

        cout << "\nRozpoczynamy gre w Wisielca!\n";

        while (!isGameOver()) {
            render();
            char guess = handleInput();
            update(guess);
        }

        time_t endTime = time(nullptr);
        int timeTaken = static_cast<int>(endTime - startTime);
        int score = (guessedWord == word ? attemptsLeft * 100 - timeTaken : 0);

        if (guessedWord == word) {
            cout << "\nGratulacje! Zgadles slowo: " << word << endl;
            cout << "Twoj wynik: " << score << " pkt" << endl;

            if (score > 0) {
                string playerName;
                cout << "Podaj swoje imie: ";
                cin >> playerName;
                highScores.push_back({ playerName, score });
                sort(highScores.begin(), highScores.end(), [](const HighScore& a, const HighScore& b) {
                    return b.score > a.score;
                    });
                if (highScores.size() > 10) {
                    highScores.pop_back();
                }
                saveHighScores(highScoresFile, highScores);
            }
        }
        else {
            cout << "\nNiestety, przegrales. Slowo to: " << word << endl;
        }
    }

    // Wyœwietlanie najlepszych wyników
    void showHighScores() const {
        displayHighScores(highScores);
    }
};

int main() {
    const string wordsFile = "words.txt"; // Plik z pul¹ s³ów
    const string highScoresFile = "high_scores.txt"; // Plik z najlepszymi wynikami

    vector<string> words = loadWords(wordsFile);
    HangmanGame game(words, highScoresFile);

    game.showHighScores();

    char playAgain;
    do {
        game.startGame();
        cout << "Czy chcesz zagrac ponownie? (t/n): ";
        cin >> playAgain;
    } while (playAgain == 't');

    cout << "Dziekujemy za gre w Wisielca!" << endl;
    return 0;
}

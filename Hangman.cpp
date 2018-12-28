#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace std;

/* The maximum possible number of tries given to the user per word*/
const static int MAX_TRIES = 7;

static map<string, string> wordsWithClues;   // useful for fast retrieval of word-clue pairs
static vector<string> words;                 // the list of all the words for the hangman game, which will act like a stack

/*
* Used for trimming the whitespace at the start and end of a string
*/
static string trim(const string& str) {

	size_t first = str.find_first_not_of(' ');
	if (first == string::npos)
	{
		return "";
	}

	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

/*
* Setups the hangman game by displaying a welcome message and by reading
* in a file of word-clue pairs. The file reading ignores trailing whitespace
* when parsing the words and their corresponding clues. 
*
* File Lines should be structured as: 
*  <word> : <clue>
*
*
* wordsWithClues - The map of the Word-Clue key-value pairs
* words - The list of all the words that are added to
* Returns an int representing the number of words successfully loaded from the file
*/
static int setup() {
	cout << "Welcome to Hangman!" << endl;
	cout << "Try to guess all the hidden words" << endl;
	cout << endl;

	// for reading the file
	ifstream wordFile;
	string line;

	// determines how many words have been loaded before the game is run
	int wordsLoaded = 0;   

	wordFile.open("Hangman.txt");
	if (wordFile.is_open()) {
		while (getline(wordFile, line)) {

			// check for delimiter between the word and clue. If doesn't exist, ignore this word
			int delimiterPos = line.find(":");
			if (delimiterPos != string::npos && (delimiterPos + 1) <= line.length()) {

				// attempt to gather the words and the clues from the file
				string hangmanWord = trim(line.substr(0, delimiterPos));
				string clue = trim(line.substr(delimiterPos + 1));

				// if there is no hangman word, skip the word. otherwise, transform the word to lowercase
				if (!hangmanWord.empty()) {
					transform(hangmanWord.begin(), hangmanWord.end(), hangmanWord.begin(), ::tolower);
				}
				else {
					continue;
				}

				// if hangman word exists, but no clue has been provided, just put N/A. Otherwise transform to lowercase
				if (!clue.empty()) {
					transform(clue.begin(), clue.end(), clue.begin(), ::tolower);
				}
				else {
					clue = "N/A";
				}
				
				// add to the map and the list of words
				wordsWithClues.insert(pair<string, string>(hangmanWord, clue));
				words.push_back(hangmanWord);

				wordsLoaded++;

			}
		}
	}
	wordFile.close();
	return wordsLoaded;
}

/*
* For Debug Purposes only. Prints the contents of the map and the vector of words
*/
static void printWords() {

	cout << "|  MAP  |" << endl;
	map<string, string>::const_iterator ma;
	for (ma = wordsWithClues.begin(); ma != wordsWithClues.end(); ma++) {
		cout << ma->first << ": " << ma->second << endl;
	}

	cout << "---------------------------------------------" << endl;

	cout << "|  WORDS  |" << endl;
	vector<string>::const_iterator it;
	for (it = words.begin(); it != words.end(); it++) {
		cout << *it << endl;
	}
}

static int updateWords(const string& actualWord, string& hiddenWord, const char& firstChar, int& numTriesLeft) {
	
	if (actualWord.find(firstChar) != string::npos) {

		for (int i = 0; i < actualWord.length(); i++) {
			if (actualWord[i] == firstChar) {
				hiddenWord[i] = firstChar;
			}
		}

		cout << "Letter '" << firstChar << "' found" << endl;
		cout << "=========================" << endl;

	}
	else {

		cout << "Sorry, there's no letter '" << firstChar << "'" << endl;
		cout << "=========================" << endl;
		numTriesLeft--;
		if (numTriesLeft <= 0) {
			return -1;
		}

	}

	return 0;
}


static int gameLoop(const string& actualWord, string& hiddenWord, string& lettersUsed, string& guess) {

	int numTriesLeft = MAX_TRIES;       // the number of tries the user has left

	while (hiddenWord != actualWord) {

		cout << "You have " << numTriesLeft << " tries left" << endl;
		cout << endl;

		cout << "Letters used already: " << lettersUsed << endl;
		cout << endl;

		cout << "Clue: " << wordsWithClues[actualWord] << endl;
		cout << endl;

		cout << "The current word is:  " << hiddenWord << endl;
		cout << endl;

		cout << "Enter a letter: " << flush;
		getline(cin, guess);

		guess = trim(guess);
		transform(guess.begin(), guess.end(), guess.begin(), ::tolower);

		char firstChar = guess[0];

		if (lettersUsed.find(firstChar) != string::npos) { 
			cout << "You've used that letter already!" << endl;
			cout << endl;
			continue;
		}

		lettersUsed += firstChar;
		sort(lettersUsed.begin(), lettersUsed.end());

		if (updateWords(actualWord, hiddenWord, firstChar, numTriesLeft) != 0) {
			return -1;
		}
	}

	return 0;
}

int main() {

	int numWordsCorrect = 0;    // the number of words correctly guessed

	string lettersUsed = "";  // the letters the user already guessed
	string actualWord = "";   // the actual word the user is trying to guess
	string guess = "";        // the user's guess

    setup();

	// randomly shuffle the words loaded in
	srand(static_cast<unsigned int>(time(0)));
	random_shuffle(words.begin(), words.end());

	printWords();

	while (!words.empty()) {

		// get the last word in the word list, and pop off the back
		actualWord = words.back();
		words.pop_back();

		// the hidden word filled with dashes that gets populated on user guesses
		string hiddenWord(actualWord.length(), '-');

		if (gameLoop(actualWord, hiddenWord, lettersUsed, guess) == 0) {
			cout << "CORRECT! Loading the next word." << endl;
			cout << "-------------------------------------" << endl;
			cout << endl;
			lettersUsed.clear();
			numWordsCorrect++;
		} else {
			cout << "GAME OVER." << endl;
			cout << "You successfully solved " << numWordsCorrect << " words" << endl;
			return 0;
		}
	}

	cout << endl;
	cout << "CONGRATULATIONS! YOU BEAT THE GAME!" << endl;

	return 0;
}

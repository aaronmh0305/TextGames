#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <thread>         
#include <chrono>        

const int NUM_WORDS = 100;

/*
* A structure for holding word information
*/
struct WordData {
	std::string word;
	std::string hint;
	bool visited;
};

/*
* Used for trimming the whitespace at the start and end of a string
*/
static std::string trim(const std::string& str) {

	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
	{
		return "";
	}

	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

/*
* Setups the game and loads in the file with all the words and hints
*/
static int setup(WordData* data[NUM_WORDS]) {
	std::cout << "----------------------------------" << std::endl;
	std::cout << "| Welcome to the Word Scrambler! |" << std::endl;
	std::cout << "----------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "Unscramble the letters to make a word." << std::endl;
	std::cout << "Enter 'hint' to receive a hint." << std::endl;
	std::cout << "Enter 'quit' to quit the game." << std::endl;
	std::cout << std::endl;

	std::ifstream wordFile;
	std::string line;
	int index = 0;

	wordFile.open("words.txt");
	if (wordFile.is_open()) {
		while (std::getline(wordFile, line) && index < NUM_WORDS) {

			int delimiterPos = line.find(":");
			if (delimiterPos != std::string::npos && (delimiterPos + 1) <= line.length()) {

				data[index] = new WordData();
				data[index]->word = trim(line.substr(0, line.find(":")));
				data[index]->hint = trim(line.substr(line.find(":") + 1));
				data[index]->visited = false;
				
				index++;

			}
		}
	}
	wordFile.close();
	return index;
}

/*
* Scrambles the word passed in by exchanging characters based on its length
*/
static void scramble(std::string& strToJumble) {
	
	for (int i = 0; i < strToJumble.length(); i++) {

		int char1 = rand() % strToJumble.length();
		int char2 = rand() % strToJumble.length();

		int temp = strToJumble[char1];
		strToJumble[char1] = strToJumble[char2];
		strToJumble[char2] = temp;

	}
}

/*
* Lets the user attempt to make a guess of the actual scrambled word, request a hint, or quit the game
*/
static std::string attemptGuess(std::string& guess, std::string& actualWord, std::string& hint) {
	
	do {
		
		std::cout << "Enter your guess: " << std::flush;
		getline(std::cin, guess);

		guess = trim(guess);

		if (guess == "quit") {
			std::cout << "Okay. Bye Bye." << std::endl;
			return guess;
		}
		else if (guess == actualWord) {
			std::cout << "Correct! Loading the next word...\n" << std::endl;
			return guess;
		}
		else if (guess == "hint") {
			std::cout << "Hint: " << hint << std::endl;
			std::cout << std::endl;
		}
		else {
			std::cout << "Incorrect. Try again.\n" << std::endl;
		}
		
	} while (guess != actualWord && guess != "quit");
}

/*
* Deletes all dynamically allocated memory
*/
static void cleanup(WordData* data[NUM_WORDS], int wordsLoaded) {

	for (int i = 0; i < wordsLoaded; i++) {
		delete data[i];
		data[i] = nullptr;
	}
}


int main() {

	WordData* data[NUM_WORDS];
	
	int wordsLoaded = setup(data);

	// if less than 20 words were loaded, then quit the game
	if (wordsLoaded < 20) {
		std::cout << "There needs to be at least 20 words loaded.." << std::endl;
		exit(1);
	}

	std::string currentJumble;    // represents the current jumbled word
	std::string guess;            // represents the user guess

	int wordsToGuess = 0;         // represents the number of words the user wants to unscramble between 10 and 20

	// let the user specify how many words they want to unscramble
	while (wordsToGuess < 10 || wordsToGuess > 20) {

		try {

			std::cout << "How many words would you like to unscramble between 10 and 20? " << std::flush;
			getline(std::cin, guess);

			wordsToGuess = stoi(guess);

			if (wordsToGuess < 10 || wordsToGuess > 20) {
				std::cout << "Please enter a number between 10 and 20." << std::endl;
				std::cout << std::endl;
			}
		}
		catch (const std::exception& ia) {
			std::cout << "Please enter a number between 10 and 20." << std::endl;
			std::cout << std::endl;
		}
	} 

	std::cout << std::endl;

	// seed the random number generator
	srand(static_cast<unsigned int>(time(0)));

	// main game loop
	int randomNum = 0;
	int wordCounter = 1;
	while (wordsToGuess > 0) {

		// randomly pick a word, given it isn't visited already
		randomNum = rand() % wordsLoaded;
		if (!data[randomNum]->visited) {

			data[randomNum]->visited = true;
			currentJumble = data[randomNum]->word;

			scramble(currentJumble);

			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Jumble #" << wordCounter << ": " << currentJumble << std::endl;
			std::cout << std::endl;

			if (attemptGuess(guess, data[randomNum]->word, data[randomNum]->hint) == "quit") {
				cleanup(data, wordsLoaded);
				return 0;;
			}

			wordCounter++;
			wordsToGuess--;
		}

	}

	cleanup(data, wordsLoaded);

	std::cout << std::endl;
	std::cout << "CONGRATULATIONS! YOU BEAT THE GAME!" << std::endl;

	// close the screen within 5 seconds after beating the game
	std::cout << "Closing in 5 seconds" << std::endl;
	for (int i = 5; i > 0; i--) {
		std::cout << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
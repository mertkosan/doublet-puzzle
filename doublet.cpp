#include <unordered_map>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// this should be class
struct Word {
	string name;
	string parent;
	int g;
	int f;

	Word(string name, string parent, int g, int f) {
		this->name = name;
		this->parent = parent;
		this->g = g;
		this->f = g;
	}
};


// globals
unordered_map<string, bool> dictionary;

auto cmp = [](Word left, Word right) { return left.f > right.f; };
priority_queue<Word, vector<Word>, decltype(cmp)> my_queue(cmp);
unordered_map<string, string> finished;
int numberOfExpansion = 0;

// uppercase of letters
string uppercase(string str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		str[i] = toupper(str[i]);
	}
	return str;
}

// trimming right white spaces
string trim(string str)
{
	while (str[str.length() - 1] == ' ')
		str = str.substr(0, str.length() - 2);
	return str;
}

// creating hash map dictionary from words
unordered_map<string, bool> createDictionary(string wordsPath, int lengthOfWords) {
	unordered_map<string, bool> dictionary;
	ifstream ifs(wordsPath);
	string temp;
	getline(ifs, temp);
	while (getline(ifs, temp)) {
		temp = uppercase(temp);
		temp = trim(temp);
		if(temp.length() == lengthOfWords) // put to dictionary if length condition is satisfied.
			dictionary[temp] = true;
	}
	ifs.close();
	return dictionary;
}

// heuristic function for doublet game
int letterDifference(string word1, string word2) {
	int count = 0;
	for (unsigned int i = 0; i < word1.length(); i++)
		if (word1[i] != word2[i])
			count++;
	return count;	
}

// getting children
vector<string> createOneLetterDifferenceWords(string word) {
	int len = word.length();
	vector<string> possibleWords;
	for (int i = 0; i < len; i++) {
		char base = word[i];
		for (char j = 'A'; j <= 'Z'; j++) {
			if (j != base) {
				string newWord = word.substr(0, i) + j + word.substr(i + 1);
				if(dictionary[newWord])
					possibleWords.push_back(newWord);
			}
		}
	}
	return possibleWords;
}

// A* search for Doublet
bool search(string start, string finish) {
	//first node
	Word first(start, start, 0, 0 + letterDifference(start, finish));
	my_queue.push(first);

	bool found = false;
	while (!my_queue.empty() && !found) {
		Word word = my_queue.top();
		my_queue.pop();
		numberOfExpansion++;
		finished[word.name] = word.parent;
		if (word.name == finish) 
			found = true;
		else {
			vector<string> children = createOneLetterDifferenceWords(word.name);
			for (int i = 0; i < children.size(); i++) {
				string newWord = children[i];
				if (finished[newWord] == "") { // child is not finished!
					int newf = word.f + 1; // 1 is cost to change word
					int newg = newf + letterDifference(newWord, finish);
					Word newWordNode = Word(newWord, word.name, newf, newg);
					my_queue.push(newWordNode);
				}
			}
		}
	}
	return found;
}

void printResult(string start, string finish) {
	string current = finish;
	vector<string> willBePrinted;
	while (current != start) {
		willBePrinted.push_back(current);
		current = finished[current];
	}
	willBePrinted.push_back(start);
	for (int i = willBePrinted.size()-1; i >= 0; i--) {
		cout << willBePrinted[i] << endl;
	}
	cout << "expansions = " << numberOfExpansion << endl;
}

void errorResult() {
	cout << "No transformation" << endl;
	cout << "expansions =  " << numberOfExpansion << endl;
}

int main(int argc, char* argv[]) {
	string firstWord = uppercase(argv[1]);
	string secondWord = uppercase(argv[2]);
	string wordsPath = argv[3];

	if (firstWord.length() != secondWord.length())
		errorResult();
	else {
		dictionary = createDictionary(wordsPath, firstWord.length());
		bool success = search(firstWord, secondWord);
		if (success)
			printResult(firstWord, secondWord);
		else 
			errorResult();
	}

	cin.get();
	return 0;
}


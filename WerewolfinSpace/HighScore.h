#pragma once
#include <fstream>
#include "DxHandler.h"
using namespace std;
class HighScore
{
private:
	vector<string> entries;
	vector<int> score;

	ID3DX10Font* font;
	ID3DX10Sprite* sprite;

	vector<RECT> r;
	RECT r2;

	string defaultString;
	int numberOfEntries, replaceAtIndex, listIndex;

	string fileName();

	void readFile(bool checkForFaults);
	void saveFile();
	void createFile();

public:
	HighScore(ID3DX10Font* font,ID3DX10Sprite* sprite, int index);
	~HighScore();

	void Draw(DxHandler* h);
	void addEntry(string _entry);
	bool checkAgainstHighScore(int _score);
	
	int getIndex() { return replaceAtIndex + 1; }
};
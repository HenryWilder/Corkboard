#include "Serialize.h"
#include <fstream>
#include <unordered_map>
#include "Thread.h"
#include "Notecard.h"
#include "CBButton.h"

std::string g_activeBoard;

/*
* v00.000.001
* c 2 t 1
* 
* x 23.364 y 58.324 r 255 g 255 b 255 ### Title ``` Content ```
* x 753.17777 y 266.2432 r 255 g 0 b 0 ###  ```  ```
* 
* s 0 d 1 r 100 g 140 b 140
* 
*/
void SaveBoard(std::string filename)
{
	std::unordered_map<Notecard*, int> cardNumber;

	std::ofstream destFile = std::ofstream(filename);

	destFile << "v00.000.001\n"; // Version number
	destFile
		<< "c " << g_cards.size() // Number of cards
		<< " t " << g_threads.size() << '\n'; // Number of threads

	destFile << '\n';

	int i = 0;
	for (Notecard* card : g_cards)
	{
		cardNumber.emplace(card, i);
		destFile
			<< "x " << (float)card->position.x << " y " << (float)card->position.y // Position
			<< " r " << (int)card->color.r << " g " << (int)card->color.g << " b " << (int)card->color.b // Color
			<< " ### " << card->title << " ``` " << card->content << " ```\n"; // Content
		++i;
	}

	destFile << '\n';

	for (Thread* thread : g_threads)
	{
		destFile
			<< "s " << (int)cardNumber.find(thread->start)->second // Source
			<< " d " << (int)cardNumber.find(thread->end)->second // Destination
			<< " r " << (int)thread->color.r << " g " << (int)thread->color.g << " b " << (int)thread->color.b << '\n'; // Color
	}

	destFile.close();
}

/*
* v%i.%i.%i
* c %i t %i
* 
* x %f y %f r %i g %i b %i ### %s ``` %s ```
* x %f y %f r %i g %i b %i ### %s ``` %s ```
* 
* s %i d %i r %i g %i b %i
* 
*/
void LoadBoard(std::string filename)
{
	g_cards.clear();
	g_threads.clear();

	std::ifstream srcFile = std::ifstream(filename);

	std::string code;
	if (srcFile >> code; code != "v00.000.001") goto Error;

	int cardsCount;
	int threadsCount;

	// Card count
	if (srcFile >> code; code != "c") goto Error;
	srcFile >> cardsCount;

	// Thread count
	if (srcFile >> code; code != "t") goto Error;
	srcFile >> threadsCount;
	g_threads.reserve(threadsCount);

	// Load cards
	for (int i = 0; i < cardsCount; ++i)
	{
		Vector2 position{0,0}; // Position
		int r, g, b; // Color
		std::string title, content; // Text

		// Position
		if (srcFile >> code; code != "x") goto Error;
		srcFile >> position.x;
		if (srcFile >> code; code != "y") goto Error;
		srcFile >> position.y;
		if (srcFile >> code; code != "r") goto Error;
		srcFile >> r;
		if (srcFile >> code; code != "g") goto Error;
		srcFile >> g;
		if (srcFile >> code; code != "b") goto Error;
		srcFile >> b;
		if (srcFile >> code; code != "###") goto Error;
		std::getline(srcFile, title, '`');
		// Trim leading/trailing spaces
		title.erase(0, 1);
		title.pop_back();
		if (srcFile >> code; code != "``") goto Error;
		std::getline(srcFile, content, '`');
		// Trim leading/trailing spaces
		content.erase(0, 1);
		content.pop_back();
		if (srcFile >> code; code != "``") goto Error;

		Notecard* card = new Notecard(position, {(unsigned char)r,(unsigned char)g,(unsigned char)b,255}, title, content);
		g_cards.push_back(card);
	}

	// Load threads
	for (int i = 0; i < threadsCount; ++i)
	{
		int sourceIndex, destinationIndex;
		int r, g, b;

		if (srcFile >> code; code != "s") goto Error;
		srcFile >> sourceIndex;
		if (srcFile >> code; code != "d") goto Error;
		srcFile >> destinationIndex;

		if (sourceIndex == destinationIndex) goto Error;

		if (srcFile >> code; code != "r") goto Error;
		srcFile >> r;
		if (srcFile >> code; code != "g") goto Error;
		srcFile >> g;
		if (srcFile >> code; code != "b") goto Error;
		srcFile >> b;

		Notecard* source = g_cards[sourceIndex];
		Notecard* destination = g_cards[destinationIndex];
		Thread* thread = new Thread({(unsigned char)r,(unsigned char)g,(unsigned char)b,255}, source, destination);
		source->AddThreadConnection(thread);
		destination->AddThreadConnection(thread);
		g_threads.push_back(thread);
	}

	goto Close;

Error:
	printf("File corrupted");
Close:
	srcFile.close();
}

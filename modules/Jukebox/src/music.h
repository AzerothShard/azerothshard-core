#include "Common.h"

class music
{
public:
	//GETTERS
	string GetAuthor() const;
	string GetTitle() const;
	uint32 GetMusicId() const;

	//SETTERS
	void SetAuthor(string author);
	void SetTitle(string title);
	void SetMusicId(uint32 musicId);

	music();
	music(string author, string title, uint32 musicId);

private:
	string author;
	string title;
	uint32 musicId;
};

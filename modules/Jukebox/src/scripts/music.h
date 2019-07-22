#include "Common.h"

class music
{
public:
	//GETTERS
	string GetAuthor() const;
	string GetTitle() const;
	uint32 GetMusicId() const;
    string GetGenre() const;

	//SETTERS
	void SetAuthor(string author);
	void SetTitle(string title);
	void SetMusicId(uint32 musicId);
    void SetGenre(string genre);

	music();
	music(string author, string title, uint32 musicId, string genre);

private:
	string author;
	string title;
	uint32 musicId;
    string genre;
};

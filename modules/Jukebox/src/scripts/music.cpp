#include "music.h"

music::music()
{
	author = string("");
	title = string("");
	musicId = uint32(0);
    genre = string("");
}

music::music(string ItsAuthor, string ItsTitle, uint32 ItsMusicId, string ItsGenre)
{
	author = ItsAuthor;
	title = ItsTitle;
	musicId = ItsMusicId;
    genre = ItsGenre;
}

string music::GetAuthor() const
{
	return author;
}


void music::SetAuthor(string ItsAuthor)
{
	author = ItsAuthor;
}

string music::GetTitle() const
{
	return title;
}


void music::SetTitle(string ItsTitle)
{
	title = ItsTitle;
}

uint32 music::GetMusicId() const
{
	return musicId;
}

void music::SetMusicId(uint32 ItsMusicId)
{
	musicId = ItsMusicId;
}

string music::GetGenre() const
{
    return genre;
}


void music::SetGenre(string ItsGenre)
{
    genre = ItsGenre;
}
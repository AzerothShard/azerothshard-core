#include "music.h"

music::music()
{
	author = string("");
	title = string("");
	musicId = uint32(0);
}

music::music(string ItsAuthor, string ItsTitle, uint32 ItsMusicId)
{
	author = ItsAuthor;
	title = ItsTitle;
	musicId = ItsMusicId;
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
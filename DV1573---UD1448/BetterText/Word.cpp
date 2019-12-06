#include <Pch/Pch.h>
#include "Word.h"

Word::Word(float fontSize)
{
	m_fontSize = fontSize;
	m_width = 0.0f;
}

void Word::addCharacter(Character c)
{
	m_characters.push_back(c);
	m_width += c.xAdvance * m_fontSize;
}

const std::vector<Character>& Word::getCharacters() const
{
	return m_characters;
}

const float& Word::getWordWidth() const
{
	return m_width;
}

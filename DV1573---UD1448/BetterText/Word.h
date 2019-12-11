#ifndef _WORD_H
#define _WORD_H
#include <Pch/Pch.h>
#include "Character.h"

class Word {
public:

	Word(float fontSize);

	void addCharacter(Character c);
	const std::vector<Character>& getCharacters() const;
	const float& getWordWidth() const;


private:
	std::vector<Character> m_characters;
	float m_width;
	float m_fontSize;

};

#endif
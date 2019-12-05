#ifndef _LINE_H
#define _LINE_H
#include <Pch/Pch.h>
#include "Word.h"

class Line {

public:
	Line(float spaceWidth, float fontSize, float maxLength);
	
	bool attemptToAddWord(Word word);
	const float& getMaxLength() const;
	const float& getCurrentLength() const;
	const std::vector<Word>& getWords() const;

private:
	float m_maxLength;
	float m_spaceWidth;
	float m_currentLength;
	std::vector<Word> m_words;
	
};

#endif
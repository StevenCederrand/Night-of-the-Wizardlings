#include <Pch/Pch.h>
#include "Line.h"

Line::Line(float spaceWidth, float fontSize, float maxLength)
{
	m_spaceWidth = spaceWidth * fontSize;
	m_maxLength = maxLength;
	m_currentLength = 0.0f;
}

bool Line::attemptToAddWord(Word word)
{
	float additionalWidth = word.getWordWidth();

	additionalWidth += !m_words.empty() ? m_spaceWidth : 0.0f;

	if (m_currentLength + additionalWidth <= m_maxLength) {
		m_words.push_back(word);
		m_currentLength += additionalWidth;
		return true;
	}

	return false;
}

const float& Line::getMaxLength() const
{
	return m_maxLength;
}

const float& Line::getCurrentLength() const
{
	return m_currentLength;
}

const std::vector<Word>& Line::getWords() const
{
	return m_words;
}

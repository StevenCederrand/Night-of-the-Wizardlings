#include <Pch/Pch.h>
#include "Metafile.h"

Metafile::Metafile()
{
	m_aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
}

bool Metafile::Load(std::string fontFile)
{
	if (openFile(fontFile) == false)
		return false;

	loadPaddingData();
	loadLineSizes();
	int imageSize = getValueOfVariable("scaleW");
	loadCharacterData(imageSize);
	
	m_fstream.close();
	return true;;
}

bool Metafile::openFile(std::string fontFile)
{
	m_fstream.open(fontFile);
	return m_fstream.is_open();
}

void Metafile::loadPaddingData()
{
	processNextLine();
	auto parts = split(m_parsedRow["padding"], NUMBER_SEPERATOR);

	if (parts.size() == 4) {
		for (size_t i = 0; i < parts.size(); i++) {
			m_padding[i] = std::stoi(parts[i]);
		}

		

		m_paddingWidth = m_padding[PAD_LEFT] + m_padding[PAD_RIGHT];
		m_paddingHeight = m_padding[PAD_TOP] + m_padding[PAD_BOTTOM];

	}
	else
	{
		logError("Metafile could not find 4 values for padding!");
	}

}

void Metafile::loadLineSizes()
{
	processNextLine();
	int lineHeightPixels = getValueOfVariable("lineHeight") - m_paddingHeight;
	m_verticalPerPixelSize = LINE_HEIGHT / lineHeightPixels;
	m_horizontalPerPixelSize = m_verticalPerPixelSize / m_aspectRatio;
}

bool Metafile::processNextLine()
{
	m_parsedRow.clear();

	std::string line;
	if (!std::getline(m_fstream, line)) {
		return false;
	}

	std::vector<std::string> parts = split(line, SPLITTER);

	for (size_t i = 0; i < parts.size(); i++) {
		std::vector<std::string> valuePairs = split(parts[i], VARIABLE_SPLITTER);
		if (valuePairs.size() == 2) {
			m_parsedRow[valuePairs[0]] = valuePairs[1];
		}
	}

	return true;
}

void Metafile::loadCharacterData(int imageSize)
{
	processNextLine();
	processNextLine();

	int count = getValueOfVariable("count");

	for (int i = 0; i < count; i++) {
		processNextLine();
		Character c = loadCharacter(imageSize);
		if (c.id != -1) {
			m_metadata[c.id] = c;
		}
	}

}

std::vector<std::string> Metafile::split(const std::string& line, const char splitter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(line);

	while (std::getline(tokenStream, token, splitter))
	{
		tokens.emplace_back(token);
	}

	return tokens;
}

int Metafile::getValueOfVariable(std::string variable)
{
	return std::stoi(m_parsedRow[variable]);	
}

Character Metafile::loadCharacter(int imageSize)
{
	Character c;
	int id = getValueOfVariable("id");
	if (id == SPACE_ASCII) {
		m_spaceWidth = (getValueOfVariable("xadvance") - m_paddingWidth) * m_horizontalPerPixelSize;
		return c;
	}

	float xTex = (float)(getValueOfVariable("x") + (m_padding[PAD_LEFT] - DESIRED_PADDING)) / (float)(imageSize);
	float yTex = (float)(getValueOfVariable("y") + (m_padding[PAD_TOP] - DESIRED_PADDING)) / (float)(imageSize);
	int width = getValueOfVariable("width") - (m_paddingWidth - (2 * DESIRED_PADDING));
	int height = getValueOfVariable("height") - (m_paddingHeight - (2 * DESIRED_PADDING));
	float quadWidth = width * m_horizontalPerPixelSize;
	float quadHeight = height * m_verticalPerPixelSize;
	float xTexSize = (float)width / imageSize;
	float yTexSize = (float)height / imageSize;
	float xOff = (getValueOfVariable("xoffset") + m_padding[PAD_LEFT] - DESIRED_PADDING) * m_horizontalPerPixelSize;
	float yOff = (getValueOfVariable("yoffset") + (m_padding[PAD_TOP] - DESIRED_PADDING)) * m_verticalPerPixelSize;
	float xAdvance = (getValueOfVariable("xadvance") - m_paddingWidth) * m_horizontalPerPixelSize;

	c.id = id;
	c.textureCoord = glm::vec4(xTex, yTex, xTexSize, yTexSize);
	c.offset = glm::vec2(xOff, yOff);
	c.size = glm::vec2(width, height);
	c.xAdvance = xAdvance;

	return c;
}

const std::map<int, Character>& Metafile::getMetaData() const
{
	return m_metadata;
}

const Character& Metafile::getCharacter(int ascii)
{
	return m_metadata[ascii];
}

const float& Metafile::getVerticalPerPixelSize() const
{
	return m_verticalPerPixelSize;
}

const float& Metafile::getHorizontalPerPixelSize() const
{
	return m_horizontalPerPixelSize;
}

const float& Metafile::getSpaceWidth() const
{
	return m_spaceWidth;
}

const int* Metafile::getPadding() const
{
	return m_padding;
}

const int& Metafile::getPaddingWidth() const
{
	return m_paddingWidth;
}

const int& Metafile::getPaddingHeight() const
{
	return m_paddingHeight;
}

const float& Metafile::getAspectRatio() const
{
	return m_aspectRatio;
}

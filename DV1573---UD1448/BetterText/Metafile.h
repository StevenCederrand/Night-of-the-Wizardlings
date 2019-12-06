#ifndef _METAFILE_H
#define _METAFILE_H
#include <Pch/Pch.h>
#include "Character.h"


/*	Array order info:
	0 = top
	1 = left
	2 = bottom
	3 = right
*/
constexpr int PAD_TOP = 0;
constexpr int PAD_LEFT = 1;
constexpr int PAD_BOTTOM = 2;
constexpr int PAD_RIGHT = 3;

class Metafile {

public:

	Metafile();
	bool Load(std::string fontFile);
	
	const std::map<int, Character>& getMetaData() const;
	const Character& getCharacter(int ascii) ;
	const float& getVerticalPerPixelSize() const;
	const float& getHorizontalPerPixelSize() const;
	const float& getSpaceWidth() const;
	const int* getPadding() const;
	const int& getPaddingWidth() const;
	const int& getPaddingHeight() const;
	const float& getAspectRatio() const;


private:
	bool openFile(std::string fontFile);
	void loadPaddingData();
	void loadLineSizes();
	bool processNextLine();
	void loadCharacterData(int imageSize);
	std::vector<std::string> split(const std::string& line, const char splitter);

	Character loadCharacter(int imageSize);
	int getValueOfVariable(std::string variable);

private:
	std::map<int, Character> m_metadata;
	std::map<std::string, std::string> m_parsedRow;

	float m_verticalPerPixelSize;
	float m_horizontalPerPixelSize;
	float m_spaceWidth;
	int m_padding[4];
	int m_paddingWidth;
	int m_paddingHeight;
	float m_aspectRatio;

	std::ifstream m_fstream;

	const char SPLITTER = ' ';
	const char NUMBER_SEPERATOR = ',';
	const char VARIABLE_SPLITTER = '=';
	
};


#endif


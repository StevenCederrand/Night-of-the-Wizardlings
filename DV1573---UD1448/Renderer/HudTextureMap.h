#ifndef _HUD_TEXTURE_MAP_H
#define _HUD_TEXTURE_MAP_H

class HudTextureMap {
public:
	HudTextureMap();
	~HudTextureMap();
	void destroy();

	static HudTextureMap* getInstance();
	void addTextureID(const std::string& key, const GLuint& textureID);
	const GLuint* getTextureID(const std::string& key) const;

private:
	std::unordered_map<std::string, GLuint> m_textureMap;



};


#endif

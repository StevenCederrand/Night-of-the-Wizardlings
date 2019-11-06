#include <Pch/Pch.h>
#include "Renderer.h"
#include <Networking/Client.h>

#define TILE_SIZE 16

Renderer* Renderer::m_rendererInstance = 0;

Renderer::Renderer()
{
	m_gWindow = nullptr;
	m_camera = nullptr;
	glEnable(GL_MULTISAMPLE);
	createDepthMap();
	int x = -10;
	int z = -40;

	//Define Work Groups
	workGroups.x = (SCREEN_WIDTH + (SCREEN_WIDTH % TILE_SIZE)) / TILE_SIZE;
	workGroups.y = (SCREEN_HEIGHT + (SCREEN_HEIGHT % TILE_SIZE)) / TILE_SIZE;
	//INIT SHADER_STORAGE_BUFFER_OBJECT 
	glGenBuffers(1, &m_lightIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightIndexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightIndex), 0, GL_STATIC_DRAW);
	//GL related calls
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

Renderer::~Renderer()
{
	//delete m_bloom;
	delete m_text;
}

void Renderer::renderHUD()
{
	glEnable(GL_BLEND);
	// Get it ONCE instead of every iteration....
	auto* shader = ShaderMap::getInstance()->getShader(HUD);
	shader->use();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	for (auto& item : m_2DHudMap) {

		auto& vec = item.second;
		
		if (vec.size() == 0)
			continue;
		
		auto* hudObjectDummy = vec[0];
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hudObjectDummy->getTextureID());

		for (size_t i = 0; i < vec.size(); i++)
		{

			auto* hudObject = vec[i];

			if (hudObject->getAlpha() == 0.0f)
				continue;

			shader->setMat4("modelMatrix", hudObject->getModelMatrix());
			shader->setFloat("alphaValue", hudObject->getAlpha());
			shader->setVec2("clip", glm::vec2(hudObject->getXClip(), hudObject->getYClip()));
			//shader->setVec3("fillColor", hudObject->getFillColor());
			glBindVertexArray(hudObject->getVAO());

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glBindVertexArray(0);
		}

		glBindTexture(GL_TEXTURE_2D, NULL);
	}
}

void Renderer::createDepthMap() {

	//Create a depth map texture for the rendering system
	glGenFramebuffers(1, &m_depthFBO);

	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1, 1, 1, 1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Inits the shaders for forward+
	initShaders();
}

void Renderer::initShaders() {
	ShaderMap::getInstance()->createShader(DEPTH_MAP, "Depth.vert", "Depth.frag");
	//Set the light index binding
	ShaderMap::getInstance()->createShader(LIGHT_CULL, "LightCullCompute.comp");
	ShaderMap::getInstance()->useByName(LIGHT_CULL);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
	ShaderMap::getInstance()->createShader(BASIC_FORWARD, "VertexShader.vert", "FragShader.frag");
	ShaderMap::getInstance()->createShader(ANIMATION, "Animation.vert", "FragShader.frag");
	ShaderMap::getInstance()->createShader("Skybox_Shader", "Skybox.vs", "Skybox.fs");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setInt("skyBox", 4);
	ShaderMap::getInstance()->createShader(DEBUG, "VertexShader.vert", "DebugFragShader.frag");

	/*=====================================================*/
	/*ShaderMap::getInstance()->createShader(BLOOM, "Bloom.vs", "Bloom.fs");
	ShaderMap::getInstance()->useByName(BLOOM);
	ShaderMap::getInstance()->getShader(BLOOM)->setInt("albedoTexture", 0);

	ShaderMap::getInstance()->createShader(BLUR, "Blur.vs", "Blur.fs");
	ShaderMap::getInstance()->useByName(BLUR);
	ShaderMap::getInstance()->getShader(BLUR)->setInt("brightImage", 0);

	ShaderMap::getInstance()->createShader(BLOOM_BLUR, "BloomBlur.vs", "BloomBlur.fs");
	ShaderMap::getInstance()->useByName(BLOOM_BLUR);
	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("sceneImage", 0);
	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("bloomImage", 1);*/
	m_text = new FreeType();
	m_text->BindTexture();
	//m_bloom = new BloomBlur;
	//m_bloom->createHdrFBO();
	//m_bloom->createPingPingFBO();
	/*=====================================================*/

	/* Hud */
	auto* shader = ShaderMap::getInstance()->createShader(HUD, "HUD.vs", "HUD.fs");
	shader->use();
	shader->setInt("textureSampler", 0);
	
}

void Renderer::bindMatrixes(const std::string& shaderName) {
	ShaderMap::getInstance()->getShader(shaderName)->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader(shaderName)->setMat4("projMatrix", m_camera->getProjMat());
}

void Renderer::bindMatrixes(Shader* shader)
{
	shader->setMat4("viewMatrix", m_camera->getViewMat());
	shader->setMat4("projMatrix", m_camera->getProjMat());
}

Renderer* Renderer::getInstance()
{
	if (m_rendererInstance == 0) {
		m_rendererInstance = new Renderer();
	}
	return m_rendererInstance;
}

void Renderer::init(GLFWwindow* window)
{
	m_gWindow = window;
}

void Renderer::setupCamera(Camera* camera)
{
	if (camera == nullptr) {
		return;
	}
	m_camera = camera;
}

void Renderer::submit(GameObject* gameObject, ObjectType objType)
{
	if (objType == STATIC) {
		m_staticObjects.emplace_back(gameObject);
	}
	else if (objType == SPELL) {
		m_spells.emplace_back(gameObject);
	}
	else if (objType == DYNAMIC) {
		m_dynamicObjects.emplace_back(gameObject);
	}
	else if (objType == ANIMATEDSTATIC) {
		m_anistaticObjects.emplace_back(gameObject);
	}
	else if (objType == ANIMATEDDYNAMIC) {
		m_anidynamicObjects.emplace_back(gameObject);
	}
	
}

void Renderer::submit2DHUD(HudObject* hud)
{
	auto item = m_2DHudMap.find(hud->getTextureID());

	if (item != m_2DHudMap.end()) {
		auto& vec = item._Ptr->_Myval.second;

		vec.emplace_back(hud);
		logTrace("Placed a hud object in an existing vector");
		return;
	} else
	{
		std::vector<HudObject*> newVec;
		newVec.reserve(5);
		newVec.emplace_back(hud);
		m_2DHudMap[hud->getTextureID()] = newVec;
		logTrace("Made a new vector for an hud object");
	}
}

void Renderer::clear() {
	
	m_staticObjects.clear();
	m_dynamicObjects.clear();
	m_anistaticObjects.clear();
	m_anidynamicObjects.clear();
	m_spells.clear();
	m_2DHudMap.clear();

}

void Renderer::removeDynamic(GameObject* gameObject, ObjectType objType)
{
	int index = -1;
	
	if (objType == DYNAMIC) { //Remove dynamic objet from the dynamic objet vector
		//Find the index of the object
		for (size_t i = 0; i < m_dynamicObjects.size(); i++)
		{
			if (m_dynamicObjects[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			m_dynamicObjects.erase(m_dynamicObjects.begin() + index);
		}
	}
	else if (objType == SPELL) { //remove spells from the spell vector!!
		//Find the index of the object
		for (size_t i = 0; i < m_spells.size(); i++)
		{
			if (m_spells[i] == gameObject) {
				index = i;
				break;
			}
		}
		if (index > -1) {
			m_spells.erase(m_spells.begin() + index);
		}
	}
}

void Renderer::destroy()
{
	delete m_rendererInstance;
}

void Renderer::renderSkybox(SkyBox* m_skybox)
{
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	auto* shader = ShaderMap::getInstance()->useByName("Skybox_Shader");
	shader->setMat4("modelMatrix", m_skybox->getModelMatrix());
	shader->setMat4("viewMatrix", glm::mat4(glm::mat3(m_camera->getViewMat())));
	shader->setMat4("projectionMatrix", m_camera->getProjMat());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox->getCubeMapTexture());
	glBindVertexArray(m_skybox->getVAO());
	glDrawArrays(GL_TRIANGLES, 0, 36); //Maybe index the skybox?
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void Renderer::render(SkyBox* m_skybox, SpellHandler* m_spellHandler) {
	Mesh* mesh;
	Transform transform;
	glm::mat4 modelMatrix;
	Shader* shader;
	/* It is better to get the singletons directly rather than having to for every new thing, get them. BRANCHING IS A PROBLEM*/
	MeshMap* meshMap = MeshMap::getInstance();
	ShaderMap* shaderMap = ShaderMap::getInstance();

#pragma region Depth_Render & Light_Cull
	if (m_spells.size() > 0) {
		shader = shaderMap->useByName(DEPTH_MAP);

		//Bind and draw the objects to the depth-buffer
		bindMatrixes(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);

		//Loop through all of the gameobjects
		for (GameObject* object : m_staticObjects)
		{
			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				modelMatrix = glm::mat4(1.0f);
				//Fetch the current mesh and its transform
				mesh = meshMap->getMesh(object->getMeshName(j));
				transform = object->getTransform(mesh, j);

				modelMatrix = object->getMatrix(j);

				glBindVertexArray(mesh->getBuffers().vao);

				//Bind the modelmatrix
				shader->setMat4("modelMatrix", modelMatrix);

				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
		}

		//Animated static objects
		//TODO: Consider animation for the depth shader
		for (GameObject* object : m_anistaticObjects)
		{
			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				modelMatrix = glm::mat4(1.0f);
				//Fetch the current mesh and its transform
				mesh = meshMap->getMesh(object->getMeshName(j));
				transform = object->getTransform(mesh, j);

				modelMatrix = object->getMatrix(j);

				glBindVertexArray(mesh->getBuffers().vao);

				//Bind the modelmatrix
				shader->setMat4("modelMatrix", modelMatrix);

				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
			   
#pragma region Light_Culling
		shader = shaderMap->useByName(LIGHT_CULL);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
		bindMatrixes(shader);

		glm::vec2 screenSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
		shader->setVec2("screenSize", screenSize);
		shader->setInt("lightCount", m_spells.size());//Set the number of active pointlights in the scene 

		//Bind the depthmap	
		glActiveTexture(GL_TEXTURE0);
		shader->setInt("depthMap", 0); //Not sure if this has to happen every frame
		glBindTexture(GL_TEXTURE_2D, m_depthMap);

		//Send all of the light data into the compute shader	
		for (size_t i = 0; i < m_spells.size(); i++) {
			shader->setVec3("lights[" + std::to_string(i) + "].position", m_spells[i]->getTransform().position);
			shader->setFloat("lights[" + std::to_string(i) + "].radius", P_LIGHT_RADIUS);
		}

		glDispatchCompute(workGroups.x, workGroups.y, 1);
		//Unbind the depth
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion
	}

#pragma endregion


	//BLOOMBLUR MISSION STEP 1: SAMPLE
	//m_bloom->bindHdrFBO();
	renderSkybox(m_skybox);
	m_spellHandler->renderSpell();

#pragma region Color_Render
	shader = shaderMap->useByName(BASIC_FORWARD);

	if (Client::getInstance()->getMyData().health <= 0) {
		shader->setInt("grayscale", 1);
	}
	else {
		shader->setInt("grayscale", 0);
	}

	//Bind view- and projection matrix
	bindMatrixes(shader);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
	shader->setVec3("CameraPosition", m_camera->getCamPos());
	//Add a step where we insert lights into the scene
	shader->setInt("LightCount", m_spells.size());

	if (m_spells.size() > 0) {
		for (size_t i = 0; i < m_spells.size(); i++) {
			shader->setVec3("pLights[" + std::to_string(i) + "].position", m_spells[i]->getTransform().position);
			if (m_spells[i]->getType() == NORMALATTACK) {
				shader->setVec3("pLights[" + std::to_string(i) + "].color", m_spellHandler->getAttackBase()->m_material->diffuse);
			}
			else if (m_spells[i]->getType() == ENHANCEATTACK) {
				shader->setVec3("pLights[" + std::to_string(i) + "].color", m_spellHandler->getEnhAttackBase()->m_material->diffuse);
			}
			shader->setFloat("pLights[" + std::to_string(i) + "].radius", P_LIGHT_RADIUS);
		}
	}
	//Render Static objects
	for (GameObject* object : m_staticObjects)
	{
		//Then through all of the meshes
		for (int j = 0; j < object->getMeshesCount(); j++)
		{
			//Fetch the current mesh and its transform
			mesh = meshMap->getMesh(object->getMeshName(j));

			//Bind the material
			object->bindMaterialToShader(shader, mesh->getMaterial());

			modelMatrix = glm::mat4(1.0f);

			modelMatrix = object->getMatrix(j);
			//Bind the modelmatrix
			shader->setMat4("modelMatrix", modelMatrix);

			glBindVertexArray(mesh->getBuffers().vao);

			glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
		}
	}

	//Dynamic objects
	if (m_dynamicObjects.size() > 0) {
		for (GameObject* object : m_dynamicObjects)
		{
			if (object == nullptr || !object->getShouldRender()) {
				continue;
			}

			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				//Fetch the current mesh and its transform
				mesh = meshMap->getMesh(object->getMeshName(j));
				//Bind the material
				object->bindMaterialToShader(shader, mesh->getMaterial());

				modelMatrix = glm::mat4(1.0f);
				//Apply the transform to the matrix. This should actually be done automatically in the mesh!
				modelMatrix = object->getMatrix(j);

				//Bind the modelmatrix
				shader->setMat4("modelMatrix", modelMatrix);

				glBindVertexArray(mesh->getBuffers().vao);

				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
		}
	}
#pragma endregion


#pragma region Animation_Render
	//TODO: Evaluate this implementation, should be an easier way to bind values to shaders as they're changed
	// Possibly extract functions. Only difference in rendering is the shader and the binding of bone matrices
	if (m_anistaticObjects.size() > 0) {
		shaderMap->useByName(ANIMATION);
		//Bind view- and projection matrix
		bindMatrixes(ANIMATION);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);

		//Add a step where we insert lights into the scene
		shaderMap->getShader(BASIC_FORWARD)->setInt("LightCount", m_spells.size());
		if (m_spells.size() > 0) {
			for (size_t i = 0; i < m_spells.size(); i++) {
				ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setVec3("pLights[" + std::to_string(i) + "].position", m_spells[i]->getTransform().position);
				ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setVec3("pLights[" + std::to_string(i) + "].attenuation", glm::vec3(1.0f, 0.09f, 0.032f));
				ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setFloat("pLights[" + std::to_string(i) + "].radius", P_LIGHT_RADIUS);
			}
		}
		for (GameObject* object : m_anistaticObjects)
		{
			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				//Fetch the current mesh and its transform
				mesh = MeshMap::getInstance()->getMesh(object->getMeshName(j));
				//Bind calculated bone matrices
				static_cast<AnimatedObject*>(object)->BindAnimation(j);
				transform = object->getTransform(mesh, j);

				//Bind the material
				object->bindMaterialToShader(ANIMATION, j);

				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, transform.position);
				modelMatrix = glm::scale(modelMatrix, transform.scale);
				modelMatrix *= glm::mat4_cast(transform.rotation);

				//Bind the modelmatrix
				ShaderMap::getInstance()->getShader(ANIMATION)->setMat4("modelMatrix", modelMatrix);

				glBindVertexArray(mesh->getBuffers().vao);

				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
		}
	}

#pragma endregion

	//ShaderMap::getInstance()->useByName(BLUR);

	//ShaderMap::getInstance()->getShader(BLUR)->setInt("horizontal", m_bloom->getHorizontal() ? 1 : 0);
	//m_bloom->blurIteration(0);


	/*for (unsigned int i = 0; i < m_bloom->getAmount() - 1; i++)
	{

		ShaderMap::getInstance()->getShader(BLUR)->setInt("horizontal", m_bloom->getHorizontal() ? 1 : 0);

		m_bloom->blurIteration(1);
	}
	m_bloom->unbindTextures();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ShaderMap::getInstance()->useByName(BLOOM_BLUR);*/
	//If the client is dead
	
	/*if (Client::getInstance()->getMyData().health <= 0) {
		ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("grayscale", 1);
	}
	else {
		ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("grayscale", 0);
	}*/


	//m_bloom->sendTextureLastPass();
	//m_bloom->renderQuad();
	//m_bloom->unbindTextures();
	
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	if (Client::getInstance()->isConnectedToSever()) {
		
		NetGlobals::SERVER_STATE state = Client::getInstance()->getServerState().currentState;

		if (state == NetGlobals::SERVER_STATE::GAME_IS_STARTING) {
			std::string timeText = std::to_string(Client::getInstance()->getCountdownPacket().timeLeft / 1000);
			m_text->RenderText("Game starts in: " + timeText, (SCREEN_WIDTH / 2) - 125.0f , 680.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
		}
		else if (state == NetGlobals::SERVER_STATE::GAME_IN_SESSION) {
			
			uint32_t minutes = Client::getInstance()->getRoundTimePacket().minutes;
			uint32_t seconds = Client::getInstance()->getRoundTimePacket().seconds;
			std::string timeText = std::to_string(minutes) +":";

			if (seconds >= 10) {
				timeText += std::to_string(seconds);
			}
				
			else{
				timeText += "0" + std::to_string(seconds);
			}
				
			
			//std::string timeText = std::to_string(Client::getInstance()->getRoundTimePacket().timeLeft / 1000);
			m_text->RenderText("Game time " + timeText, (SCREEN_WIDTH / 2) - 125.0f, 680.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
		}
		else if (state == NetGlobals::SERVER_STATE::WAITING_FOR_PLAYERS) {
			std::string timeText = std::to_string(Client::getInstance()->getCountdownPacket().timeLeft / 1000);
			m_text->RenderText("Warmup", SCREEN_WIDTH / 2 - 80.0f, 680.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

			if (Client::getInstance()->isServerOwner()) {
				m_text->RenderText("Press \"E\" to start the game", 10.0f, 620.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f));
			}
			
		}
		else if (state == NetGlobals::SERVER_STATE::GAME_END_STATE) {
			uint32_t minutes = Client::getInstance()->getRoundTimePacket().minutes;
			uint32_t seconds = Client::getInstance()->getRoundTimePacket().seconds;
			std::string timeText = std::to_string(minutes) + ":";

			if (seconds >= 10) {
				timeText += std::to_string(seconds);
			}

			else {
				timeText += "0" + std::to_string(seconds);
			}
			m_text->RenderText("End of round: " + timeText, SCREEN_WIDTH / 2 - 135.0f, 680.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		
		if (Client::getInstance()->getMyData().health == 0) {
			std::string timeText = std::to_string(Client::getInstance()->getRespawnTime().timeLeft / 1000);
			m_text->RenderText("Respawn in " + timeText + " seconds", (SCREEN_WIDTH / 2) - 200.0f, 480.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		m_text->RenderText("Health: " + std::to_string(Client::getInstance()->getMyData().health), 10.0f, 680.0f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));
		
		if(state == NetGlobals::SERVER_STATE::GAME_IN_SESSION)
			m_text->RenderText("Kills: " + std::to_string(Client::getInstance()->getMyData().numberOfKills), 1000.0f, 680.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));


	}
	renderHUD();
}


void Renderer::renderSpell(SpellHandler* spellHandler)
{

	for (int i = 0; i < m_spells.size(); i++)
	{
		if (m_spells[i]->getType() == NORMALATTACK)
		{
			Mesh* meshRef = spellHandler->getAttackBase()->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMaterial(spellHandler->getAttackBase()->m_material);

			// TODO: Fix below
			const Transform meshTransform = m_spells[i]->getTransform();
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, meshTransform.position);
			modelMatrix = glm::scale(modelMatrix, meshTransform.scale);
			modelMatrix *= glm::mat4_cast(meshTransform.rotation);
			bindMatrixes(BASIC_FORWARD);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMat4("modelMatrix", modelMatrix);

			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
		}

		else if (m_spells[i]->getType() == ENHANCEATTACK)
		{
			Mesh* meshRef = spellHandler->getEnhAttackBase()->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMaterial(spellHandler->getEnhAttackBase()->m_material);

			// TODO: Fix below
			const Transform meshTransform = m_spells[i]->getTransform();
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, meshTransform.position);
			modelMatrix = glm::scale(modelMatrix, meshTransform.scale);
			modelMatrix *= glm::mat4_cast(meshTransform.rotation);
			bindMatrixes(BASIC_FORWARD);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMat4("modelMatrix", modelMatrix);

			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
		}

		else if (m_spells[i]->getType() == REFLECT)
		{
			Mesh* meshRef = spellHandler->getReflectBase()->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMaterial(spellHandler->getReflectBase()->m_material);

			// TODO: Fix below
			const Transform meshTransform = m_spells[i]->getTransform();
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, meshTransform.position);
			modelMatrix = glm::scale(modelMatrix, meshTransform.scale);
			modelMatrix *= glm::mat4_cast(meshTransform.rotation);
			bindMatrixes(BASIC_FORWARD);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMat4("modelMatrix", modelMatrix);

			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
		}

		else if (m_spells[i]->getType() == FLAMESTRIKE)
		{
			Mesh* meshRef = spellHandler->getAttackBase()->m_mesh;
			glBindVertexArray(meshRef->getBuffers().vao);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMaterial(spellHandler->getAttackBase()->m_material);

			// TODO: Fix below
			const Transform meshTransform = m_spells[i]->getTransform();
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, meshTransform.position);
			modelMatrix = glm::scale(modelMatrix, meshTransform.scale);
			modelMatrix *= glm::mat4_cast(meshTransform.rotation);
			bindMatrixes(BASIC_FORWARD);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMat4("modelMatrix", modelMatrix);

			glDrawElements(GL_TRIANGLES, meshRef->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);
		}

	}
}

void Renderer::renderDebug()
{
	glm::mat4 modelMatrix;
	ShaderMap::getInstance()->useByName(DEBUG);
	//Bind view- and projection matrix
	bindMatrixes(DEBUG);	
	
	//Render Static objects
	for (size_t i = 0; i < m_staticObjects.size(); i++)
	{		
		for (size_t j = 0; j < m_staticObjects.at(i)->getDebugDrawers().size(); j++)
		{			
			modelMatrix = glm::mat4(1.0f);
			//Bind the modelmatrix
			//modelMatrix = m_staticObjects.at(i)->getMatrix(j);
			ShaderMap::getInstance()->getShader(DEBUG)->setMat4("modelMatrix", modelMatrix);

			glBindVertexArray(m_staticObjects.at(i)->getDebugDrawers()[j]->getBuffers().vao);

			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glDrawElements(GL_TRIANGLES, m_staticObjects.at(i)->getDebugDrawers()[j]->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(0);
		}
	}
}

Camera* Renderer::getMainCamera() const
{
	return m_camera;
}

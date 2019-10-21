#include <Pch/Pch.h>
#include "Renderer.h"

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

	//HDR and a sperate colour buffer
	glGenFramebuffers(1, &m_hdrFbo);

	glGenTextures(1, &m_colourBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colourBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	ShaderMap::getInstance()->createShader(BLOOM, "Bloom.vs", "Bloom.fs");
	ShaderMap::getInstance()->useByName(BLOOM);
	ShaderMap::getInstance()->getShader(BLOOM)->setInt("albedoTexture", 0);

	ShaderMap::getInstance()->createShader(BLUR, "Blur.vs", "Blur.fs");
	ShaderMap::getInstance()->useByName(BLUR);
	ShaderMap::getInstance()->getShader(BLUR)->setInt("brightImage", 0);

	ShaderMap::getInstance()->createShader(BLOOM_BLUR, "BloomBlur.vs", "BloomBlur.fs");
	ShaderMap::getInstance()->useByName(BLOOM_BLUR);
	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("bloomImage", 0);
	ShaderMap::getInstance()->getShader(BLOOM_BLUR)->setInt("sceneImage", 1);

	m_bloom = new BloomBlur;
	m_bloom->createHdrFBO();
	m_bloom->createPingPingFBO();
	/*=====================================================*/


}

void Renderer::bindMatrixes(const std::string& shaderName) {
	ShaderMap::getInstance()->getShader(shaderName)->setMat4("viewMatrix", m_camera->getViewMat());
	ShaderMap::getInstance()->getShader(shaderName)->setMat4("projMatrix", m_camera->getProjMat());
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
			logWarning(m_spells.size());
			m_spells.erase(m_spells.begin() + index);
			logWarning(m_spells.size());
		}
		logWarning("index", index);
	}

	
}

void Renderer::destroy()
{
	delete m_rendererInstance;
}

void Renderer::renderSkybox(const SkyBox& skybox)
{
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	//glDepthMask(false);
	ShaderMap::getInstance()->useByName("Skybox_Shader");
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setMat4("viewMatrix", glm::mat4(glm::mat3(m_camera->getViewMat())));
	ShaderMap::getInstance()->getShader("Skybox_Shader")->setMat4("projectionMatrix", m_camera->getProjMat());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubeMapTexture());
	glBindVertexArray(skybox.getVAO());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	//glDepthMask(true);
	glEnable(GL_CULL_FACE);
}

void Renderer::update(float dt) {
	m_camera->fpsControls(dt);
	m_camera->update(m_gWindow);
}

void Renderer::render() {
	Mesh* mesh;
	Transform transform;
	glm::mat4 modelMatrix;
	
#pragma region Depth_Render & Light_Cull
	if (m_spells.size() > 0) {
		ShaderMap::getInstance()->useByName(DEPTH_MAP);

		//Bind and draw the objects to the depth-buffer
		bindMatrixes(DEPTH_MAP);
		glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//Loop through all of the gameobjects
		for (GameObject* object : m_staticObjects)
		{
			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				modelMatrix = glm::mat4(1.0f);
				//Fetch the current mesh and its transform
				mesh = MeshMap::getInstance()->getMesh(object->getMeshName(j));
				transform = object->getTransform(j);

				modelMatrix = object->getMatrix(j);

				glBindVertexArray(mesh->getBuffers().vao);

				//Bind the modelmatrix
				ShaderMap::getInstance()->getShader(DEPTH_MAP)->setMat4("modelMatrix", modelMatrix);

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
				mesh = MeshMap::getInstance()->getMesh(object->getMeshName(j));
				transform = object->getTransform(j);

				modelMatrix = object->getMatrix(j);

				glBindVertexArray(mesh->getBuffers().vao);

				//Bind the modelmatrix
				ShaderMap::getInstance()->getShader(DEPTH_MAP)->setMat4("modelMatrix", modelMatrix);

				glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		#pragma region Light_Culling
		ShaderMap::getInstance()->useByName(LIGHT_CULL);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
		bindMatrixes(LIGHT_CULL);

		glm::vec2 screenSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
		ShaderMap::getInstance()->getShader(LIGHT_CULL)->setVec2("screenSize", screenSize);
		ShaderMap::getInstance()->getShader(LIGHT_CULL)->setInt("lightCount", m_spells.size());//Set the number of active pointlights in the scene 


		//Bind the depthmap	
		glActiveTexture(GL_TEXTURE0);
		ShaderMap::getInstance()->getShader(LIGHT_CULL)->setInt("depthMap", 0); //Not sure if this has to happen every frame
		glBindTexture(GL_TEXTURE_2D, m_depthMap);


		//Send all of the light data into the compute shader	
		for (size_t i = 0; i < m_spells.size(); i++) {
			ShaderMap::getInstance()->getShader(LIGHT_CULL)->setVec3("lights[" + std::to_string(i) + "].position", m_spells[i]->getTransform().position);
			ShaderMap::getInstance()->getShader(LIGHT_CULL)->setFloat("lights[" + std::to_string(i) + "].radius", P_LIGHT_RADIUS);
		}

		glDispatchCompute(workGroups.x, workGroups.y, 1);
		//Unbind the depth
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		#pragma endregion
	}

#pragma endregion
	
	//BLOOMBLUR MISSION STEP 1: SAMPLE
	m_bloom->bindHdrFBO();
#pragma region Color_Render
	ShaderMap::getInstance()->useByName(BASIC_FORWARD);
	//Bind view- and projection matrix
	bindMatrixes(BASIC_FORWARD);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);
	
	//Add a step where we insert lights into the scene
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("LightCount", m_spells.size());
	if (m_spells.size() > 0) {
		for (size_t i = 0; i < m_spells.size(); i++) {
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setVec3("pLights[" + std::to_string(i) + "].position", m_spells[i]->getTransform().position);
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setVec3("pLights[" + std::to_string(i) + "].attenuation", glm::vec3(1.0f, 0.09f, 0.032f));
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setFloat("pLights[" + std::to_string(i) + "].radius", P_LIGHT_RADIUS);
		}
	}
	//Render Static objects
	for (GameObject* object : m_staticObjects)
	{
		//Then through all of the meshes
		for (int j = 0; j < object->getMeshesCount(); j++)
		{
			//Fetch the current mesh and its transform
			mesh = MeshMap::getInstance()->getMesh(object->getMeshName(j));

			//Bind the material
			object->bindMaterialToShader(BASIC_FORWARD, j);
			modelMatrix = glm::mat4(1.0f);

			modelMatrix = object->getMatrix(j);
			//Bind the modelmatrix
			ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMat4("modelMatrix", modelMatrix);
			
			glBindVertexArray(mesh->getBuffers().vao);

			glDrawElements(GL_TRIANGLES, mesh->getBuffers().nrOfFaces * 3, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
		}
	}

	//Dynamic objects
	if (m_dynamicObjects.size() > 0) {
		for (GameObject* object : m_dynamicObjects)
		{
			
			if (object == nullptr) {
				continue;
			}
			
			//Then through all of the meshes
			for (int j = 0; j < object->getMeshesCount(); j++)
			{
				//Fetch the current mesh and its transform
				mesh = MeshMap::getInstance()->getMesh(object->getMeshName(j));
				//Bind the material
				object->bindMaterialToShader(BASIC_FORWARD, j);
				
				modelMatrix = glm::mat4(1.0f);
				//Apply the transform to the matrix. This should actually be done automatically in the mesh!
				modelMatrix = object->getMatrix(j);

				//Bind the modelmatrix
				ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMat4("modelMatrix", modelMatrix);

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
		ShaderMap::getInstance()->useByName(ANIMATION);
		//Bind view- and projection matrix
		bindMatrixes(ANIMATION);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightIndexSSBO);

		//Add a step where we insert lights into the scene
		ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("LightCount", m_spells.size());
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
			for (size_t j = 0; j < object->getMeshesCount(); j++)
			{
				//Fetch the current mesh and its transform
				mesh = MeshMap::getInstance()->getMesh(object->getMeshName(j));
				//Bind calculated bone matrices
				static_cast<AnimatedObject*>(object)->BindAnimation(j);
				transform = object->getTransform(j);

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

	// MISSION COMPLEATE = OUTPUT 2 TEXTURES

	
	//BLOOMBLUR MISSION STEP 2: BLUR TEXTURE
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ShaderMap::getInstance()->useByName("Blur_Shader");

	ShaderMap::getInstance()->getShader("Blur_Shader")->setInt("horizontal", m_bloom->getHorizontal() ? 1 : 0);
	m_bloom->blurIteration(0);

	for (unsigned int i = 0; i < m_bloom->getAmount() - 1; i++)
	{

		ShaderMap::getInstance()->getShader("Blur_Shader")->setInt("horizontal", m_bloom->getHorizontal() ? 1 : 0);

		m_bloom->blurIteration(1);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//BLOOMBLUR MISSION 3: COMBINE TEXTURES

	ShaderMap::getInstance()->useByName(BLOOM_BLUR);
	m_bloom->sendTextureLastPass();

	m_bloom->renderQuad();
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}


void Renderer::renderSpell(const AttackSpellBase* spellBase) 
{

	Mesh* meshRef = spellBase->m_mesh;
	glBindVertexArray(meshRef->getBuffers().vao);
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setMaterial(spellBase->m_material);

	for (int i = 0; i < m_spells.size(); i++)
	{
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

void Renderer::renderDebug()
{
	glm::mat4 modelMatrix;
	ShaderMap::getInstance()->useByName(DEBUG);
	//Bind view- and projection matrix
	bindMatrixes(DEBUG);	
	
	//Render Static objects
	for (int i = 0; i < m_staticObjects.size(); i++)
	{		
		for (size_t j = 0; j < m_staticObjects.at(i)->getDebugDrawers().size(); j++)
		{			
			modelMatrix = glm::mat4(1.0f);
			//Bind the modelmatrix
			ShaderMap::getInstance()->getShader(DEBUG)->setMat4("modelMatrix", modelMatrix);
			//Then through all of the meshes

			modelMatrix = m_staticObjects.at(i)->getMatrix(j);
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

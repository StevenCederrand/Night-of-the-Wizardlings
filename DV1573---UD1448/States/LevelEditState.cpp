#include <Pch/Pch.h>
#include "LevelEditState.h"
#include <System/StateManager.h>
#include "MenuState.h"
#include <string>
#include <commdlg.h>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std::filesystem;

#define MESH_FILEPATH "C:/Users/timpa/source/repos/Impwing/DV1573---UD1448/Assets/Meshes/LevelEditMeshList"
#define NR_OF_MESHES 512
#define MAX_NR_LIGHTS 40

LevelEditState::LevelEditState()
{
	//Pick shader
	ShaderMap::getInstance()->getShader(BASIC_FORWARD)->setInt("albedoTexture", 0);

	//Create the scenes camera
	m_camera = new Camera(glm::vec3(57.f, 100.f, -78.f), -232.0f, -43.2);
	//Instanciate a mouse picker
	m_picker = new MousePicker(m_camera, m_camera->getProjMat());
	
	//Get the instance of the renderer
	Renderer* renderer = Renderer::getInstance();
	
	//Prepare the camera
	renderer->setupCamera(m_camera);

	//Load skybo
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	renderer->submitSkybox(m_skybox);
	
	//Check the file directory
	fileDirectoryUpdate();

	

	//the load map function might need tweaking in this regard
	//We also need a save map func.
	//loadMap();
}

LevelEditState::~LevelEditState()
{
	for (GameObject* object : m_objects)
		delete object;
	for (Pointlight* light : m_pointlights)
		delete light; 

	//Delete UI

	m_pointlights.clear();
	m_objects.clear();

	delete m_camera;
	delete m_skybox;
	delete m_picker;
	
	ImGui::DestroyContext();

	BulletPhysics::getInstance()->destroy();
	MeshMap::getInstance()->cleanUp();
}

std::string LevelEditState::OpenFileDialog(const char* filter = "All Files (*.*)\0*.mesh*\0", HWND owner = NULL)
{

	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";
	std::string fileNameStr;
	
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}

void LevelEditState::loadAsset(std::vector<GameObject*>& objectVector)
{
	//File dialog opens and returns a string that is desired filepath
	std::string filePath = OpenFileDialog();

	//Format the string to only be the name of the file
	std::size_t found = filePath.find_last_of("/\\");
	std::string editedName = filePath.substr(found + 1);

	//Debug
	std::cout << filePath << std::endl;
	std::cout << editedName << std::endl;
	
	//Remove everything from the dot onwards to get a name without file prefix
	std::size_t foundDot = editedName.find_first_of(".");
	std::cout << foundDot << std::endl;
	std::string fileType = editedName.substr(foundDot, editedName.length());
	std::cout << editedName << std::endl;
	
	if (fileType == ".mesh")
	{
		std::ifstream source(filePath, std::ios::binary);
		//This can't be hardcoded in the final product :P
		std::ofstream dest( MESH_FILEPATH + '/' + editedName,
			std::ios::binary);

		//file size
		source.seekg(0, std::ios::end);
		std::ifstream::pos_type size = source.tellg();
		source.seekg(0); 
		//allocate memory for buffer
		char* buffer = new char[size];

		//copy file
		source.read(buffer, size);
		dest.write(buffer, size);

		//Clean
		delete[] buffer;
		source.close();
		dest.close();
	}
	else
		std::cout << "This file is not a .mesh" << std::endl;
	
	//TODO:: Fetch info about texture, copy over to correct map.

	//Update list 
	fileDirectoryUpdate();
}

void LevelEditState::addInstance(std::vector<GameObject*> &objectVector, std::string filePath)
{
	Renderer* renderer = Renderer::getInstance();

	std::cout << "<<<<<" << filePath  << "<<<<<<<<" << std::endl;

	//Call fileformat with false param. to get file name,
	//call fileformat with true param. to get filePath.
	std::string editedName = fileNameFormat(filePath, false);
	std::string editedPath = fileNameFormat(filePath, true);

	std::cout << editedName << std::endl;
	std::cout << editedPath << std::endl;

	std::size_t foundDot = editedName.find_first_of(".");
	editedName = editedName.substr(0, foundDot);

	//Search list if there are objects with the same name 
	int count = 0;
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->getObjectName().find(editedName) == 0)
			count++;
	}
	
	if(count != 0)
		editedName = editedName + std::to_string(count);

	//CHECK IF FILE IS .MESH
	std::cout << filePath << std::endl;
	fileDirectoryUpdate();
	objectVector.push_back(new MapObject(editedName));
	objectVector[objectVector.size() - 1]->loadMesh(editedPath);
	objectName = objectVector[objectVector.size() - 1]->getObjectName();
	m_objectNames.push_back(objectName.c_str());
	renderer->submit(objectVector[objectVector.size() - 1], RENDER_TYPE::STATIC);

	//Update transform vector
	m_transforms.push_back(objectVector.at(objectVector.size() - 1)->getObjectTransform());

	std::cout << to_string(objectVector[0]->getLastPosition()) << std::endl;

}

void LevelEditState::createDuplicate(std::vector<GameObject*>& objectVector, int chosen)
{
	Renderer* renderer = Renderer::getInstance();

	std::cout << chosen << std::endl;

	std::string firstName = m_objects[chosen]->getObjectName();
	std::string object = firstName + ".mesh";
	//Search list if there are objects with the same name 

	firstName = firstName + "_Dup";

	objectVector.push_back(new MapObject(firstName));
	objectVector[objectVector.size() - 1]->loadMesh(object);
	objectName = objectVector[objectVector.size() - 1]->getObjectName();
	m_objectNames.push_back(objectName.c_str());
	renderer->submit(objectVector[objectVector.size() - 1], RENDER_TYPE::STATIC);

	//objectVector[chosen]->setTransform(glm::vec3(1.0), glm::vec3(1.0), glm::vec3(1.0));

}

void LevelEditState::deleteObject(std::vector<GameObject*>& objectVector, int chosen)
{
	objectVector.erase(objectVector.begin() + chosen);
	m_objectNames.erase(m_objectNames.begin() + chosen);
}
void LevelEditState::saveLevel()
{
	OpenFileDialog();

	std::string outputFilePath = "ExportLvl/";
	std::string name = "Lights";
	std::ofstream lightAscii;
	lightAscii.open(outputFilePath + name + ".txt");

	for (unsigned int i = 0; i < m_pointlights.size(); i++)
	{
		lightAscii << "Light nr. : " << i << std::endl;
		lightAscii << "==================" << std::endl;
		lightAscii << "Position : " << to_string(m_pointlights[i]->getLastPosition()) << std::endl;
		lightAscii << "Color : " << to_string(m_pointlights[i]->getColor()) << std::endl;
	}

	lightAscii << "I've now saved all lights!" << std::endl;
	lightAscii.close();

	std::ofstream meshAscii;
	name = "Level";
	meshAscii.open(outputFilePath + name + ".txt");

	meshAscii << "testing..." << std::endl;

	std::cout << m_objects.size() << std::endl;

	for (unsigned int i = 0; i < m_objects.size(); i++) 
	{
		meshAscii << "Mesh nr. : " << i << std::endl;
		meshAscii << "Mesh name : " << m_objects[i]->getObjectName() << std::endl;
		meshAscii << "==================" << std::endl;
		meshAscii << "Position : " << to_string(m_objects[i]->getLastPosition()) << std::endl;
	}
	meshAscii << "endend" << std::endl;

	meshAscii.close();
}

void LevelEditState::createPointLight()
{
	if (m_nrOfLight != MAX_NR_LIGHTS)
	{
		Renderer* renderer = Renderer::getInstance();

		m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3, 0.85, 1.0)));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 65.0f));

		for (Pointlight* p : m_pointlights)
			renderer->submit(p, RENDER_TYPE::POINTLIGHT_SOURCE);


		//Search list if there are objects with the same name 
		std::string lightName = "PointLight";
		int count = 0;
		for (int i = 0; i < m_LightsNames.size(); i++)
		{
			if (m_LightsNames[i].find(lightName) == 0)
				count++;
		}

		if (count != 0)
			lightName = lightName + std::to_string(count);

		m_LightsNames.push_back(lightName);
		m_nrOfLight++;
	}
}


void LevelEditState::cleanScene()
{
	for (GameObject* object : m_objects)
		delete object;
	for (Pointlight* light : m_pointlights)
		delete light;

	m_pointlights.clear();
	m_objects.clear();
	m_objectNames.clear();

	std::cout << "Scene cleaned." << std::endl;
}

void LevelEditState::update(float dt)
{
	updateState(dt);

	
	static float t = 0.0f;
	t += DeltaTime;
}

void LevelEditState::render()
{
	Renderer::getInstance()->render();
}

void LevelEditState::guiInfo()
{
	//This has to be moved to be class or be made vectors instead? variables to be accable by other imgui instances
	static int listBox_Meshes_Current = 0;

	const char* listBox_Particles[] = { "Fire", "Torch", "Flies", "Smoke", "Rain" };
	static int listBox_PointLight_Current = 1;
	static int listBox_ActiveMeshes_Current = -1;

	float* View;
	float* Proj;
	float* ID;
	float* gridSize;
	float* objectMat;

	

	glm::mat4 m_viewMat = m_camera->getViewMat();
	View = glm::value_ptr(m_viewMat);

	glm::mat4 m_projMat = m_camera->getProjMat();
	Proj = glm::value_ptr(m_projMat);

	glm::mat4 m_identity = glm::mat4(1.0f);
	ID = glm::value_ptr(m_identity);

	//IMGUIZMO
	ImGuizmo::DrawGrid(View, Proj, ID, 30.f);


	
	if (m_objects.size() != 0)
	{
		glm::mat4 m_objectMatrix = m_objects.at(listBox_ActiveMeshes_Current)->getMatrix();
		objectMat = glm::value_ptr(m_objectMatrix);
		EditTransform(View, Proj, objectMat);
	}

	float matrixT[3], matrixR[3], matrixS[3];

#pragma region SceneList
	//Lock ImGui window in place
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 240, 18));
	ImGui::SetNextWindowSize(ImVec2(240, SCREEN_HEIGHT - 18));
	ImGui::Begin("Scene");

	if (ImGui::BeginMainMenuBar())
	{
		//====================FILE-TAB=======================
		if (ImGui::BeginMenu("File"))
		{
			//NEW LEVEL BUTTON
			if (ImGui::MenuItem("New Level", "Ctrl+N"))
			{
				// Do Stuff
				saveLevel();
				cleanScene();
			}
			//OPEN LEVEL BUTTON
			if (ImGui::MenuItem("Open Level", "Ctrl+O"))
			{
				//addInstance(m_objects, OpenFileDialog());
			}
			//SAVE LEVEL BUTTON
			if (ImGui::MenuItem("Save Level", "Ctrl+S"))
			{
				saveLevel();
			}
			//EXIT EDITOR BUTTON
			if (ImGui::MenuItem("Exit", "Ctrl+Q"))
			{
				quitEditor();
			}
			ImGui::EndMenu();
		}
		//======================END=========================
		//====================EDIT-TAB=======================

		if (ImGui::BeginMenu("Edit"))
		{
			//DELETE MESH BUTTON
			if (ImGui::MenuItem("Delete"))
			{
				//if (listBox_ActiveMeshes_Current != -1)
					//deleteObject(m_objects, listBox_ActiveMeshes_Current);
			}
			//DUPLICATE BUTTON
			if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
			{
				if (listBox_ActiveMeshes_Current != -1)
					createDuplicate(m_objects, listBox_ActiveMeshes_Current);
			}

			ImGui::EndMenu();
		}
		//======================END=========================
		//====================CREATE-TAB=======================
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Create PointLight"))
			{
				createPointLight();
			}
			if (ImGui::MenuItem("Load Asset"))
			{
				// Do Stuff
				loadAsset(m_objects);

			}
			if (ImGui::MenuItem("Load Particles"))
			{
				// Do Stuff
			}
			ImGui::EndMenu();
		}
		//======================END========================= 
		ImGui::EndMainMenuBar();
	}

	if (m_objectNames.size() > 0)
	{
		ListBox("Mesh", &listBox_ActiveMeshes_Current, m_objectNames);
		ImGui::Separator();
	}

	if (m_LightsNames.size() > 0)
	{
		std::string amount = std::to_string(m_nrOfLight) + "/" + std::to_string(MAX_NR_LIGHTS) + " Lights";

		ImGui::Text(amount.c_str());
		static int listBox_ActiveLights_Current = 1;
		ListBox("Lights", &listBox_ActiveLights_Current, m_LightsNames);
		ImGui::Separator();
	}

	if (m_ParticlesNames.size() > 0) 
	{
		static int listBox_item_current3 = 1;
		ImGui::ListBox("Particles", &listBox_item_current3, &m_ParticlesNames[0], m_ParticlesNames.size(), 6);
	}
	
	ImGui::End();

#pragma endregion

#pragma region MeshList
	//Lock ImGui window in place
	ImGui::SetNextWindowPos(ImVec2(0, 18));
	ImGui::SetNextWindowSize(ImVec2(258, SCREEN_HEIGHT - 18));
	ImGui::Begin("Assets");
	
	ImGui::BeginGroup();
	if (ImGui::Button("Meshes", ImVec2(120, 25)))
	{
		assetTab = 0;	
	}
	ImGui::SameLine(0.0f, 1.0f);
	if (ImGui::Button("Particles", ImVec2(120, 25)))
	{
		assetTab = 1;
	}

	//The list items needs to represent actual meshes and such
	switch (assetTab)
	{
	case 0:
		ListBox("Meshes", &listBox_Meshes_Current, m_fileNames);
		break;
	/*case 1:
		ImGui::ListBox("Particles", &listBox_Particles_Current, listBox_Particles, IM_ARRAYSIZE(listBox_Particles), 6);
		break;*/
	default:
		break;
	}
	ImGui::EndGroup();

	ImGui::BeginGroup();
	if (ImGui::Button("Create", ImVec2(70, 25)))
	{
		addInstance(m_objects, m_files[listBox_Meshes_Current]);
		listBox_ActiveMeshes_Current = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("Refresh", ImVec2(70, 25)))
	{
		fileDirectoryUpdate();
	}
	ImGui::EndGroup();
	
	ImGui::Separator();

	ImGui::BeginGroup();

	if (Input::isKeyPressed(GLFW_KEY_W))
		changeAttrib = 1;
	ImGui::RadioButton("Translate", &changeAttrib, 1);
	ImGui::SameLine(0.0f, 10.0f);
	if (Input::isKeyPressed(GLFW_KEY_E))
		changeAttrib = 2;
	ImGui::RadioButton("Rotate", &changeAttrib, 2);
	ImGui::SameLine(0.0f, 10.0f);
	if (Input::isKeyPressed(GLFW_KEY_R))
		changeAttrib = 3;
	ImGui::RadioButton("Scale", &changeAttrib, 3);
	ImGui::Separator();

	
	
	ImGui::Text("Attribute");
	int index = listBox_ActiveMeshes_Current;
		
	//ImGui::ShowDemoWindow();

	if (m_objects.size() != 0)
	{
		
		ImGui::DragFloat3("Position", &m_transforms.at(index).position.x, 0.01f, -1000000, 1000000);
		ImGui::DragFloat3("Rotation", &m_transforms.at(index).rotation.x, 0.01f, -1000000, 1000000);

		float gSx[3] = { 1, 1, 1 };
		ImGui::DragFloat3("Scale", gSx, 0.01f, -1000000, 1000000);

		if (m_objects.size() != 0)
		{
			m_objects[index]->setTransform(
				//Position
				glm::vec3(m_transforms.at(index).position),
				//Rotation
				glm::quat(glm::vec3(m_transforms.at(index).rotation.x, m_transforms.at(index).rotation.y, m_transforms.at(index).rotation.z)),
				//Scale
				glm::vec3(1, 1, 1)
			);
		}
	}

	int indexL = listBox_PointLight_Current;

	if (m_pointlights.size() != 0)
	{
		static float f = 0.0f;

		//Transform position = m_pointlights[indexL]->getLocalTransform(indexL);

		//ImGui::DragFloat3("Light Position", &position.position.x, 0.01f, -1000000, 1000000);
		ImGui::SliderFloat("Attenuation", &f, 0.0f, 1.0f);

		glm::vec3 color = m_pointlights[0]->getColor();
		clear_Color = ImVec4(color.x, color.y, color.z, 1);
		ImGui::ColorEdit3("Color", (float*)&clear_Color);
		m_pointlights[0]->setColor(glm::vec3(clear_Color.x, clear_Color.y, clear_Color.z));
		std::cout << clear_Color.x << "|" << clear_Color.y << "|" << clear_Color.z << "|" << std::endl;

		if (m_pointlights.size() != 0)
		{
		//	m_pointlights[indexL]->setTransform(
		//		//Position
		//		glm::vec3(position.position),
		//		//Rotation
		//		glm::quat(),
		//		//Scale
		//		glm::vec3(1, 1, 1)
		//	);
			//m_pointlights[indexL]->setAttenuationAndRadius(glm::vec4(f, f, f, 1));
			m_pointlights[0]->setColor(glm::vec3(clear_Color.x, clear_Color.y, clear_Color.z));
		}
	}

	ImGui::EndGroup();

	ImGui::End();

#pragma endregion

#pragma region SHORTCUTS
	//DUPLICATE
	if (Input::isKeyHeldDown(GLFW_KEY_LEFT_CONTROL) && Input::isKeyPressed(GLFW_KEY_D))
	{
		if (listBox_ActiveMeshes_Current != -1)
			createDuplicate(m_objects, listBox_ActiveMeshes_Current);
	}

	//DELETE 

	//NEW LEVEL

	//OPEN LEVEL

	//SAVE LEVEL 

	//EXIT

#pragma endregion

#pragma region GRID


	/*ImGuizmo::DrawGrid(View, Proj, ID, 10.f);
	ImGuizmo::DrawCube(View, Proj, ID);*/

#pragma endregion

	//ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
}


void LevelEditState::updateState(const float& dt)
{
	for (GameObject* object : m_objects)
		if (object != nullptr)
			object->update(dt);



	m_camera->updateLevelEd();
	m_picker->update();
	//logTrace("MousePicker: ({0}, {1}, {2})", std::to_string(m_picker->getCurrentRay().x), std::to_string(m_picker->getCurrentRay().y), std::to_string(m_picker->getCurrentRay().z));
	Renderer::getInstance()->updateParticles(dt);
	for (GameObject* object : m_objects)
		object->update(dt);

}

void LevelEditState::quitEditor()
{
	Renderer::getInstance()->clear();
	m_stateManager->clearAllAndSetState(new MenuState());
	//glfwSetWindowShouldClose(glfwGetCurrentContext(), true); //<--- This option is for closing the whole program.
}

void LevelEditState::fileDirectoryUpdate()
{
	m_files.clear();
	m_fileNames.clear();
	int i = 0;
	for (const directory_entry dirEntry : recursive_directory_iterator(MESH_FILEPATH))
	{
		std::cout << dirEntry.path().string() << std::endl;
		m_files.push_back(dirEntry.path().string());
		m_fileNames.push_back(fileNameFormat(m_files[i], false));
		i++;
	}
}

bool LevelEditState::vecOfStrGet(void* data, int n, const char** out_text)
{
	return false;
}

std::string LevelEditState::fileNameFormat(std::string filePath, bool isPath)
{
	std::string finalString;

	std::string firstName = filePath;
	std::size_t found = firstName.find_last_of("/\\");
	std::string editedName = firstName.substr(found + 1);
	std::size_t foundDot = editedName.find_first_of(".");
	finalString = editedName.substr(0, foundDot);
	
	if (isPath)
	{
		std::string newPath = firstName;
		newPath.erase(found, editedName.size() + 1);
		std::size_t secondFound = newPath.find_last_of("/\\");
		std::string editedPath = newPath.substr(secondFound + 1);
		finalString = editedPath + "/" + editedName;
	}

	return finalString;
}

void LevelEditState::EditTransform(const float* cameraView, float* cameraProjection, float* objectMatrix)
{
	static ImGuizmo::OPERATION mCurrentOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentMode(ImGuizmo::LOCAL);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("TestTr", matrixTranslation, 3);
	ImGui::InputFloat3("TestRt", matrixRotation, 3);
	ImGui::InputFloat3("TestSc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, objectMatrix);

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentOperation, mCurrentMode, objectMatrix, NULL, NULL, NULL, NULL);
}

bool LevelEditState::ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
	if (values.empty()) { return false; }
	return ImGui::ListBox(label, currIndex, vector_getter,
		static_cast<void*>(&values), values.size());
}

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

void LevelEditState::loadBasicLight()
{
	Renderer* renderer = Renderer::getInstance();
	// Church
	m_pointlights.emplace_back(new Pointlight(glm::vec3(49.0f, 15.0f, 2.0f), glm::vec3(0.3, 0.85, 1.0)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 65.0f));

	// Middle
	m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 24.0f, 0.0f), glm::vec3(0.9, 0.17, 0.123)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.07f, 47.0f));

	// Court area
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-41.0f, 21.0f, 10.0f), glm::vec3(0.9, 0.2, 0.5)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.045f, 0.0075f, 100.0f));

	// Back wall platforms M
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-2.0f, 19.0f, -31.0f), glm::vec3(0.98, 0.675, 0.084)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));

	// Back wall platforms R
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-31.0f, 17.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14, 0.11f, 47.0f));

	// Back wall platforms L
	m_pointlights.emplace_back(new Pointlight(glm::vec3(29.0f, 19.0f, -37.0f), glm::vec3(0.98, 0.675, 0.084)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.14f, 0.11f, 47.0f));

	// Maze
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-100.0f, 13.0f, -4.0f), glm::vec3(0.9, 0.9, 1.0)));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.09f, 0.032f, 64.0f));

	for (Pointlight* p : m_pointlights)
		renderer->submit(p, RENDER_TYPE::POINTLIGHT_SOURCE);
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
	static int listBox_Particles_Current = 1;
	static int listBox_ActiveMeshes_Current = -1;

	float* View;
	float* Proj;
	float* ID;
	float* gridSize;
	

	glm::mat4 m_viewMat = m_camera->getViewMat();
	View = glm::value_ptr(m_viewMat);

	glm::mat4 m_projMat = m_camera->getProjMat();
	Proj = glm::value_ptr(m_projMat);

	glm::mat4 m_identity = glm::mat4(1.0f);
	ID = glm::value_ptr(m_identity);

	float matrixT[3], matrixR[3], matrixS[3];

	

	//ImGui::Begin("Level Editor");

//#pragma region AttribCheck
//	ImGui::BeginGroup();
//	if (Input::isKeyPressed(GLFW_KEY_W))
//		changeAttrib = 1;
//	ImGui::RadioButton("Translate", &changeAttrib, 1);
//	ImGui::SameLine(0.0f, 10.f);
//	if (Input::isKeyPressed(GLFW_KEY_E))
//		changeAttrib = 2;
//	ImGui::RadioButton("Rotate", &changeAttrib, 2);
//	ImGui::SameLine(0.0f, 10.f);
//	if (Input::isKeyPressed(GLFW_KEY_R))
//		changeAttrib = 3;
//	ImGui::RadioButton("Scale", &changeAttrib, 3);
//	ImGui::Separator();
//	
//	glm::vec3 meshTransform = glm::vec3(0, 0, 0);
//	glm::quat meshRotation = glm::quat(0.0, 0.0, 0.0, 1.0);
//	glm::vec3 meshScale = glm::vec3(0, 0, 0);
//	float objectX, objectY, objectZ;
//	float objRotX, objRotY, objRotZ;
//	objectX = objectY = objectZ = 0.0;
//	objRotX = objRotY = objRotZ = 0.0;
//
//	int index = listBox_ActiveMeshes_Current;
//	
//	if (index == lastMeshItem)
//	{
//
//		//Check if the vector has content or not
//		if (m_objects.size() != 0)
//		{
//			meshTransform = m_objects[index]->getTransform(0).position;
//			meshRotation = m_objects[index]->getTransform(0).rotation;
//			meshScale = m_objects[index]->getTransform(0).scale;
//		}
//
//		//The variables for the 
//		static float gTx[3] = { meshTransform.x, meshTransform.y, meshTransform.z };
//		ImGui::DragFloat3("Position", gTx, 0.01f, -1000000, 1000000);
//	
//
//		static float gRx[3] = { 0, 0, 0 };
//		ImGui::DragFloat3("Rotation", gRx, 0.01f, -1000000, 1000000);
//
//		static float gSx[3] = { 1, -1, 1 };
//		ImGui::DragFloat3("Scale", gSx, 0.01f, -1000000, 1000000);
//
//		if (m_objects.size() != 0)
//			m_objects[index]->setTransform(glm::vec3(gTx[0], gTx[1], gTx[2]),
//				glm::vec3(gRx[0], gRx[1], gRx[2]), glm::vec3(gSx[0], gSx[1], gSx[2]));
//	
//		lastMeshItem = listBox_ActiveMeshes_Current;
//	}
//
//	ImGui::EndGroup();
//	ImGui::End();
//#pragma endregion


#pragma region SceneList
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
				loadBasicLight();
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
		static int listBox_ActiveLights_Current = 1;
		ImGui::ListBox("Lights", &listBox_ActiveLights_Current, &m_LightsNames[0], m_LightsNames.size(), 6);
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
	case 1:
		ImGui::ListBox("Particles", &listBox_Particles_Current, listBox_Particles, IM_ARRAYSIZE(listBox_Particles), 6);
		break;
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

	glm::vec3 meshTransform = glm::vec3(0, 0, 0);
	glm::quat meshRotation = glm::quat(0.0, 0.0, 0.0, 1.0);
	glm::vec3 meshScale = glm::vec3(0, 0, 0);
	float objectX, objectY, objectZ;
	float objRotX, objRotY, objRotZ;
	objectX = objectY = objectZ = 0.0;
	objRotX = objRotY = objRotZ = 0.0;

	int index = listBox_ActiveMeshes_Current;
	
	ImGui::Text("Attribute");

	//if (index == lastMeshItem)
	//{

		//Check if the vector has content or not
	if (m_objects.size() != 0)
	{
			m_MeshPos[0] = m_objects[index]->getTransform(0).position.x;
			m_MeshPos[1] = m_objects[index]->getTransform(0).position.y;
			m_MeshPos[2] = m_objects[index]->getTransform(0).position.z;

			m_MeshRot[0] = m_objects[index]->getTransform(0).rotation.x;
			m_MeshRot[1] = m_objects[index]->getTransform(0).rotation.y;
			m_MeshRot[2] = m_objects[index]->getTransform(0).rotation.z;
			
			m_MeshSca[0] = m_objects[index]->getTransform(0).scale.x;
			m_MeshSca[1] = m_objects[index]->getTransform(0).scale.y;
			m_MeshSca[2] = m_objects[index]->getTransform(0).scale.z;
		}


		//The variables for the 
		static float gTx[3] = { m_MeshPos[0], m_MeshPos[1], m_MeshPos[2] };
		ImGui::DragFloat3("Position", gTx, 0.01f, -1000000, 1000000);

		static float gRx[3] = { m_MeshRot[0], m_MeshRot[1], m_MeshRot[2] };
		ImGui::DragFloat3("Rotation", gRx, 0.01f, -1000000, 1000000);

		static float gSx[3] = { 1, 1, 1 };
		ImGui::DragFloat3("Scale", gSx, 0.01f, -1000000, 1000000);

		if (m_objects.size() != 0)
			m_objects[index]->setTransform(glm::vec3(gTx[0], gTx[1], gTx[2]),
				glm::vec3(gRx[0], gRx[1], gRx[2]), glm::vec3(gSx[0], gSx[1], gSx[2]));
	
		lastMeshItem = listBox_ActiveMeshes_Current;
	//}

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

bool LevelEditState::ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
	if (values.empty()) { return false; }
	return ImGui::ListBox(label, currIndex, vector_getter,
		static_cast<void*>(&values), values.size());
}

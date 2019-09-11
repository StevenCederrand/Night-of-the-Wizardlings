#ifndef _PCH_H
#define _PCH_H



//Standard includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <limits.h>
#include <time.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <math.h>
#include <chrono>
#include <thread>
#include <string>
#include <stdio.h>

//GL related includes
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include "System/Log.h"
//Raknet
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetTypes.h>
#include <raknet/RakSleep.h>
#include <raknet/GetTime.h>
#include <raknet/BitStream.h>

//Custom
#include <Renderer/Shader.h>
#include <Renderer/ShaderMap.h>
#include <Renderer/Camera.h>	
#include <Renderer/Renderer.h>
#include <System/Input.h>

const std::string SHADERPATH = "Assets/Shaders/";

#endif

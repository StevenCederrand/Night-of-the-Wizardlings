#ifndef _PCH_H
#define _PCH_H

#include <iostream>
#include <fstream>
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

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include "System/Log.h"

#include <raknet/MessageIdentifiers.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetTypes.h>
#include <raknet/RakSleep.h>
#include <raknet/GetTime.h>
#include <raknet/BitStream.h>
#include <Networking/NetworkGlobals.h>
#include <Networking/NetworkPackets.h>

#endif

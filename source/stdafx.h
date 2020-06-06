// Copyright (c) 2019 Guillem Costa Miquel, kayter72@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. The above copyright notice and this permission notice shall be included in
//	  all copies or substantial portions of the Software.

#include <stdio.h>

#include <assert.h>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <memory>
#include <typeinfo>
#include <functional>
#include <algorithm>
#include <fstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//------ CONTRIB ---------

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "glad/glad.h"

#include "stb_image/stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include "SDL2/include/SDL.h"

#include "assimp/include/scene.h"
#include "assimp/include/Importer.hpp"
#include "assimp/include/postprocess.h"

#include "jsoncpp/json/json.h"

//------ INTERNAL ---------

#include "utilities/utilities.h"
#include "utilities/types.h"
#include "utilities/singleton.h"
#include "utilities/logging.h"
#include "utilities/json_utils.h"
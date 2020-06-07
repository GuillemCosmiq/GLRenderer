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

#include "resource_system.h"

#include "resources/resource.h"
#include "resources/mesh.h"
#include "resources/program.h"
#include "resources/texture.h"
#include "resources/cubemap.h"
#include "resources/framebuffer_object.h"

namespace_begin

void ResourceSystem::Initialize(Config& config)
{
	m_resources.reserve(50);
}

void ResourceSystem::Shutdown()
{
	// TODO: assert if resources still load on vram
}

namespace_end
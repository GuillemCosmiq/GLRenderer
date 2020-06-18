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

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#define namespace_begin namespace GLEngine {
#define namespace_end }

#define FLUSH_ENABLED

#define DF_SBUFF 1024

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define VISUAL_LOG
#endif

#define FRAME_MS 0.016f // Only supports 60 fps

#define DEFAULT_ROTATION_STRENGH 0.f
#define DEFAULT_MOVEMENT_AMPLITUDE 0.f
#define DEFAULT_MOVEMENT_STRENGH 0.f

#define NON_COPYABLE_CLASS(C) private: \
C( const C& ) = delete; \
C& operator=( const C& ) = delete

#define SIZEOF_ARRAY(A) sizeof(A)/sizeof(A[0])

template<typename T>
constexpr size_t SizeofArray(const T & arr)
{
	return SIZEOF_ARRAY(arr);
}

#endif
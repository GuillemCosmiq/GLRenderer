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

#ifndef __LOGGING_H__
#define __LOGGING_H__

#ifndef VISUAL_DEBUG_PRINTF
#if defined(VISUAL_LOG)
#define VISUAL_DEBUG_PRINTF(x) VDebugPrintF(x)
#else
#define VISUAL_DEBUG_PRINTF(x)
#endif
#endif

#ifndef LOG
#if defined(VISUAL_LOG)
#define LOG(verbosity, format, ...) DebugPrintF(verbosity, format, ##__VA_ARGS__)
#else
#define LOG(x)
#endif
#endif

#ifndef FLUSH_TO_FILE
#if defined(FLUSH_ENABLED)
#define FLUSH_TO_FILE(x) FlushToLogFile(x)
#else
#define FLUSH_TO_FILE(x)
#endif
#endif

int DebugPrintF(int verbosity, const char* format, ...);

extern int g_verbosity;

#endif
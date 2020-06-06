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

#include "logging.h"

int g_verbosity = 0;

static bool FlushToLogFile(const char* s_buffer)
{
	FILE* fp;
	fp = fopen("LogError.txt", "a+");
	if (fp)
	{
		const char* tmp_ptr = s_buffer + strlen(s_buffer);
		sprintf(const_cast<char*>(tmp_ptr), "\n");
		fwrite(s_buffer, sizeof(char), strlen(s_buffer), fp);
		fclose(fp);
		return true;
	}
	return false;
}

static inline void VDebugPrintF(const char* s_buffer)
{
	OutputDebugString(s_buffer);
}

int DebugPrintF(int verbosity, const char* format, ...)
{
	int ret = -1;

	if (g_verbosity >= verbosity)
	{
		va_list argList;
		va_start(argList, format);

		char s_buffer[DF_SBUFF];
		int charsWritten = vsnprintf(s_buffer, DF_SBUFF, format, argList);

		if (charsWritten >= 0)
			ret = 1;

		va_end(argList);

		if (ret)
			VISUAL_DEBUG_PRINTF(s_buffer);

		if (ret)
			ret = FLUSH_TO_FILE(s_buffer);

		return ret ? charsWritten : ret;
	}

	return ret;
}
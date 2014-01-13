#include "Nemo.h"
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>

int Nm_flength(FILE *f)
{
	int pos;
	int end;

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}

int Nm_flength(const char *filename)
{
	FILE *f = fopen(filename, "rb");

	if (!f)
	{
		return 0;
	}

	int i = Nm_flength(f);

	fclose(f);

	return i;
}

bool Nm_fexist(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	bool ok = (f != 0);
	if (f)
	{
		fclose(f);
	}
	return ok;
}

int Nm_LoadFile(const char *filename, void **bufferptr)
{
	FILE *f;
	int  length;
	void *buffer;

	*bufferptr = NULL;

	if (filename == NULL || strlen(filename) == 0)
	{
		return -1;
	}

	f = fopen(filename, "rb");
	if (!f)
	{
		return -1;
	}

	length = Nm_flength(f);

	buffer = malloc(length + 1);
	memset(buffer, 0, length);

	((char *)buffer)[length] = 0;
	fread(buffer, 1, length, f);
	fclose(f);

	*bufferptr = buffer;
	return length;
}

bool Nm_SaveFile(const char *filename, const char *bufptr)
{
	FILE *f;

	if (filename == NULL || strlen(filename) == 0)
	{
		return false;
	}

	f = fopen(filename, "wb");
	if (!f)
	{
		return false;
	}

	fwrite(bufptr, 1, strlen(bufptr), f);

	fclose(f);

	return true;
}

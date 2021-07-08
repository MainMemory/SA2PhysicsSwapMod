// SA2PhysicsSwapMod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include "SA2ModLoader.h"
#include "IniFile.hpp"
#include <algorithm>
#include <fstream>
using std::unordered_map;
using std::string;

HMODULE moduleHandle;

static const string charnames[] = {
	"sadxsonic",
	"sadxeggman",
	"sadxtails",
	"sadxknuckles",
	"sadxtikal",
	"sadxamy",
	"sadxgamma",
	"sadxbig",
	"sadxsupersonic",
	"sonic",
	"shadow",
	"tails",
	"eggman",
	"knuckles",
	"rouge",
	"mechtails",
	"mecheggman",
	"amy",
	"supersonic",
	"supershadow",
	"unused",
	"metalsonic",
	"chaowalker",
	"darkchaowalker",
	"tikal",
	"chaos",
	"unused2",
	"unused3",
	"heroessonic",
	"heroesknuckles",
	"heroestails",
	"heroesshadow",
	"heroesomega",
	"heroesrouge",
	"heroesamy",
	"heroesbig",
	"heroescream",
	"heroesespio",
	"heroesvector",
	"heroescharmy"
};

static unordered_map<string, uint8_t> charnamemap;

static string trim(const string &s)
{
	auto st = s.find_first_not_of(' ');
	if (st == string::npos)
		st = 0;
	auto ed = s.find_last_not_of(' ');
	if (ed == string::npos)
		ed = s.size() - 1;
	return s.substr(st, (ed + 1) - st);
}

static uint8_t ParseCharacterID(const string &str, int def)
{
	string s = trim(str);
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	auto ch = charnamemap.find(s);
	if (ch != charnamemap.end())
		return ch->second;
	return def;
}

static PhysicsData* GetPhysicsFile(const char* modPath)
{
	auto physPath = string(modPath) + "\\phys.bin";
	auto physFile = std::ifstream(physPath);
	auto expectedFileSize = LengthOfArray(charnames) * sizeof(PhysicsData);
	auto physDataArray = new char[expectedFileSize];

	physFile.seekg(0, std::ios::beg);
	physFile.read(physDataArray, expectedFileSize);

	return (PhysicsData*)physDataArray;
}

static const string keynames[] = { "Sonic", "Shadow", "Tails", "Eggman", "Knuckles", "Rouge", "MechTails", "MechEggman", "Amy", "SuperSonic", "SuperShadow", "Unused", "MetalSonic", "ChaoWalker", "DarkChaoWalker", "Tikal", "Chaos" };

static PhysicsData *const PhysicsArray = (PhysicsData *)0x17391E8;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char *path, const HelperFunctions &helperFunctions)
	{
		for (uint8_t i = 0; i < LengthOfArray(charnames); i++)
			charnamemap[charnames[i]] = i;

		const PhysicsData* physicsData = GetPhysicsFile(path);
		const IniFile *settings = new IniFile(std::string(path) + "\\config.ini");
		const int SADX_CHARACTER_COUNT = 9;
		
		for (uint8_t i = 0; i < LengthOfArray(keynames); i++)
		{
			uint8_t c = ParseCharacterID(settings->getString("", keynames[i]), i + SADX_CHARACTER_COUNT);
			memcpy(&PhysicsArray[i], &physicsData[c], offsetof(PhysicsData, anonymous_27));
			PhysicsArray[i].Gravity = physicsData[c].Gravity;
		}
		
		delete settings;
		delete[] physicsData;
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
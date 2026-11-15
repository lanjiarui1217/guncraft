#pragma once
#include <string>
#include "../Resource.h"
typedef unsigned long long ull;
typedef unsigned itemtype;
struct item { enum _ { air,bedrock,grasssoil,soil,sand,glass,snowsoil,stone,logwood,woodplank,
	TNT,leaf,glowstone,ender_pearl,ironore,rawiron,ironingot,AK47,grenade,guidedmissile,falcula,ITEMNUM }; };
constexpr ull ITEMNUM=(ull)item::ITEMNUM;
const std::string itemname[ITEMNUM]={"air","bedrock","grasssoil","soil","sand","glass","snowsoil","stone",
"logwood","woodplank","TNT","leaf","glowstone","ender_pearl","ironore","rawiron","ironingot","AK47","grenade","guidedmissile","falcula"};
constexpr int itemtexid[ITEMNUM]=
{
	0,
	IDB_BEDROCK,
	IDB_GRSIL,
	IDB_SOIL,
	IDB_SAND,
	IDB_WHITE,
	IDB_SNSIL,
	IDB_STONE,
	IDB_LGWD,
	IDB_WDPLK,
	IDB_TNT,
	IDB_LEAF,
	IDC_GLSTN,
	IDC_ENDERPEARL,
	IDC_IRONORE,
	IDC_RAWIRON,
	IDC_IRONINGOT,
	IDC_AK47,
	IDC_GRENADE,
	IDC_GUIDEDMISSILE,
	IDC_FALCULA
};// IDC

constexpr int toblock[ITEMNUM]=
{
	block::air,block::bedrock,block::grasssoil,block::soil,block::sand,block::glass,block::snowsoil,block::stone,block::logwood,block::woodplank,block::TNT,block::leaf,
	block::glowstone,0,block::ironore,0,0,0,0,0,0
};
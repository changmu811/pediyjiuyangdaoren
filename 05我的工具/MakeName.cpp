#include "stdafx.h"
#include "MakeName.h"
#include "Character.h"

CMakeName::CMakeName()
{
}


CMakeName::~CMakeName()
{
}

// 创建男人名称
CString CMakeName::ManName()
{
	int i = rand() % 307;
	int j = rand() % 1700;
	CString str;
	CString name;
	name.Format(L"%ls", g_Surname[i]);
	str.Format(L"%ls", g_ManName[j]);
	name += str;
	return name;	
}

// 创建女人名称
CString CMakeName::WomanName()
{
	int i = rand() % 307;
	int j = rand() % 4608;
	CString str;
	CString name;
	name.Format(L"%ls", g_Surname[i]);
	str.Format(L"%ls", g_WomanName[j]);
	name += str;
	return name;
}

// 创建药材名称
CString CMakeName::CrudeDrugsName()
{
	int i = rand() % 80;
	int j = rand() % 400;
	int k = rand() % 47;
	CString str;
	CString name;
	name.Format(L"%ls", g_CrudeHead[i]);
	str.Format(L"%ls", g_Current[j]);
	name += str;
	str.Empty();
	str.Format(L"%ls", g_CrudeEnd[k]);
	name += str;
	return name;
	
}

// 创建丹药名称
CString CMakeName::ElixirName()
{
	int i = 0;
	int j = rand() % 400;
	int k = rand() % 20;
	CString str;
	CString name;
	if (rand()%3)
	{
		i = rand() % 237;
		name.Format(L"%ls", g_ElixirHead[i]);

	}
	else
	{
		i = rand() % 80;
		name.Format(L"%ls", g_CrudeHead[i]);
	}
	str.Format(L"%ls", g_Current[j]);
	name += str;
	str.Empty();
	str.Format(L"%ls", g_ElixirEnd[k]);
	name += str;
	return name;

}

// 创建矿石名称
CString CMakeName::MineralName()
{
	int i = 0;
	int j = rand() % 400;
	int k = rand() % 61;
	CString str;
	CString name;
	if (rand() % 4)
	{
		i = rand() % 234;
		name.Format(L"%ls", g_MineralHead[i]);

	}
	else
	{
		i = rand() % 80;
		name.Format(L"%ls", g_CrudeHead[i]);
	}
	str.Format(L"%ls", g_Current[j]);
	name += str;
	str.Empty();
	str.Format(L"%ls", g_MineralEnd[k]);
	name += str;
	return name;

}

// 创建武器法宝名称
CString CMakeName::WeaponName()
{
	int i = 0;
	int j = rand() % 400;
	int k = rand() % 66;
	CString str;
	CString name;
	if (rand() % 4)
	{
		i = rand() % 234;
		name.Format(L"%ls", g_MineralHead[i]);

	}
	else
	{
		i = rand() % 80;
		name.Format(L"%ls", g_CrudeHead[i]);
	}
	str.Format(L"%ls", g_Current[j]);
	name += str;
	str.Empty();
	str.Format(L"%ls", g_WeaponEnd[k]);
	name += str;
	return name;
}

// 创建地点名称
CString CMakeName::PlaceName()
{
	int i = rand() % 100;
	int j = rand() % 40;
	CString str;
	CString name;
	name.Format(L"%ls", g_PlaceHead[i]);
	str.Format(L"%ls", g_PlaceEnd[j]);
	name += str;
	return name;
}

// 创建宗派名称
CString CMakeName::SectName()
{
	int i = rand() % 50;
	int j = rand() % 40;
	CString str;
	CString name;
	name.Format(L"%ls", g_SectHead[i]);
	str.Format(L"%ls", g_SectEnd[j]);
	name += str;
	return name;
}

// 创建国家名称
CString CMakeName::CountryName()
{
	int i = rand() % 100;
	
	CString str;
	CString name;
	name.Format(L"%ls", g_PlaceHead[i]);
	if (rand()%2)
		str.Format(L"帝国");	
	else
		str.Format(L"皇朝");		
	name += str;
	return name;

}

// 创建妖兽名称
CString CMakeName::MonsterName()
{
	int i = rand() % 60;
	int j = rand() % 40;
	CString str;
	CString name;
	name.Format(L"%ls", g_MonsterHead[i]);
	str.Format(L"%ls", g_MonsterEnd[j]);
	name += str;
	return name;
}
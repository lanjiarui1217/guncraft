#include "item.h"
#include "Player.h"

ull ItemIniterDeler::downnum=0;

void ItemDrawUtil::draw()
{
	// 暂不画 HUD（热键栏）：等软件渲染 HUD 落地后再实装
	for(ull i(0);i<10;++i)
	{
		if(tspl.itemlist[i].num==0) tspl.itemlist[i].tp=item::ITEMNUM;
	}
}

bool ItemIniterDeler::down()
{
	return downnum>0;
}
ItemIniterDeler::ItemIniterDeler()
{
	if(downnum)
	{
		++downnum;
		return;
	}
	++downnum;
}
ItemIniterDeler::~ItemIniterDeler()
{
	--downnum;
}

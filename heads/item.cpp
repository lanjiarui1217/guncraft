#include "item.h"
#include "direct3d.h"
#include "../Resource.h"
#include "Player.h"

Texture ItemDrawUtil::texture[ITEMNUM];
Texture ItemDrawUtil::hotbar;
Texture ItemDrawUtil::hotbar1;
VtxBuf ItemDrawUtil::vertexbuffer[ITEMNUM];
ull ItemIniterDeler::downnum=0;
extern HWND hwnd;
void ItemDrawUtil::draw()
{
	RECT rc;
	GetClientRect(hwnd,&rc);
	long w=rc.right,h=rc.bottom;
	int num=10;
	long a=35,d=a*num/2;

	//d3ddev->SetTexture(0,hotbar);
	//DrawTexture(w/2-d,h-a,w/2+d,h,0,0,1,1);
	for(ull i(0);i<num;++i)
	{
		// (w/2-d+a*i,h-a)-(w/2-d+a*i+a,h)
		d3ddev->SetTexture(0,i==tspl.chooseind?hotbar1:hotbar);
		DrawTexture(w/2-d+a*i,h-a,w/2-d+a*i+a,h,0,0,1,1);
	}
	for(ull i(0);i<num;++i)
	{
		if(tspl.itemlist[i].num==0) tspl.itemlist[i].tp=ITEMNUM;
		if(tspl.itemlist[i].tp==item::ITEMNUM) continue;
		d3ddev->SetTexture(0,texture[tspl.itemlist[i].tp]);
		DrawTexture(w/2-d+a*i+4,h-a+4,w/2-d+a*i+a-4,h-4,0,0,1,1);
		char s[100];
		sprintf(s,"%llu",tspl.itemlist[i].num);
		Drawtxt(w/2-d+a*i+a-(strlen(s)==1?8:16),h-16,8,16,s,strlen(s),lettersw);
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
	SetTexture(ItemDrawUtil::hotbar,IDB_HOTBAR);
	SetTexture(ItemDrawUtil::hotbar1,IDB_HOTBAR1);
	for(ull k(0);k<ITEMNUM;++k)
	{
		SetTexture(ItemDrawUtil::texture[k],itemtexid[k]);
	}
	++downnum;
}

ItemIniterDeler::~ItemIniterDeler()
{
	--downnum;
	if(downnum) return;
	sfrls(ItemDrawUtil::hotbar);
	sfrls(ItemDrawUtil::hotbar1);
	for(ull i(0);i<ITEMNUM;++i)
	{
		sfrls(ItemDrawUtil::vertexbuffer[i]);
		sfrls(ItemDrawUtil::texture[i]);
	}
}
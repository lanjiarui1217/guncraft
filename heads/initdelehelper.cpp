#pragma warning(push)
#pragma warning(disable:4005)
#include "initdelehelper.h"
#include <vector>
#pragma warning(pop)
std::vector<Inithelper*> initlist;
std::vector<Delehelper*> delelist;
void initall() { for(Inithelper* pih:initlist) pih->init(); initlist.clear(); }
void deleall() { for(Delehelper* pdh:delelist) pdh->dele(); delelist.clear(); }

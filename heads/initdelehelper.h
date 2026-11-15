#pragma once
#include <vector>
class Inithelper { public: virtual void init()=0; };
class Delehelper { public: virtual void dele()=0; };
void initall();
void deleall();
extern std::vector<Inithelper*> initlist;
extern std::vector<Delehelper*> delelist;

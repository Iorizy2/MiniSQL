#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "../ERROR/error.h"
#include <string>
// file name convert .idx to .dbf 
std::string IdxToDbf(std::string idx_name);
// file name convert .dbf to .idx 
std::string DbfToIdx(std::string idx_name);

#endif

#include "global.h"

std::string IdxToDbf(std::string idx_name)
{
	std::string dbf_name(idx_name);

	int i = idx_name.size()-1;
	while (idx_name[i] != '.')
		i--;
	if (i < 0) throw ERROR::FILENAME_CONVERT_FAILED;
	idx_name[i + 1] = 'd';
	idx_name[i + 2] = 'b';
	idx_name[i + 3] = 'f';
	idx_name[i + 4] = '\0';

	return dbf_name;
}

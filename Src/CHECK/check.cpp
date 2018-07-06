#include "check.h"
#include "../APILIB/APILIB.h"
void Check_TB_Create_Info(const TB_Create_Info &tb_create_info)
{
	// 表名
	std::string table_name = tb_create_info.table_name;

	// 检查每个字段的信息
	for (int i = 0; i < tb_create_info.columns_info.size(); i++)
	{
		auto &column = tb_create_info.columns_info[i];
		
		// 检查字段名称长度
		if (column.name.size() >= ColumnNameLength)
			throw SQLError::TABLE_ERROR("Error!Column name length overflow");

		// 检查字段类型
		if(column.type !=Column_Type::C || column.type != Column_Type::D|| column.type != Column_Type::I)
			throw SQLError::TABLE_ERROR("Column data type error!");
	}

	// 检查是否多个关键字
	int primary_count = 0;
	for (auto &e : tb_create_info.columns_info)
		if (e.isPrimary) primary_count++;
	
	if(primary_count>1)
		throw SQLError::TABLE_ERROR("Error!More than one primary key!");


	// 检查字段个数
	if(tb_create_info.columns_info.size()>RecordColumnCount)
		throw SQLError::TABLE_ERROR("Error!Column count is overflow!");
}

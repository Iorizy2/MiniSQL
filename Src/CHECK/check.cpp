#include "check.h"
#include "../APILIB/APILIB.h"
void Check_TB_Create_Info(const TB_Create_Info &tb_create_info)
{
	// ����
	std::string table_name = tb_create_info.table_name;

	// ���ÿ���ֶε���Ϣ
	for (int i = 0; i < tb_create_info.columns_info.size(); i++)
	{
		auto &column = tb_create_info.columns_info[i];
		
		// ����ֶ����Ƴ���
		if (column.name.size() >= ColumnNameLength)
			throw SQLError::TABLE_ERROR("Error!Column name length overflow");

		// ����ֶ�����
		if(column.type !=Column_Type::C || column.type != Column_Type::D|| column.type != Column_Type::I)
			throw SQLError::TABLE_ERROR("Column data type error!");
	}

	// ����Ƿ����ؼ���
	int primary_count = 0;
	for (auto &e : tb_create_info.columns_info)
		if (e.isPrimary) primary_count++;
	
	if(primary_count>1)
		throw SQLError::TABLE_ERROR("Error!More than one primary key!");


	// ����ֶθ���
	if(tb_create_info.columns_info.size()>RecordColumnCount)
		throw SQLError::TABLE_ERROR("Error!Column count is overflow!");
}

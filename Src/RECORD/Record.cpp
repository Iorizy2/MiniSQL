#include "Record.h"

RecordHead::RecordHead()
	:phead(nullptr), data(nullptr), pLast(nullptr)
{

}

size_t RecordHead::size() const
{
	unsigned long sz = 0;
	auto p = phead;
	while (p)
	{
		sz += p->size();
		p = p->next;
	}
	return sz;
}

const Column_Cell* RecordHead::GetFirstColumn()const
{
	return phead;
}

RecordHead::~RecordHead()
{
	while (phead)
	{
		auto next = phead->next;
		delete phead;
		phead = next;
	}

	if (data) delete data;
}

void RecordHead::AddColumnCell(const Column_Cell &cc)
{
	if (!phead)
	{
		phead = new Column_Cell;
		*phead = cc;
		phead->next = nullptr;
		pLast = phead;
	}
	else
	{
		pLast->next = new Column_Cell;
		*(pLast->next) = cc;
		pLast = pLast->next;
		pLast->next = nullptr;
	}
}

size_t Column_Cell::size() const
{
	size_t sz = 0;

	switch (column_type)
	{
	case Column_Type::I:
		return sizeof(int);
		break;

	case Column_Type::C:
		// 字符串字段的前三个字符表示字符串的长度 0-999
		assert(strlen(column_value.StrValue) > 3);
		for (int i = 0; i < 3; i++)
			sz = sz * 10 + (column_value.StrValue[i] - '0');
		return sz;
		break;

	case Column_Type::D:
		return sizeof(double);
		break;
	default:
		break;
	}
	return 0;
}

void* Column_Cell::data() const
{
	switch (column_type)
	{
	case Column_Type::I:
		return (void*)&column_value.IntValue;
		break;
	case Column_Type::C:
		return column_value.StrValue;
		break;
	case Column_Type::D:
		return (void*)&column_value.DoubleValue;
		break;
	default:
		break;
	}
	return 0;
}

Column_Cell::~Column_Cell()
{
	if (column_type == Column_Type::C)
	{
		if (column_value.StrValue)
		{
			delete column_value.StrValue;
			column_value.StrValue = nullptr;
		}
			
	}
}

Column_Cell& Column_Cell::operator=(const Column_Cell&rhs)
{
	column_type = rhs.column_type;
	columu_name = rhs.columu_name;
	column_value = rhs.column_value;

	// 如果是指针类型，则指保留一个指针副本
	
	if (rhs.column_type == Column_Type::C)
	{
		Column_Cell& tmp = const_cast<Column_Cell&>(rhs);
		tmp.column_value.StrValue = nullptr;
	}
	return *this;
}

FileAddr Record::InsertRecord(const std::string dbf_name, const RecordHead &rd)
{
	// 记录数据的副本
	auto tp = GetRecordData(rd);

	// 插入记录
	auto fd =  GetGlobalFileBuffer()[dbf_name.c_str()]->AddRecord(std::get<1>(tp), std::get<0>(tp));
	delete std::get<1>(tp);  // 释放副本内存
	return fd;
}

FileAddr Record::DeleteRecord(const std::string dbf_name, FileAddr fd, size_t record_size)
{
	return GetGlobalFileBuffer()[dbf_name.c_str()]->DeleteRecord(&fd, record_size);
}

bool Record::UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd)
{
	// 记录数据的副本
	auto tp = GetRecordData(rd);
	auto bUpdate = GetGlobalFileBuffer()[dbf_name.c_str()]->UpdateRecord(&fd, std::get<1>(tp), std::get<0>(tp));
	delete std::get<1>(tp);  // 释放副本内存
	return bUpdate;
}

std::tuple<unsigned long, char*> Record::GetRecordData(const RecordHead &rd)
{
	// 记录数据的副本
	unsigned long data_size = rd.size();
	char *rd_data = (char*)malloc(data_size);
	auto pcolumn = rd.GetFirstColumn();

	unsigned long offset = 0;
	while (pcolumn)
	{
		memcpy(rd_data + offset, pcolumn->data(), pcolumn->size());
		offset += pcolumn->size();
		pcolumn = pcolumn->next;
	}
	assert(data_size == offset);

	auto tp = std::make_tuple(data_size, rd_data);
	return tp;
}

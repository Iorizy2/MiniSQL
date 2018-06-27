#include "Record.h"


std::ostream& operator<<(std::ostream &os, const KeyAttr &key)
{
	switch (key.type)
	{
	case Column_Type::I:
		os << key.value.IntValue << " ";
		break;

	case Column_Type::C:
		os << key.value.StrValue << " ";
		break;

	case Column_Type::D:
		os << key.value.DoubleValue << " ";
		break;
	default:
		break;
	}

	return os;
}

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
		// �ַ����ֶε�ǰ�����ַ���ʾ�ַ����ĳ��� 0-999
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
		return column_value.StrValue+3; // ǰ�����ֽڱ�����ַ�������ĳ��ȣ�����Ҫ����
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

	// �����ָ�����ͣ���ָ����һ��ָ�븱��
	
	if (rhs.column_type == Column_Type::C)
	{
		Column_Cell& tmp = const_cast<Column_Cell&>(rhs);
		tmp.column_value.StrValue = nullptr;
	}
	return *this;
}

FileAddr Record::InsertRecord(const std::string dbf_name, const RecordHead &rd)
{
	// ��¼���ݵĸ���
	auto tp = GetRecordData(rd);

	// �����¼
	auto fd =  GetGlobalFileBuffer()[dbf_name.c_str()]->AddRecord(std::get<1>(tp), std::get<0>(tp));
	delete std::get<1>(tp);  // �ͷŸ����ڴ�
	return fd;
}

FileAddr Record::DeleteRecord(const std::string dbf_name, FileAddr fd, size_t record_size)
{
	return GetGlobalFileBuffer()[dbf_name.c_str()]->DeleteRecord(&fd, record_size);
}

bool Record::UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd)
{
	// ��¼���ݵĸ���
	auto tp = GetRecordData(rd);
	auto bUpdate = GetGlobalFileBuffer()[dbf_name.c_str()]->UpdateRecord(&fd, std::get<1>(tp), std::get<0>(tp));
	delete std::get<1>(tp);  // �ͷŸ����ڴ�
	return bUpdate;
}

std::tuple<unsigned long, char*> Record::GetRecordData(const RecordHead &rd)
{
	// ��¼���ݵĸ���
	unsigned long data_size = rd.size();
	char *rd_data = (char*)malloc(data_size);
	memset(rd_data, 0, data_size);
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

bool KeyAttr::operator<(const KeyAttr &rhs)
{
	if (this->type != rhs.type)
		return false;

	bool res = true;
	std::string s1;
	std::string s2;

	switch (this->type)
	{
	case Column_Type::I:
		res = this->value.IntValue < rhs.value.IntValue;
		break;

	case Column_Type::C:
		s1 = std::string(this->value.StrValue);
		s2 = std::string(rhs.value.StrValue);
		res = s1 < s2;
		break;

	case Column_Type::D:
		res = this->value.DoubleValue < rhs.value.DoubleValue;
		break;
	default:
		break;
	}
	return res;
}

bool KeyAttr::operator>(const KeyAttr &rhs)
{
	if (this->type != rhs.type)
		return false;

	bool res = true;
	std::string s1;
	std::string s2;

	switch (this->type)
	{
	case Column_Type::I:
		res = this->value.IntValue > rhs.value.IntValue;
		break;

	case Column_Type::C:
		s1 = std::string(this->value.StrValue);
		s2 = std::string(rhs.value.StrValue);
		res = s1 > s2;
		break;

	case Column_Type::D:
		res = this->value.DoubleValue > rhs.value.DoubleValue;
		break;
	default:
		break;
	}
	return res;
}

bool KeyAttr::operator>=(const KeyAttr &rhs)
{
	if (this->type != rhs.type)
		return false;

	return !(*this < rhs);
}

bool KeyAttr::operator!=(const KeyAttr &rhs)
{
	if (this->type != rhs.type)
		return false;

	return !(*this == rhs);
}

bool KeyAttr::operator<=(const KeyAttr &rhs)
{
	if (this->type != rhs.type)
		return false;

	return !(*this > rhs);
}

bool KeyAttr::operator==(const KeyAttr &rhs)
{
	if (this->type != rhs.type)
		return false;

	bool res = true;
	std::string s1;
	std::string s2;

	switch (this->type)
	{
	case Column_Type::I:
		res = (this->value.IntValue == rhs.value.IntValue);
		break;

	case Column_Type::C:
		s1 = std::string(this->value.StrValue);
		s2 = std::string(rhs.value.StrValue);
		res = (s1 == s2);
		break;

	case Column_Type::D:
		res = (this->value.DoubleValue == rhs.value.DoubleValue);
		break;
	default:
		break;
	}
	return res;
}

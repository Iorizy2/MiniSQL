/***********************************************************************************

** 文件名: check.h

** Copyright (c)

** 创建人: ReFantasy/TDL

** 日  期: 2018-07-06

** 描  述: 类型检查模块

** 功  能：相关语法以及数据类型的合法检查

** 版  本: 1.00

***********************************************************************************/
#include "../GLOBAL/global.h"
#include "../APILIB/APILIB.h"

struct TB_Create_Info;
struct TB_Insert_Info;
struct TB_Update_Info;
struct TB_Delete_Info;
struct TB_Select_Info;

// 创建新表信息检查
void Check_TB_Create_Info(const TB_Create_Info &tb_create_info);

// 表插入信息检查
void Check_TB_Insert_Info(const TB_Insert_Info &tb_insert_info);

// 表更新信息检查
void Check_TB_Update_Info(const TB_Update_Info &tb_update_info);

// 删除信息检查
void Check_TB_Delete_Info(const TB_Delete_Info &tb_delete_info);

// 查找信息检查
void Check_TB_Select_Info(const TB_Select_Info &tb_select_info);
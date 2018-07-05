# MiniSQL

**一个简单的数据库实现 你喜欢的样子我都有~~** 

**编程语言 :**  C/C++

**运行平台 :**  Windows

**可移植性 :**  Linux  

**开发方式 :**  自底向上模块化开发

**第三方依赖库 :**  无

**主要功能及命令列表 :**

- 创建/删除数据库

  ```
  CREATE DATABASE 数据库名;
  DROP DATABASE  数据库名;
  ```

- 显示数据库

  ```
  SHOW DATABASES;
  ```

  

- 选择数据库

  ```
  USE DATABASE;
  ```

  

- 创建/删除表

  ```
  CREATE TABLE table_name (column_name column_type);
  DROP TABLE table_name ;
  ```

- 添加/删除记录

  ```
  INSERT INTO table_name ( field1, field2,...fieldN )
                         VALUES
                         ( value1, value2,...valueN );
  ```

  ```
  DELETE from table_name where expr;
  // eg. delete from student where id=3;
  ```

  

- 更新记录

  ```
  UPDATE table_name SET field1=new-value1, field2=new-value2...where expr;
  ```

- 条件查找记录

  ```
  SELECT * FROM table_name where contion_expr;
  ```
  





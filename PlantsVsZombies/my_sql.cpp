#include "my_sql.h"

void InitMysql() {
	if (mysql_init(&mysql) == NULL)	//初始化句柄mysql
	{
		printf("%d : error : %s\n", __LINE__, mysql_error(&mysql));
		exit(1);
	}
	if (mysql_library_init(0, NULL, NULL) != 0) 	//初始化mysql数据库
	{
		fprintf(stderr, "could not initialize MySQL client library\n");
		exit(1);
	}
	if (NULL == mysql_real_connect
	(&mysql,
		"127.0.0.1",
		"root",
		"123456",
		"pvz",
		0,
		NULL,
		0)
		)						//与mysql服务器建立连接	
	{
		printf("%d : error : %s\n", __LINE__, mysql_error(&mysql));
		exit(1);
	}
	if (mysql_set_character_set(&mysql, "gb2312") != 0)	//设置中文字符集，VS从控制台输入的汉字为gb2312编码，故使用gb2312而不使用utf8.
	{
		printf("%d : error : %s\n", __LINE__, mysql_error(&mysql));
		exit(1);
	}
	//printf("connect success!\n");
}

void SearchInfo(char* table, char* field_name, char* index_str) {
	MYSQL_RES* res;
	MYSQL_ROW row;
	int field = 0;
	char query_str[500] = { 0 };
	sprintf(query_str, "select %s from %s %s", field_name, table, index_str);
	int ret = 0;
	ret = mysql_real_query(&mysql, query_str, strlen(query_str));
	if (ret) {
		printf("查询信息出错：%s", mysql_error(&mysql));
		return;
	}
	res = mysql_store_result(&mysql);
	if (!mysql_num_rows(res)) {
		printf("符合条件的信息不存在\n");
		return;
	}
	field = mysql_num_fields(res);		//获得数据表的列数
	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i < field; i++)
		{
			printf("%-20s", row[i]);
		}
		printf("\n");
	}
}

void Change(char* table, char* field_name, char* index_str) {
	int field = 0;
	char query_str[500] = { 0 };
	sprintf_s(query_str, "UPDATE %s SET %s %s", table, field_name, index_str);
	int ret = 0;
	ret = mysql_real_query(&mysql, query_str, strlen(query_str));
	if (ret) {
		printf("修改信息出错：%s\n", mysql_error(&mysql));
		return;
	}
	printf("修改成功！\n");
	return;
}

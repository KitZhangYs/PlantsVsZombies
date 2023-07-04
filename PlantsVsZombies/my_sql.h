#pragma once
#include "include/mysql.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib,"libmysql")


extern MYSQL mysql;

extern void InitMysql();
extern void SearchInfo(char* table, char* key, char* index_str);
extern void Change(char* table, char* field_name, char* index_str);
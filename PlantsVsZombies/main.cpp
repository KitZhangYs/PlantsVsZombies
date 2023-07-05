#include "my_sql.h"
#include "game.h"

MYSQL mysql = { 0 };

int main() {
	//InitMysql();
	InitGame();
	GameStart();

	//mysql_close(&mysql);
	//mysql_library_end();
	return 0;
}
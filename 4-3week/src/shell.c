#include "network-programming.h"

int main()
{
	//顯示shell字符
	printf("%% ");

	//初始化環境變數
	setenv("PATH", "bin:.", 1);

	//用來讀取輸入指令的buffer
	char command_buffer[256];

	//創建暫存資料的雙向鍊結串列的head
	dllNode_t * count_list = DLL_init();

	while(1)
	{
		//讀取使用者輸入的命令
		fgets(command_buffer, 256, stdin);
		//丟到分析函數裡
		parse(command_buffer, count_list);
		//執行完重新顯示shell字符
		printf("%% ");
	}
}

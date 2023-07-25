#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_console.h"
#include "cmdline.h"
//#include "misc/utils.h"

#include "cmdline.h"

//static const char *TAG = "cmd";

void CmdHelp( Cmd_stype *pList )
{
	uint32_t i;

	printf("====== Help ====== \r\n");
	for( i = 0 ; i < 0xff; i++ )
	{
		if( pList[i].ucArgNum == 0 )
		{
			return;
		}
#if 0
		if( (pList[i].pcCmd1 == NULL) && (pList[i].pcCmd2 == NULL) )
		{
			ESP_LOGI(TAG,"%d,", pList[i].ucArgNum );
		}
#endif
		if( (pList[i].pcCmd1 != NULL) && (pList[i].pcCmd2 == NULL) )
		{
			printf("%d,%s \r\n", pList[i].ucArgNum, pList[i].pcCmd1 );
		}

		if( (pList[i].pcCmd1 != NULL) && (pList[i].pcCmd2 != NULL) )
		{
			printf("%d,%s %s \r\n", pList[i].ucArgNum, pList[i].pcCmd1,pList[i].pcCmd2 );
		}
	}
}


void CmdFuncExe(Cmd_stype *pList, int argc, char **argv)
{
	if( (pList->ucArgNum)+1 != argc )
	{
		printf("Argument Number Error %d:%d \r\n", argc, (pList->ucArgNum)+1);
		return;
	}

	if( pList->pFunction1 != NULL )
		pList->pFunction1();

	if( pList->pFunction2 != NULL )
		pList->pFunction2(argc, argv);
}

void CmdRun(Cmd_stype *pList, int argc, char **argv)
{
	uint32_t i;
	//Cmd_stype *pList = test;

	for( i = 0 ; i < 0xff; i++ )
	{
		if( pList[i].ucArgNum == 0 )
		{
			return;
		}

		if( argc == 1 )
		{
			CmdHelp( pList );
			return;
		}

		if( (pList[i].pcCmd1 == NULL) && (pList[i].pcCmd2 == NULL) )
		{
			CmdFuncExe( &pList[i], argc, argv);
			break;
		}

		if( (pList[i].pcCmd1 != NULL) && (argc >= 2) )
		if( !strcmp(pList[i].pcCmd1,argv[1]) && (pList[i].pcCmd2 == NULL) )
		{
			CmdFuncExe( &pList[i], argc, argv);
			break;
		}

		if( (pList[i].pcCmd1 != NULL) && (pList[i].pcCmd2 != NULL) && (argc >= 3) )
		if( !strcmp(pList[i].pcCmd1,argv[1]) && !strcmp(pList[i].pcCmd2,argv[2]) )
		{
			CmdFuncExe( &pList[i], argc, argv);
			break;
		}

	}
}


#if 0
void CMD_TEST_SETUP1( int argc, char **argv )
{
	ESP_LOGI(TAG, "argc:%d", argc);
}

void CMD_TEST_Init(void)
{
	ESP_LOGI(TAG, "CMD_TEST_Init()" );
}

void CMD_TEST_DeInit(void)
{
	ESP_LOGI(TAG, "CMD_TEST_DeInit()" );
}

void CMD_TEST_START(void)
{
	ESP_LOGI(TAG, "CMD_TEST_START()" );
}

void CMD_TEST_STOP(void)
{
	ESP_LOGI(TAG, "CMD_TEST_STOP()" );
}


Cmd_stype test[] =
{
	{ 1, "init", 	NULL, 		CMD_TEST_Init, 			NULL },
	{ 1, "deinit", 	NULL, 		CMD_TEST_DeInit,		NULL },

	{ 2, "test", 	"start", 	CMD_TEST_START,			NULL },
	{ 2, "test", 	"stop", 	CMD_TEST_STOP,			NULL },

//		{ 1, NULL, 		NULL, 		NULL,					CMD_TEST_SETUP1 },
//		{ 2, "setup", 	NULL, 		NULL,					CMD_TEST_SETUP1 },
//		{ 2, "setup", 	NULL, 		NULL, 				PCNT_TEST },
//		{ 2, "setup", 	NULL, 		NULL, 			PCNT_TEST },

	{ 0, NULL, 		NULL, 		NULL, 			NULL }
};

#endif

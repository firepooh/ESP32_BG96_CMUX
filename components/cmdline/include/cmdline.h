#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t 	ucArgNum;	/* input argument number */
	const char *pcCmd1;     /* first string cmd */
	const char *pcCmd2;		/* second string cmd */
	void 		(*pFunction1)(void);	/* void void function pointer */
	void 		(*pFunction2)(int argc, char **argv);	/* (void)(int argc, char **argv) function pointer */
}Cmd_stype;

void CmdRun(Cmd_stype *pList, int argc, char **argv);

#ifdef __cplusplus
}
#endif
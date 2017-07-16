#ifndef CSRC_LOG_H
#define CSRC_LOG_H

#include <stdio.h>

#ifndef __DEBUG_FLAG__
#	define __DEBUG_FLAG__ 1
#endif

#define	LOG_PRINT(fmt, ...) \
			printf("[log]%s(): " fmt "\n", __func__, ## __VA_ARGS__)

#define	ERR_PRINT(fmt, ...) \
		fprintf(stderr, "[log]%s(): " fmt "\n", __func__, ## __VA_ARGS__)

#endif

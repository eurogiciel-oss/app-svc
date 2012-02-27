/*
 *  app-svc
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>, Sewook Park <sewook7.park@samsung.com>, Jaeho Lee <jaeho81.lee@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <tet_api.h>
#include <appsvc.h>
#include <bundle.h>
#include <unistd.h>

static void startup(void);
static void cleanup(void);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_appsvc_set_operation_func_01(void);
static void utc_appsvc_set_operation_func_02(void);
static void utc_appsvc_set_operation_func_03(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{utc_appsvc_set_operation_func_01, POSITIVE_TC_IDX},
	{utc_appsvc_set_operation_func_02, NEGATIVE_TC_IDX},
	{utc_appsvc_set_operation_func_03, NEGATIVE_TC_IDX},
	{NULL, 0}
};

static void startup(void)
{
	tet_infoline("Testing API appsvc_set_operation starts");
}

static void cleanup(void)
{
	tet_infoline("Testing API appsvc_set_operation ends");
}

/**
 * @brief Positive test case of appsvc_set_operation()
 */
static void utc_appsvc_set_operation_func_01(void)
{
	int ret = APPSVC_RET_OK;
	bundle *b = NULL;
	b = bundle_create();
	if (b == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
	if (ret == APPSVC_RET_OK) {
		tet_infoline("appsvc_set_operation test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_set_operation test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	bundle_free(b);
	return;
}

/**
 * @brief Negative test case of appsvc_set_operation()
 */
static void utc_appsvc_set_operation_func_02(void)
{
	int ret = APPSVC_RET_OK;
	bundle *b = NULL;
	b = bundle_create();
	if (b == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = appsvc_set_operation(b, NULL);
	if (ret == APPSVC_RET_EINVAL) {
		tet_infoline("appsvc_set_operation test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_set_operation test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	bundle_free(b);
	return;
}

/**
 * @brief Negative test case of appsvc_set_operation()
 */
static void utc_appsvc_set_operation_func_03(void)
{
	int ret = APPSVC_RET_OK;
	ret = appsvc_set_operation(NULL, APPSVC_OPERATION_VIEW);
	if (ret == APPSVC_RET_EINVAL) {
		tet_infoline("appsvc_set_operation test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_set_operation test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	return;
}

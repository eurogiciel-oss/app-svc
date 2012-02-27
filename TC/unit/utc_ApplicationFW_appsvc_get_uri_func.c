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

static void utc_appsvc_get_uri_func_01(void);
static void utc_appsvc_get_uri_func_02(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{utc_appsvc_get_uri_func_01, POSITIVE_TC_IDX},
	{utc_appsvc_get_uri_func_02, NEGATIVE_TC_IDX},
	{NULL, 0}
};

static void startup(void)
{
	tet_infoline("Testing API appsvc_get_uri start");
}

static void cleanup(void)
{
	tet_infoline("Testing API appsvc_get_uri ends ");

}

/**
 * @brief Positive test case of appsvc_get_uri()
 */
static void utc_appsvc_get_uri_func_01(void)
{
	const char *ret_uri = NULL;
	int ret = APPSVC_RET_OK;
	bundle *b = NULL;
	b = bundle_create();
	if (b == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = appsvc_set_uri(b, "http://www.samsung.com");
	if (ret != APPSVC_RET_OK) {
		tet_infoline("Test initialization failed!!!");
		bundle_free(b);
		tet_result(TET_UNINITIATED);
		return;
	}

	ret_uri = appsvc_get_uri(b);
	if (ret_uri != NULL) {
		tet_infoline("appsvc_get_uri test PASS!");
		tet_printf("ret_uri = %s", ret_uri);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_get_uri test FAIL!");
		tet_printf("ret_uri = NULL");
		tet_result(TET_FAIL);
	}
	bundle_free(b);
	return;
}

/**
 * @brief Negative test case of appsvc_get_uri()
 */
static void utc_appsvc_get_uri_func_02(void)
{
	const char *ret_uri = NULL;
	bundle *b = NULL;
	b = bundle_create();
	if (b == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret_uri = appsvc_get_uri(b);
	if (ret_uri == NULL) {
		tet_infoline("appsvc_get_uri test PASS!");
		tet_printf("ret = NULL");
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_get_uri test FAIL!");
		tet_printf("ret_uri = %s", ret_uri);
		tet_result(TET_FAIL);
	}
	bundle_free(b);
	return;
}

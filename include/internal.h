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


#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include <unistd.h>
#include <ctype.h>
#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "APPSVC"

#define MAX_FILTER_STR_SIZE 1024
#define MAX_PACKAGE_STR_SIZE 512
#define MAX_URI_STR_SIZE 256
#define MAX_MIME_STR_SIZE 256
#define MAX_SCHEME_STR_SIZE 256
#define MAX_OP_STR_SIZE 128
#define MAX_LOCAL_BUFSZ 128

#define _E(fmt, arg...) LOGE("[%s,%d] "fmt,__FUNCTION__,__LINE__,##arg)
#define _D(fmt, arg...) LOGD("[%s,%d] "fmt,__FUNCTION__,__LINE__,##arg)

#define retvm_if(expr, val, fmt, arg...) do { \
	if(expr) { \
		_E(fmt, ##arg); \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#define retv_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#endif	/* __INTERNAL_H__ */


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



#ifndef __APP_SVC_H__
#define __APP_SVC_H__

/**
 * @file    appsvc.h
 * @version 1.1
 * @brief   This file contains APIs of the appsvc library
 */

/**
 * @addtogroup APPLICATION_FRAMEWORK
 * @{
 *
 * @defgroup appsvc Application Service
 * @version  1.1
 * @brief    Application Service library
 *
 */

/**
 * @addtogroup appsvc
 * @{
 */


#include <bundle.h>

#ifdef __cplusplus
extern "C" {
#endif


/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_DEFAULT		"http://tizen.org/appcontrol/operation/default"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_EDIT			"http://tizen.org/appcontrol/operation/edit"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_VIEW			"http://tizen.org/appcontrol/operation/view"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_PICK			"http://tizen.org/appcontrol/operation/pick"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_CREATE_CONTENT		"http://tizen.org/appcontrol/operation/create_content"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_CALL			"http://tizen.org/appcontrol/operation/call"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_SEND			"http://tizen.org/appcontrol/operation/send"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_SEND_TEXT		"http://tizen.org/appcontrol/operation/send_text"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_DIAL			"http://tizen.org/appcontrol/operation/dial"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_SEARCH			"http://tizen.org/appcontrol/operation/search"
/** APP_SVC OPERATION TYPE*/
#define APPSVC_OPERATION_DOWNLOAD		"http://tizen.org/appcontrol/operation/download"


/** APP_SVC DATA SUBJECT*/
#define APPSVC_DATA_SUBJECT		"http://tizen.org/appcontrol/data/subject"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_TO			"http://tizen.org/appcontrol/data/to"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_CC			"http://tizen.org/appcontrol/data/cc"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_BCC			"http://tizen.org/appcontrol/data/bcc"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_TEXT		"http://tizen.org/appcontrol/data/text"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_TITLE		"http://tizen.org/appcontrol/data/title"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_SELECTED		"http://tizen.org/appcontrol/data/selected"
/** APP_SVC DATA TYPE*/
#define APPSVC_DATA_KEYWORD		"http://tizen.org/appcontrol/data/keyword"

/** APP SVC internal private key */
#define APP_SVC_K_URI_R_INFO		"__APP_SVC_URI_R_INFO__"

/** APP SVC internal private value */
#define APP_SVC_V_SCHEME_AND_HOST		"__SCHEME_AND_HOST__"
/** APP SVC internal private value */
#define APP_SVC_V_SCHEME		"__SCHEME__"


/**
 * @brief Return values in appsvc. 
 */
typedef enum _appsvc_return_val {
	APPSVC_RET_ELAUNCH = -4,		/**< Failure on launching the app */
	APPSVC_RET_ENOMATCH = -3,		/**< No matching result Error */
	APPSVC_RET_EINVAL = -2,			/**< Invalid argument */
	APPSVC_RET_ERROR = -1,			/**< General error */
	APPSVC_RET_OK = 0			/**< General success */
}appsvc_return_val;


/**
 * @brief result values in appsvc. 
 */
typedef enum _appsvc_result_val {
	APPSVC_RES_CANCEL = -2,		/**< Cancel by system */
	APPSVC_RES_NOT_OK = -1,		/**< Fail by user */
	APPSVC_RES_OK = 0		/**< Success by user */
}appsvc_result_val;


/** 
 * @brief appsvc_res_fn is appsvc result function
 * @param[out]	b   		result bundle	
 * @param[out]	request_code   	request code
 * @param[out]	result   		result value
 * @param[out]	data		user-supplied data	
*/
typedef void (*appsvc_res_fn)(bundle *b, int request_code, appsvc_result_val result, void *data);


/** 
 * @brief iterator function running with appsvc_get_list 
 * @param[out]	pkg_name	package name retreived by appsvc_get_list
 * @param[out]	data		user-supplied data	
*/
typedef int (*appsvc_info_iter_fn)(const char *pkg_name, void *data);

typedef int (*appsvc_host_res_fn)(void *data);

/**
 * @par Description:
 * This function sets an operation to launch application based on appsvc.
 *
 * @param[in] b bundle object 
 * @param[in] operation operation 
 *
 * @return 0 if success, negative value(<0) if fail
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks An application must call this function before using appsvc_run_service API. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
}
 * @endcode
 *
 */
int appsvc_set_operation(bundle *b, const char *operation);

/**
 * @par Description:
 * This function sets an uri to launch application based on appsvc.
 *
 * @param[in] b bundle object 
 * @param[in] uri uri 
 *
 * @return 0 if success, negative value(<0) if fail
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
	appsvc_set_uri(b,"http://www.samsung.com");
}
 * @endcode
 *
 */
int appsvc_set_uri(bundle *b, const char *uri);

/**
 * @par Description:
 * This function sets a mime-type to launch application based on appsvc.
 *
 * @param[in] b bundle object 
 * @param[in] mime mime-type 
 *
 * @return 0 if success, negative value(<0) if fail
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_PICK);
	appsvc_set_mime(b,"image/jpg");
}
 * @endcode
 *
 */
int appsvc_set_mime(bundle *b, const char *mime);

/**
 * @par Description:
 * This function sets an extra data to launch application based on appsvc.
 *
 * @param[in] b bundle object 
 * @param[in] key key of extra data 
 * @param[in] val data 
 *
 * @return 0 if success, negative value(<0) if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_SEND);
	appsvc_set_uri(b,"mailto:xxx1@xxx");
	appsvc_add_data(b,APPSVC_DATA_CC,"xxx2@xxx");
}
 * @endcode
 *
 */
int appsvc_add_data(bundle *b, const char *key, const char *val);

/**
 * @par Description:
 * This function sets an extra array data to launch application based on appsvc.
 *
 * @param[in] b bundle object 
 * @param[in] key key of extra data 
 * @param[in] val_array data 
 * @param[in] len Length of array
 *
 * @return 0 if success, negative value(<0) if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;
	char *images[] = {"/opt/media/a.jpg", "/opt/media/b.jpg", "/opt/media/c.jpg"};

	b = bundle_create();

	appsvc_add_data_array(b, APPSVC_DATA_SELECTED, images, 3);
}
 * @endcode
 *
 */
int appsvc_add_data_array(bundle *b, const char *key, const char **val_array, int len);


/**
 * @par Description:
 * This function sets a package name to launch application based on appsvc.
 *
 * @param[in] b bundle object 
 * @param[in] pkg_name package name for explict launch
 *
 * @return 0 if success, negative value(<0) if fail
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_PICK);
	appsvc_set_mime(b,"image/jpg");
	appsvc_set_pkgname(b, "org.tizen.mygallery");
}
 * @endcode
 *
 */
int appsvc_set_pkgname(bundle *b, const char *pkg_name);


/**
 * @par Description:
 * This function sets a appid to launch application based on appsvc.
 *
 * @param[in] b bundle object
 * @param[in] appid application id for explict launch
 *
 * @return 0 if success, negative value(<0) if fail
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None.
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;

	b = bundle_create();

	appsvc_set_operation(b, APPSVC_OPERATION_PICK);
	appsvc_set_mime(b,"image/jpg");
	appsvc_set_appid(b, "org.tizen.mygallery");
}
 * @endcode
 *
 */
int appsvc_set_appid(bundle *b, const char *appid);


/**
 * @par Description:
 * This API launch application based on appsvc.
 *
 * @param[in] b bundle to be passed to callee
 * @param[in] request_code request code
 * @param[in] cbfunc result callback function
 * @param[in] data user-supplied data passed to callback function
 *
 * @return callee's pid if success, negative value(<0) if fail
 * @retval callee's pid - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 * @retval APPSVC_RET_ENOMATCH - no matching result Error 
 * @retval APPSVC_RET_ELAUNCH - failure on launching the app
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	bundle *b = NULL;
	static int num = 0;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_PICK);
	appsvc_set_mime(b,"image/jpg");

	return appsvc_run_service(b, 0, cb_func, (void*)NULL);
}
 * @endcode
 *
 */
int appsvc_run_service(bundle *b, int request_code, appsvc_res_fn cbfunc, void *data);

/**
 * @par Description:
 * This API use to get application list that is matched with given bundle.
 *
 * @param[in] b bundle to resolve application
 * @param[in] iter_fn iterator function
 * @param[in] data user-supplied data for iter_fn
 *
 * @return 0 if success, negative value(<0) if fail
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 * @retval APPSVC_RET_ENOMATCH - no matching result Error 
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

static int iter_fn(const char* pkg_name, void *data)
{
	printf("\t==========================\n");
 	printf("\t pkg_name: %s\n", pkg_name);
 	printf("\t==========================\n");
 	return 0;
}

...
{
	bundle *b = NULL;
	static int num = 0;

	b = bundle_create();
	
	appsvc_set_operation(b, APPSVC_OPERATION_PICK);
	appsvc_set_mime(b,"image/jpg");

	return appsvc_get_list(b, iter_fn, (void*)NULL);
}
 * @endcode
 *
 */
int appsvc_get_list(bundle *b, appsvc_info_iter_fn iter_fn, void *data);

/**
 * @par Description:
 * This function gets a operation from bundle.
 *
 * @param[in] b bundle object 
 *
 * @return Pointer for operation string if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char *val;	
	val = appsvc_get_operation(b);
}
 * @endcode
 *
 */
const char *appsvc_get_operation(bundle *b);

/**
 * @par Description:
 * This function gets a uri from bundle.
 *
 * @param[in] b bundle object 
 *
 * @return Pointer for uri string if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char *val;	
	val = appsvc_get_uri(b);
}
 * @endcode
 *
 */
const char *appsvc_get_uri(bundle *b);

/**
 * @par Description:
 * This function gets a mime-type from bundle.
 *
 * @param[in] b bundle object 
 *
 * @return Pointer for mime-type string if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char *val;	
	val = appsvc_get_mime(b);
}
 * @endcode
 *
 */
const char *appsvc_get_mime(bundle *b);

/**
 * @par Description:
 * This function gets a package name from bundle.
 *
 * @param[in] b bundle object 
 *
 * @return Pointer for package name string if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char *val;	
	val = appsvc_get_pkgname(b);
}
 * @endcode
 *
 */
const char *appsvc_get_pkgname(bundle *b);

/**
 * @par Description:
 * This function gets a application id from bundle.
 *
 * @param[in] b bundle object
 *
 * @return Pointer for application id string if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None.
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char *val;
	val = appsvc_get_appid(b);
}
 * @endcode
 *
 */
const char *appsvc_get_appid(bundle *b);

/**
 * @par Description:
 * This function gets value from key.
 *
 * @param[in] b bundle object 
 * @param[in] key key
 *
 * @return Pointer for value string if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char *val;	
	val = appsvc_get_data(b, APPSVC_DATA_CC);
}
 * @endcode
 *
 */
const char *appsvc_get_data(bundle *b, const char *key);

/**
 * @par Description:
 * This function gets value from key.
 *
 * @param[in] b bundle object 
 * @param[in] key key
 * @param[out] len length of array
 *
 * @return Pointer for value string array if success, NULL if fail
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	char **val_array;	
	int len;
	char *val;

	if(appsvc_data_is_array(b, APPSVC_DATA_SELECTED))
		val_array = appsvc_get_data_array(b, APPSVC_DATA_SELECTED, &len);
	else 
		val = appsvc_get_data(b, APPSVC_DATA_SELECTED);
}
 * @endcode
 *
 */
const char **appsvc_get_data_array(bundle *b, const char *key, int *len);

/**
 * @par Description:
 * This API create appsvc result bundle based on bundle received in reset event.
 *
 * @param[in] inb bundle received in reset event 
 * @param[in] outb bundle to use for returning result 
 *
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see appsvc_send_result.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	struct appdata *ad = data;
	bundle* res_bundle;
	
	appsvc_create_result_bundle(ad->b,&res_bundle);
	bundle_add(res_bundle, "result", "1");
	appsvc_send_result(res_bundle, 0);
}
 * @endcode
 *
 */
int appsvc_create_result_bundle(bundle *inb, bundle **outb);

/**
 * @par Description:
 * This API send appsvc result to caller with bundle.
 *
 * @param[in] b Result data in bundle format
 * @param[in] result result value
 *
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre appsvc_create_result_bundle.
 * @post None.
 * @see appsvc_send_result.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	struct appdata *ad = data;
	bundle* res_bundle;
	
	appsvc_create_result_bundle(ad->b,&res_bundle);
	bundle_add(res_bundle, "result", "1");
	appsvc_send_result(res_bundle, 0);
}
 * @endcode
 *
 */
int appsvc_send_result(bundle *b, appsvc_result_val result);

/**
 * @par Description:
 * This API set the default application(package name) associated with op, uri and mime-type.
 *
 * @param[in] op		operation
 * @param[in] mime_type	mime-type
 * @param[in] scheme 	scheme of uri
 * @param[in] defapp 	default application
 *
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 * @retval APPSVC_RET_EINVAL - invalid argument(content)
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	appsvc_set_defapp(APPSVC_OPERATION_VIEW, NULL,"http", "org.tizen.mybrowser");
}
 * @endcode
 *
 */
int appsvc_set_defapp(const char *op, const char *mime_type, const char *uri,
				const char *defapp);

/**
 * @par Description:
 * This API unset the default application(package name) associated with op, uri and mime-type.
 *
 * @param[in] defapp 	default application
 *
 * @retval APPSVC_RET_OK - success
 * @retval APPSVC_RET_ERROR - general error 
 *
 * @pre None.
 * @post None.
 * @see None.
 * @remarks None. 
 *
 * @par Sample code:
 * @code
#include <appsvc.h>

...
{
	appsvc_unset_defapp("org.tizen.test");
}
 * @endcode
 *
 */
int appsvc_unset_defapp(const char *defapp);

/**
 * @par Description:
 *	This API ask a application is default application or not.
 *
 * @param[in]	pkg_name	application package name
 * @return	true / false
 * @retval	1	app_name is default application in appsvc.
 * @retval	0	app_name is NOT default application in appsvc.
 *
  * @pre None.
  * @post None.
  * @see None.
  * @remarks None. 
  *
  * @par Sample code:
  * @code
#include <appsvc.h>
 
 ...

 * int is_defapp_browser_app()
 * { 
 *      return appsvc_is_defapp("org.tizen.browser");
 * }
 *
 * @endcode
 * @remark
 *	None
* 
*/
int appsvc_is_defapp(const char *pkg_name);


/**
 * @par Description:
 *	This API ask a extra data is array or not.
 *
 * @param[in] b bundle object 
 * @param[in] key key of extra data 
 * @return	true / false
 * @retval	1	a extra data is array.
 * @retval	0	a extra data is not array.
 *
  * @pre None.
  * @post None.
  * @see None.
  * @remarks None. 
  *
  * @par Sample code:
  * @code
#include <appsvc.h>
 
 ...

 * int is_defapp_browser_app(bundle *b, char *key)
 * { 
 *      return appsvc_data_is_array(b, key);
 * }
 *
 * @endcode
 * @remark
 *	None
* 
*/
int appsvc_data_is_array(bundle *b, const char *key);


#ifdef __cplusplus
	}
#endif

/**
 * @}
 */
/**
 * @}
 */


#endif		/* __APP_SVC_H__ */

/* vi: set ts=8 sts=8 sw=8: */

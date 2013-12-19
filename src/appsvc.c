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


#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include <ail.h>
#include <aul.h>
#include <libsoup/soup.h>

#ifndef WAYLAND
#include <Ecore_X.h>
#endif

#include <Ecore.h>
#include <iniparser.h>
#include <pkgmgr-info.h>

#include "appsvc.h"
#include "appsvc_db.h"
#include "internal.h"
#include "priv_key.h"




#ifndef SLPAPI
#define SLPAPI __attribute__ ((visibility("default")))
#endif


/* callback handling */
typedef struct _appsvc_cb_info_t{
	appsvc_res_fn cb_func;
	int request_code;
	void *data;
}appsvc_cb_info_t;

typedef struct _appsvc_resolve_info_t{
	char *pkgname;
	char *op;
	char *uri;
	char *scheme;
	char *host;
	char *uri_r_info;
	char *origin_mime;
	char *mime;	
	char *m_type;
	char *s_type;
	char *category;
	char *win_id;
	int mime_set;
}appsvc_resolve_info_t;

extern int aul_launch_app_with_result(const char *pkgname, bundle *kb,
			       void (*cbfunc) (bundle *, int, void *),
			       void *data);

static appsvc_cb_info_t *__create_rescb(int request_code, appsvc_res_fn cbfunc,
					void *data);
static void __remove_rescb(appsvc_cb_info_t *info);
static int __set_bundle(bundle *b, const char *key, const char *value);
static void __aul_cb(bundle *b, int is_cancel, void *data);
static int __run_svc_with_pkgname(char *pkgname, bundle *b, int request_code,
				  appsvc_res_fn cbfunc, void *data);
static int __get_resolve_info(bundle *b, appsvc_resolve_info_t *info);
static int __free_resolve_info_data(appsvc_resolve_info_t *info);
static ail_cb_ret_e __ail_iter_func(const ail_appinfo_h appinfo,
				    void *user_data);
static int __get_list_with_condition(char *op, char *uri,
				     char *mime, GSList **pkg_list);


static appsvc_cb_info_t *__create_rescb(int request_code, appsvc_res_fn cbfunc, void *data)
{
	appsvc_cb_info_t* info;

	info = (appsvc_cb_info_t*)calloc(1, sizeof(appsvc_cb_info_t));
	if(info == NULL)
		return NULL;

	info->request_code = request_code;
	info->cb_func = cbfunc;
	info->data = data;

	return info;
}

static void __remove_rescb(appsvc_cb_info_t *info)
{
	if(info) free(info);
}

static int __set_bundle(bundle *b, const char *key, const char *value)
{
	const char *val = NULL;

	val = bundle_get_val(b, key);
	if(val){
		if( bundle_del(b, key) != 0 ){
			return APPSVC_RET_ERROR;
		}
	}

	if(!value) 
		return APPSVC_RET_EINVAL;
		
	if( bundle_add(b, key, value) != 0 ){
		return APPSVC_RET_ERROR;
	}

	_D("__set_bundle");
//	bundle_iterate(b, __bundle_iterate, NULL);

	return APPSVC_RET_OK;
}

static int __set_bundle_array(bundle *b, const char *key, const char **value, int len)
{

	int type;
	type = appsvc_data_is_array(b, key);

	if(type == 1) {
		if( bundle_del(b, key) != 0 ){
			return APPSVC_RET_ERROR;
		}
	}

	if(!value) 
		return APPSVC_RET_EINVAL;

	if( bundle_add_str_array(b, key, value, len) != 0 ){
		return APPSVC_RET_ERROR;
	}

	_D("__set_bundle_array");
//	bundle_iterate(b, __bundle_iterate, NULL);

	return APPSVC_RET_OK;
}


static void __aul_cb(bundle *b, int is_cancel, void *data)
{
	const char *val = NULL;
	appsvc_cb_info_t*  cb_info;
	int res;

	if(is_cancel)
		res = APPSVC_RES_CANCEL;
	else{
		/* get result_code from bundle */
		val = bundle_get_val(b, APP_SVC_K_RES_VAL);
		res = (val==NULL)? APPSVC_RES_NOT_OK : atoi(val);
	}

	/* remove result_code from bundle */
	bundle_del(b, APP_SVC_K_RES_VAL);

	/* find corresponding callback */
	cb_info = (appsvc_cb_info_t*)data;
	
	cb_info->cb_func(b, cb_info->request_code, (appsvc_result_val)res, cb_info->data);
	__remove_rescb(cb_info);
	

	return;
}

static int __run_svc_with_pkgname(char *pkgname, bundle *b, int request_code, appsvc_res_fn cbfunc, void *data)
{
	appsvc_cb_info_t *cb_info = NULL;
	int ret = -1;
	
	if (cbfunc) {
		_D("pkg_name : %s - with result", pkgname);

		cb_info = __create_rescb(request_code, cbfunc, data);
		ret = aul_launch_app_with_result(pkgname, b, __aul_cb, cb_info);
	} else {
		_D("pkg_name : %s - no result", pkgname);
		ret = aul_launch_app(pkgname, b);
	}

	if(ret < 0) {
		switch (ret) {
			case AUL_R_EILLACC:
				ret = APPSVC_RET_EILLACC;
				break;
			case AUL_R_EINVAL:
				ret = APPSVC_RET_EINVAL;
				break;
			default:
				ret = APPSVC_RET_ELAUNCH;
		}
	}

	return ret;
}

static int __get_resolve_info(bundle *b, appsvc_resolve_info_t *info)
{
	char *tmp = NULL;
	char *strtok_buf = NULL;
	SoupURI *s_uri;
	
	info->op = (char *)appsvc_get_operation(b);
	info->uri = (char *)appsvc_get_uri(b);
	info->origin_mime = info->mime = (char *)appsvc_get_mime(b);
	info->pkgname = (char *)appsvc_get_pkgname(b);
	info->category = (char *)appsvc_get_category(b);
	info->win_id = (char *)bundle_get_val(b, APP_SVC_K_WIN_ID);

	if(info->uri) {
		if(strncmp(info->uri,"/",1) == 0){
			if(!info->mime) {
				info->origin_mime = info->mime = malloc(MAX_MIME_STR_SIZE);
				aul_get_mime_from_file(info->uri, info->mime, MAX_MIME_STR_SIZE);
				info->mime_set = 1;
			}
			info->uri = NULL;			
		} else if(strncmp(info->uri,"file:/",6)==0){
			if(!info->mime) {
				info->origin_mime = info->mime = malloc(MAX_MIME_STR_SIZE);
				aul_get_mime_from_file(&info->uri[5], info->mime, MAX_MIME_STR_SIZE);
				info->mime_set = 1;
			}
			info->uri = NULL;
		} else if(strncmp(info->uri,"file:///",8) == 0){
			if(!info->mime) {
				info->origin_mime = info->mime = malloc(MAX_MIME_STR_SIZE);
				aul_get_mime_from_file(&info->uri[7], info->mime, MAX_MIME_STR_SIZE);	
				info->mime_set = 1;
			}
			info->uri = NULL;
		}
	}

	if(info->uri) {
		s_uri = soup_uri_new(info->uri);

		if(!s_uri)
			return APPSVC_RET_EINVAL;

		if(s_uri->scheme) {
			info->scheme = malloc(MAX_SCHEME_STR_SIZE);
			strncpy(info->scheme, s_uri->scheme, MAX_SCHEME_STR_SIZE-1);
		}
		if(s_uri->host) {
			info->host = malloc(MAX_HOST_STR_SIZE);
			strncpy(info->host, s_uri->host, MAX_HOST_STR_SIZE-1);
		}
		if(info->scheme && info->host) {
			info->uri_r_info = malloc(MAX_SCHEME_STR_SIZE+MAX_HOST_STR_SIZE+2);
			snprintf(info->uri_r_info, MAX_SCHEME_STR_SIZE+MAX_HOST_STR_SIZE+1,
				"%s://%s", info->scheme, info->host);
		}

		soup_uri_free(s_uri);
	} else {
		info->scheme = strdup("NULL");
	}

	if(!info->mime) 
		info->mime = strdup("NULL");
	else {
		info->m_type = malloc(MAX_LOCAL_BUFSZ);
		info->s_type = malloc(MAX_LOCAL_BUFSZ);
		tmp = strdup(info->mime);
		strtok_buf = strtok(tmp,"/");
		if(strtok_buf)
			strncpy(info->m_type, strtok_buf, MAX_LOCAL_BUFSZ-1);
		strtok_buf = strtok(NULL,"/");
		if(strtok_buf)
			strncpy(info->s_type, strtok_buf, MAX_LOCAL_BUFSZ-1);
		free(tmp);

		if(strncmp(info->m_type, "*", 1) == 0) {
			strncpy(info->m_type, "%", MAX_LOCAL_BUFSZ-1);
		}
		if(strncmp(info->s_type, "*", 1) == 0) {
			strncpy(info->s_type, "%", MAX_LOCAL_BUFSZ-1);	
		}

		info->mime = malloc(MAX_MIME_STR_SIZE);
		snprintf(info->mime, MAX_MIME_STR_SIZE-1, "%s/%s", info->m_type, info->s_type);
	}

	return 0;
}

static int __free_resolve_info_data(appsvc_resolve_info_t *info)
{
	if (info->mime) 
		free(info->mime);
	if (info->scheme) 
		free(info->scheme);
	if (info->host)
		free(info->host);
	if (info->m_type)
		free(info->m_type);
	if (info->s_type)
		free(info->s_type);
	if (info->uri_r_info)
		free(info->uri_r_info);
	if (info->mime_set)
		free(info->origin_mime);
	
	return 0;
}

static ail_cb_ret_e __ail_iter_func(
			const ail_appinfo_h appinfo, void *user_data)
{
	GSList **pkg_list = (GSList **)user_data;
	GSList *iter = NULL;
	char *str = NULL;
	char *pkgname = NULL;

	ail_appinfo_get_str(appinfo, AIL_PROP_PACKAGE_STR, &str);

	_D("Matching application is %s",str);

	for (iter = *pkg_list; iter != NULL; iter = g_slist_next(iter)) {
		pkgname = (char *)iter->data;
		if (strncmp(str, pkgname, MAX_PACKAGE_STR_SIZE-1) == 0)
			return AIL_CB_RET_CONTINUE;
	}

	pkgname = strdup(str);
	*pkg_list = g_slist_append(*pkg_list, (void *)pkgname);
	_D("%s is added",pkgname);

	return AIL_CB_RET_CONTINUE;
}

static int __get_list_with_condition(char *op, char *uri, char *mime, GSList **pkg_list)
{
	ail_filter_h filter;
	ail_error_e ail_ret;
	char svc_filter[MAX_FILTER_STR_SIZE] = {0,};
	
	ail_ret = ail_filter_new(&filter);
	if (ail_ret != AIL_ERROR_OK) 
		return APPSVC_RET_ERROR;

	snprintf(svc_filter, MAX_FILTER_STR_SIZE-1, "%s|%s|%s", op, uri, mime);
	_D("svc_filter : %s",svc_filter);

	ail_ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_SVC_STR, svc_filter);
	if (ail_ret != AIL_ERROR_OK) {
		ail_filter_destroy(filter);
		return APPSVC_RET_ERROR;
	}

	ail_filter_list_appinfo_foreach(filter, __ail_iter_func, (void *)pkg_list);

	ail_filter_destroy(filter);

	return APPSVC_RET_OK;
}

SLPAPI int appsvc_set_operation(bundle *b, const char *operation)
{	
	if(b == NULL){
		_E("bundle for appsvc_set_operation is NULL");
		return APPSVC_RET_EINVAL;
	}

	return __set_bundle(b, APP_SVC_K_OPERATION, operation);
}

SLPAPI int appsvc_set_uri(bundle *b, const char *uri)
{	
	if(b == NULL){
		_E("bundle for appsvc_set_uri is NULL");
		return APPSVC_RET_EINVAL;
	}
	
	return __set_bundle(b, APP_SVC_K_URI, uri);
}

SLPAPI int appsvc_set_mime(bundle *b, const char *mime)
{	
	if(b == NULL){
		_E("bundle for appsvc_set_mime is NULL");
		return APPSVC_RET_EINVAL;
	}

	return __set_bundle(b, APP_SVC_K_MIME, mime);
}

SLPAPI int appsvc_add_data(bundle *b, const char *key, const char *val)
{
	if(b == NULL || key == NULL) {
		return APPSVC_RET_EINVAL;
	}

	/* check key for data */
	/******************/

	return __set_bundle(b, key, val);
}

SLPAPI int appsvc_add_data_array(bundle *b, const char *key,  const char **val_array, int len)
{
	if(b == NULL || key == NULL) {
		return APPSVC_RET_EINVAL;
	}

	/* check key for data */
	/******************/

	return __set_bundle_array(b, key, val_array, len);
}


SLPAPI int appsvc_set_pkgname(bundle *b, const char *pkg_name)
{	
	if(b == NULL){
		_E("bundle for appsvc_set_pkgname is NULL");
		return APPSVC_RET_EINVAL;
	}

	return __set_bundle(b, APP_SVC_K_PKG_NAME, pkg_name);
}

static char* __get_alias_appid(char *appid)
{
	char *alias_id = NULL;
	char *val = NULL;
	char key_string[MAX_PACKAGE_STR_SIZE+5];
	dictionary *dic;

	dic = iniparser_load("/usr/share/appsvc/alias.ini");

	if(dic == NULL)
		return NULL;

	sprintf(key_string, "Alias:%s", appid);
	val = iniparser_getstr(dic, key_string);

	_D("alias_id : %s", val);

	if(val != NULL) {
		alias_id = malloc(MAX_PACKAGE_STR_SIZE);
		strncpy(alias_id, val, MAX_PACKAGE_STR_SIZE-1);
	}

	iniparser_freedict(dic);

	return alias_id;
}

SLPAPI int appsvc_set_appid(bundle *b, const char *appid)
{
	char *alias_id = NULL;
	int ret;

	if(b == NULL || appid == NULL){
		_E("bundle for appsvc_set_appid is NULL");
		return APPSVC_RET_EINVAL;
	}

	alias_id = __get_alias_appid(appid);
	if(alias_id == NULL) {
		ret = __set_bundle(b, APP_SVC_K_PKG_NAME, appid);
	} else {
		ret = __set_bundle(b, APP_SVC_K_PKG_NAME, alias_id);
		free(alias_id);
	}

	return ret;
}

SLPAPI int appsvc_set_category(bundle *b, const char *category)
{
	if(b == NULL){
		_E("bundle for appsvc_set_category is NULL");
		return APPSVC_RET_EINVAL;
	}

	return __set_bundle(b, APP_SVC_K_CATEGORY, category);
}

static int __get_list_with_condition_mime_extened(char *op, char *uri, char *mime,
	char *m_type, char *s_type, GSList **pkg_list)
{
	char *tmp;

	tmp = malloc(MAX_MIME_STR_SIZE);

	__get_list_with_condition(op, uri, mime, pkg_list);
	if ((strncmp(mime, "NULL", 4) != 0) && (strncmp(s_type, "%", 1) != 0)) {
		snprintf(tmp, MAX_MIME_STR_SIZE-1, "%s/*", m_type);
		__get_list_with_condition(op, uri, tmp, pkg_list);
	}
	if ((strncmp(mime, "NULL", 4) != 0) && (strncmp(m_type, "%", 1) != 0)) {
		snprintf(tmp, MAX_MIME_STR_SIZE-1, "*/*");
		__get_list_with_condition(op, uri, tmp, pkg_list);
	}
	free(tmp);

	return 0;
}

static int __get_list_with_condition_mime_extened_with_collation(char *op, char *uri, char *mime,
	char *m_type, char *s_type, GSList **pkg_list)
{
	char *tmp;

	tmp = malloc(MAX_MIME_STR_SIZE);

	_svc_db_get_list_with_collation(op, uri, mime, pkg_list);
	if ((strncmp(mime, "NULL", 4) != 0) && (strncmp(s_type, "%", 1) != 0)) {
		snprintf(tmp, MAX_MIME_STR_SIZE-1, "%s/*", m_type);
		_svc_db_get_list_with_collation(op, uri, tmp, pkg_list);
	}
	if ((strncmp(mime, "NULL", 4) != 0) && (strncmp(m_type, "%", 1) != 0)) {
		snprintf(tmp, MAX_MIME_STR_SIZE-1, "*/*");
		_svc_db_get_list_with_collation(op, uri, tmp, pkg_list);
	}
	free(tmp);

	return 0;
}

GSList *tmp_list;
static int __app_list_cb(pkgmgrinfo_appinfo_h handle, void *user_data)
{
	char *appid = NULL;
	GSList **app_list = (GSList **)user_data;
	char *str = NULL;
	GSList *iter = NULL;

	pkgmgrinfo_appinfo_get_appid(handle, &str);
	_D("Matching application is %s",str);

	for (iter = tmp_list; iter != NULL; iter = g_slist_next(iter)) {
		if (strncmp(str, (char *)iter->data, MAX_PACKAGE_STR_SIZE-1) == 0) {
			appid = strdup(str);
			*app_list = g_slist_append(*app_list, (void *)appid);
			_D("%s is added",appid);
		}
	}

	return 0;
}

static int __get_list_with_category(char *category, GSList **pkg_list)
{
	int ret;
	pkgmgrinfo_appinfo_filter_h handle;
	GSList *app_list = NULL;
	GSList *iter = NULL;
	char *list_item = NULL;

	ret = pkgmgrinfo_appinfo_filter_create(&handle);
	ret = pkgmgrinfo_appinfo_filter_add_string(handle, PMINFO_APPINFO_PROP_APP_CATEGORY, category);

	tmp_list = *pkg_list;
	ret = pkgmgrinfo_appinfo_filter_foreach_appinfo(handle, __app_list_cb, &app_list);
	if (ret != PMINFO_R_OK) {
		pkgmgrinfo_appinfo_filter_destroy(handle);
		return -1;
	}
	pkgmgrinfo_appinfo_filter_destroy(handle);

	for (iter = *pkg_list; iter != NULL; iter = g_slist_next(iter)) {
		list_item = (char *)iter->data;
		g_free(list_item);
	}
	g_slist_free(*pkg_list);

	*pkg_list = app_list;

	return 0;
}

static int __appid_compare(gconstpointer data1, gconstpointer data2)
{
	char *a = (char *)data1;
	char *b = (char *)data2;
	return strcmp(a,b);
}

static int __get_list_with_submode(char *win_id, GSList **pkg_list)
{
	int ret;
	GSList *iter = NULL;
	char *appid = NULL;
	GSList *find_item = NULL;
	char *find_appid = NULL;
	ail_appinfo_h handle;
	char *submode_mainid = NULL;

#ifndef WAYLAND
	for (iter = *pkg_list; iter != NULL; ) {
		find_item = NULL;
		submode_mainid = NULL;
		appid = (char *)iter->data;
		ret = ail_get_appinfo(appid, &handle);
		SECURE_LOGD("ret %d, %s, %x", ret, appid, handle);
		ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_SUBMODEMAINID_STR, &submode_mainid);
		SECURE_LOGD("appid(%s) submode_mainid(%s) win_id(%s)", appid, submode_mainid, win_id);
		if(submode_mainid) {
			if(win_id) {
				find_item = g_slist_find_custom(*pkg_list, submode_mainid, __appid_compare);
				if(find_item) {
					find_appid = find_item->data;
					if(find_item == g_slist_next(iter)) {
						iter = g_slist_next(find_item);
						*pkg_list = g_slist_remove(*pkg_list, find_appid);
						free(find_appid);
					} else {
						iter = g_slist_next(iter);
						*pkg_list = g_slist_remove(*pkg_list, find_appid);
						free(find_appid);
					}
				}
			} else {
				find_item = g_slist_find_custom(*pkg_list, submode_mainid, __appid_compare);
				if(find_item) {
					iter = g_slist_next(iter);
					*pkg_list = g_slist_remove(*pkg_list, appid);
					free(appid);
				}
			}
		}
		ail_destroy_appinfo(handle);
		if(!find_item) {
			iter = g_slist_next(iter);
		}
	}
#endif

	for (iter = *pkg_list; iter != NULL; iter = g_slist_next(iter)) {
		appid = (char *)iter->data;
		SECURE_LOGD("appid(%s)", appid);
	}

	return 0;
}

SLPAPI int appsvc_run_service(bundle *b, int request_code, appsvc_res_fn cbfunc, void *data)
{
	appsvc_resolve_info_t info;
	char *pkgname;
	int pkg_count = 0;
	int ret = -1;

	GSList *pkg_list = NULL;
	GSList *iter = NULL;
	char *list_item;

	if(b == NULL){
		_E("bundle for appsvc_set_appid is NULL");
		return APPSVC_RET_EINVAL;
	}

	pkgname = (char *)appsvc_get_pkgname(b);

	/* explict*/
	if(pkgname) {
		if(appsvc_get_operation(b) == NULL)
			appsvc_set_operation(b,APPSVC_OPERATION_DEFAULT);
		ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
		return ret;
	}

	memset(&info, 0, sizeof(appsvc_resolve_info_t));
	ret = __get_resolve_info(b, &info);
	if(ret < 0)
		return ret;

	_D("op - %s / mime - %s / shceme - %s\n", info.op, info.origin_mime, info.scheme);

	/*uri*/
	pkgname = _svc_db_get_app(info.op, info.origin_mime, info.uri);
	if(pkgname==NULL){
		__get_list_with_condition_mime_extened_with_collation(info.op, info.uri,
				info.mime, info.m_type, info.s_type, &pkg_list);
		pkg_count = g_slist_length(pkg_list);
		if(pkg_count > 0) {

			if(info.uri_r_info) {
				__get_list_with_condition_mime_extened(info.op, info.uri_r_info,
					info.mime, info.m_type, info.s_type, &pkg_list);
			}

			__get_list_with_condition_mime_extened(info.op, info.scheme,
				info.mime, info.m_type, info.s_type, &pkg_list);

			__get_list_with_condition_mime_extened(info.op, "*",
				info.mime, info.m_type, info.s_type, &pkg_list);

			if(info.category) {
				__get_list_with_category(info.category, &pkg_list);
			}

			__get_list_with_submode(info.win_id, &pkg_list);

			pkg_count = g_slist_length(pkg_list);
			_D("pkg_count : %d", pkg_count);

			if(pkg_count == 1){
				pkgname = (char *)pkg_list->data;
				if(pkgname != NULL){
					ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
					goto end;
				}
			} else {
				bundle_add(b, APP_SVC_K_URI_R_INFO, info.uri);
				ret = __run_svc_with_pkgname(APP_SELECTOR, b, request_code, cbfunc, data);
				goto end;
			}
			for (iter = pkg_list; iter != NULL; iter = g_slist_next(iter)) {
				list_item = (char *)iter->data;
				g_free(list_item);
			}
			g_slist_free(pkg_list);
			pkg_list = NULL;
		}
	} else {
		ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
		free(pkgname);
		goto end;
	}

	/*scheme & host*/
	if(info.uri_r_info) {
		pkgname = _svc_db_get_app(info.op, info.origin_mime, info.uri_r_info);

		if(pkgname==NULL){
			__get_list_with_condition_mime_extened(info.op, info.uri_r_info,
				info.mime, info.m_type, info.s_type, &pkg_list);
			pkg_count = g_slist_length(pkg_list);
			if(pkg_count > 0) {
				__get_list_with_condition_mime_extened(info.op, info.scheme,
					info.mime, info.m_type, info.s_type, &pkg_list);

				__get_list_with_condition_mime_extened(info.op, "*",
								info.mime, info.m_type, info.s_type, &pkg_list);

				if(info.category) {
					__get_list_with_category(info.category, &pkg_list);
				}

				__get_list_with_submode(info.win_id, &pkg_list);

				pkg_count = g_slist_length(pkg_list);
				_D("pkg_count : %d", pkg_count);

				if(pkg_count == 1){
					pkgname = (char *)pkg_list->data;
					if(pkgname != NULL){
						ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
						goto end;
					}
				} else {
					bundle_add(b, APP_SVC_K_URI_R_INFO, info.uri_r_info);
					ret = __run_svc_with_pkgname(APP_SELECTOR, b, request_code, cbfunc, data);
					goto end;
				}
			}
			for (iter = pkg_list; iter != NULL; iter = g_slist_next(iter)) {
				list_item = (char *)iter->data;
				g_free(list_item);
			}
			g_slist_free(pkg_list);
			pkg_list = NULL;
		}  else {
			ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
			free(pkgname);
			goto end;
		}
	}

	/*scheme*/
	pkgname = _svc_db_get_app(info.op, info.origin_mime, info.scheme);

	if(pkgname==NULL){
		__get_list_with_condition_mime_extened(info.op, info.scheme,
			info.mime, info.m_type, info.s_type, &pkg_list);

		__get_list_with_condition_mime_extened(info.op, "*",
								info.mime, info.m_type, info.s_type, &pkg_list);

		if(info.category) {
			__get_list_with_category(info.category, &pkg_list);
		}

		__get_list_with_submode(info.win_id, &pkg_list);

		pkg_count = g_slist_length(pkg_list);
		_D("pkg_count : %d", pkg_count);

		if(pkg_count == 1){
			pkgname = (char *)pkg_list->data;
			if(pkgname != NULL){
				ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
			}
		} else if(pkg_count < 1) {
			__free_resolve_info_data(&info);
			return APPSVC_RET_ENOMATCH;
		} else {
			bundle_add(b, APP_SVC_K_URI_R_INFO, info.scheme);
			ret = __run_svc_with_pkgname(APP_SELECTOR, b, request_code, cbfunc, data);
		}

		for (iter = pkg_list; iter != NULL; iter = g_slist_next(iter)) {
			list_item = (char *)iter->data;
			g_free(list_item);
		}
		g_slist_free(pkg_list);	
	} else {
		ret = __run_svc_with_pkgname(pkgname, b, request_code, cbfunc, data);
		free(pkgname);
	}

end:
	__free_resolve_info_data(&info);

	return ret;
}

SLPAPI int appsvc_get_list(bundle *b, appsvc_info_iter_fn iter_fn, void *data)
{
	appsvc_resolve_info_t info;
	char *pkgname = NULL;
	int pkg_count;
	int ret = -1;

	GSList *pkg_list = NULL;
	GSList *iter = NULL;

	if(b == NULL){
		_E("bundle for appsvc_run_service is NULL");
		return APPSVC_RET_EINVAL;
	}
	
	if (iter_fn == NULL){
		_E("iter_fn for appsvc_run_service is NULL");
		return APPSVC_RET_EINVAL;
	}
	
	/* parse bundle */
	memset(&info, 0, sizeof(appsvc_resolve_info_t));
	ret = __get_resolve_info(b,&info);
	if(ret < 0)
		return ret;
	
	_D("operation - %s / shceme - %s / mime - %s\n", info.op, info.scheme, info.mime);

	__get_list_with_condition_mime_extened_with_collation(info.op, info.uri,
			info.mime, info.m_type, info.s_type, &pkg_list);

	if(info.uri_r_info) {
		__get_list_with_condition_mime_extened(info.op, info.uri_r_info, 
			info.mime, info.m_type, info.s_type, &pkg_list);
	}
	
	__get_list_with_condition_mime_extened(info.op, info.scheme, 
		info.mime, info.m_type, info.s_type, &pkg_list);

	if(info.category) {
		__get_list_with_category(info.category, &pkg_list);
	}

	__get_list_with_submode(info.win_id, &pkg_list);

	pkg_count = g_slist_length(pkg_list);
	if (pkg_count == 0) {
		_E("Cannot find associated application");
		return APPSVC_RET_ENOMATCH;
	}

	for (iter = pkg_list; iter != NULL; iter = g_slist_next(iter)) {
		pkgname = iter->data;
		_D("PKGNAME : %s\n", pkgname);
		if( iter_fn(pkgname,data) != 0)
			break;
		g_free(pkgname);
	}

	g_slist_free(pkg_list);
	__free_resolve_info_data(&info);

	return APPSVC_RET_OK;	
}

SLPAPI const char *appsvc_get_operation(bundle *b)
{
	return bundle_get_val(b, APP_SVC_K_OPERATION);
}

SLPAPI const char *appsvc_get_uri(bundle *b)
{
	return bundle_get_val(b, APP_SVC_K_URI);
}

SLPAPI const char *appsvc_get_mime(bundle *b)
{
	return bundle_get_val(b, APP_SVC_K_MIME);
}

SLPAPI const char *appsvc_get_data(bundle *b, const char *key)
{
	return bundle_get_val(b, key);
}

SLPAPI const char **appsvc_get_data_array(bundle *b, const char *key, int *len)
{
	return bundle_get_str_array(b, key, len);
}

SLPAPI const char *appsvc_get_pkgname(bundle *b)
{
	return bundle_get_val(b, APP_SVC_K_PKG_NAME);
}

SLPAPI const char *appsvc_get_appid(bundle *b)
{
	return bundle_get_val(b, APP_SVC_K_PKG_NAME);
}

SLPAPI const char *appsvc_get_category(bundle *b)
{
	return bundle_get_val(b, APP_SVC_K_CATEGORY);
}

SLPAPI int appsvc_create_result_bundle(bundle *inb, bundle **outb)
{
	int ret = -1;

	if(inb == NULL || outb == NULL) {
		_E("bundle is NULL");
		return APPSVC_RET_EINVAL;
	}
	
	ret = aul_create_result_bundle(inb, outb);

	/* add additional bundle */
	/*	bundle_add(outb, " ", " ");  */

	if(ret == AUL_R_OK)
		ret = APPSVC_RET_OK;
	else if(ret == AUL_R_EINVAL)
		ret = APPSVC_RET_EINVAL;
	else
		ret = APPSVC_RET_ERROR;

	return ret;
}

SLPAPI int appsvc_send_result(bundle *b, appsvc_result_val result)
{
	int ret;
	char tmp[MAX_LOCAL_BUFSZ];
	
	if(b == NULL){
		_E("appsvc_send_result is NULL");
		return APPSVC_RET_EINVAL;
	}

	if(result != APPSVC_RES_OK && result != APPSVC_RES_NOT_OK){
		_E("invalid result %d", (int)result);
		return APPSVC_RET_EINVAL;
	}

	/* add result_code to bundle */
	snprintf(tmp,MAX_LOCAL_BUFSZ,"%d", (int)result);
	ret = __set_bundle(b, APP_SVC_K_RES_VAL, tmp);
	if(ret < 0)
		return APPSVC_RET_ERROR;

	ret = aul_send_service_result(b);

	/* remove result_code from bundle */
	bundle_del(b, APP_SVC_K_RES_VAL);

	return ret;
} 

SLPAPI int appsvc_set_defapp(const char *op, const char *mime_type, const char *uri,
					const char *defapp)
{
	int ret;

	if(op == NULL || defapp == NULL)
		return APPSVC_RET_EINVAL;

	ret = _svc_db_add_app(op, mime_type, uri, defapp);

	if(ret < 0)
		return APPSVC_RET_ERROR;
	
	return APPSVC_RET_OK;
}

SLPAPI int appsvc_unset_defapp(const char *defapp)
{
	int ret;

	if(defapp == NULL)
		return APPSVC_RET_EINVAL;

	ret = _svc_db_delete_with_pkgname(defapp);

	if(ret < 0) 
		return APPSVC_RET_ERROR;
	
	return APPSVC_RET_OK;
}

SLPAPI int appsvc_is_defapp(const char *pkg_name)
{
	return _svc_db_is_defapp(pkg_name);
}

SLPAPI int appsvc_data_is_array(bundle *b, const char *key)
{
	int type;
	type = bundle_get_type(b, key);

	if(type <= 0)
		return 0;

	if(type & BUNDLE_TYPE_ARRAY) 
		return 1;
	return 0;
}

typedef struct _appsvc_transient_cb_info_t{
	appsvc_host_res_fn cb_func;

#ifndef WAYLAND
	Ecore_X_Window win_id;
#endif

	void *data;
}appsvc_transient_cb_info_t;

static Eina_Bool __transient_cb(void *data, int type, void *event)
{
#ifndef WAYLAND
	Ecore_X_Event_Window_Hide *ev;
	appsvc_transient_cb_info_t*  cb_info;

	ev = event;
	cb_info = (appsvc_transient_cb_info_t*) data;

	if(ev->win == cb_info->win_id) {
		cb_info->cb_func(cb_info->data);
		ecore_main_loop_quit();
	}
#endif
	return ECORE_CALLBACK_RENEW;
}

int __aul_subapp_cb(void *data)
{
	appsvc_transient_cb_info_t*  cb_info;

	cb_info = (appsvc_transient_cb_info_t*) data;

	cb_info->cb_func(cb_info->data);
	ecore_main_loop_quit();

	return 0;
}

#ifndef WAYLAND
SLPAPI int appsvc_allow_transient_app(bundle *b, Ecore_X_Window id)
{
	char win_id[MAX_LOCAL_BUFSZ];

	snprintf(win_id, MAX_LOCAL_BUFSZ, "%d", id);

	if(b == NULL){
		_E("bundle for appsvc_allow_transient_app is NULL");
		return APPSVC_RET_EINVAL;
	}

	return __set_bundle(b, APP_SVC_K_WIN_ID, win_id);
}
#else
SLPAPI int appsvc_allow_transient_app(bundle *b, unsigned int id)
{
	return 0;
}
#endif

#ifndef WAYLAND
SLPAPI int appsvc_request_transient_app(bundle *b, Ecore_X_Window callee_id, appsvc_host_res_fn cbfunc, void *data)
{
	char *caller = NULL;
	Ecore_X_Window caller_id;

	caller = bundle_get_val(b, APP_SVC_K_WIN_ID);
	if(caller == NULL)
		return APPSVC_RET_ERROR;

	caller_id = atoi(caller);

	ecore_x_icccm_transient_for_set(callee_id, caller_id);

	ecore_x_window_client_manage(caller_id);

	appsvc_transient_cb_info_t* info;

	info = (appsvc_transient_cb_info_t*)calloc(1, sizeof(appsvc_transient_cb_info_t));
	if(info == NULL)
		return APPSVC_RET_ERROR;

	info->win_id = caller_id;
	info->cb_func = cbfunc;
	info->data = data;

	ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, __transient_cb, info);
	aul_set_subapp(__aul_subapp_cb, info);

	return 0;
}
#else
SLPAPI int appsvc_request_transient_app(bundle *b, unsigned int callee_id, appsvc_host_res_fn cbfunc, void *data)
{
	return 0;
}
#endif

SLPAPI int appsvc_subapp_terminate_request_pid(int pid)
{
	return aul_subapp_terminate_request_pid(pid);
}


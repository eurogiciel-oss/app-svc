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

#include <poll.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <Ecore.h>
#include <aul.h>

#include "appsvc.h"


static char** gargv;
static int gargc;
static char* cmd;
static int apn_pid;


typedef struct _test_func_t{
	char* name;
	int(*func)();
	char* desc;
	char* usage;
}test_func_t;

void __set_bundle_from_args(bundle * kb)
{
	int opt;
	char *op = NULL;
	char *mime = NULL;
	char *uri = NULL;
	char *package = NULL;
	char* key = NULL;
	char* val_array[128];
	
	while( (opt = getopt(gargc,gargv,"d:o:m:u:p:")) != -1){
		switch(opt) {
			case 'o':
				if(optarg)
					op = strdup(optarg);
				break;
			case 'm':
				if(optarg)
					mime = strdup(optarg);
				break;
			case 'u':
				if(optarg)
					uri = strdup(optarg);
				break;
			case 'p':
				if(optarg)
					package = strdup(optarg);
				break;
			case 'd':
				if(optarg){
					int i = 0;
					key = strtok(optarg,",");
					while(1)
					{
						val_array[i] = strtok(NULL,",");
						if(val_array[i] == NULL)
							break;
						i++;
					}
					if(i==1) 
						appsvc_add_data(kb, key, val_array[0]);
					else if(i>1) 
						appsvc_add_data_array(kb, key, (const char **)val_array, i);
				}
				break;
		}
	}		

	if(op) {
		appsvc_set_operation(kb,op);
		free(op);
	}
	if(mime) {
		appsvc_set_mime(kb,mime);
		free(mime);
	}
	if(uri) {
		appsvc_set_uri(kb,uri);
		free(uri);
	}
	if(package) {
		appsvc_set_pkgname(kb,package);
		free(package);
	}
}

int run_svc()
{
	int ret;
	bundle *kb=NULL;
	kb = bundle_create();
	if(kb == NULL)
	{
		printf("bundle creation fail\n");
		return -1;
	}
	printf("[run_svc test]\n");

	__set_bundle_from_args(kb);
	
	ret = appsvc_run_service(kb,0,NULL, NULL);
	
	if(ret >= 0){
		printf("open service success\n");
		if(kb)
		{
			bundle_free(kb); 
			kb=NULL;
		}
		return 0;
	}
	else{
		printf("open service fail\n");
		if(kb)
		{
			bundle_free(kb); 
			kb=NULL;
		}
		return -1;		
	}
}

static void prt_recvd_bundle(const char *key, const int type, const bundle_keyval_t *kv, void *user_data)
{
	char **array_val;
	int array_len;
	size_t *array_item_size;

	char *val;
	size_t size;
	int i;
	
	if(bundle_keyval_type_is_array((bundle_keyval_t *)kv) > 0) {
		bundle_keyval_get_array_val((bundle_keyval_t *)kv,
			(void ***)&array_val, (unsigned int *)&array_len,
			&array_item_size);
		
		for (i=0;i<array_len;i++)
		{
			printf("recvd - key: %s[%d], value: %s\n", key, i, array_val[i]);
		}
		
	} else {
		bundle_keyval_get_basic_val((bundle_keyval_t *)kv, (void **)&val, &size);
		printf("recvd - key: %s, value: %s\n",key,val);
	}	
}

static void cb_func(bundle *kb, int request_code, appsvc_result_val result, void *data)
{
	int num;
	num = (int)data;
	
	if(result == APPSVC_RES_CANCEL){
		printf("==== %d : canceled(preemptted) my request ===\n",num);
	}
	else{
		printf("==== %d : result packet === result %d\n",num, (int)result);
		//bundle_iterate(kb, prt_recvd_bundle, NULL);

		bundle_foreach(kb, prt_recvd_bundle, NULL);
	}	


	if(strcmp(cmd,"run_svc_res")==0){
		printf("==== end of appsvc_run() ====\n");
		ecore_main_loop_quit();
	}
}

int run_svc_res()
{
	static int num=0;
	int ret;

	bundle *kb=NULL;
	kb = bundle_create();
	if(kb == NULL)
	{
		printf("bundle creation fail\n");
		return -1;
	}
	
	printf("[run_svc_res test]\n");
	
	__set_bundle_from_args(kb);	
	
	ret = appsvc_run_service(kb, 0, cb_func, (void*)num);
	
	if(ret >= 0){
		printf("open service success\n");
		if(kb)
		{
			bundle_free(kb); 
			kb=NULL;
		}
		return 0;
	}
	else{
		printf("open service fail\n");
		if(kb)
		{
			bundle_free(kb); 
			kb=NULL;
		}
		return -1;		
	}
}


static test_func_t test_func[] = {


	{"run_svc", run_svc, "run_svc test",
	      	"[usage] run_svc -o <OPERATION> [-m <MIME TYPE>] [-u <URI>] [-d \"<key>,<val>\"]..."},
   	{"run_svc_res", run_svc_res, "run_svc_res test",
	      	"[usage] run_svc_res -o <OPERATION> [-m <MIME TYPE>] [-u <URI>] [-d \"<key>,<val>\"]..."},
	      	
};

int callfunc(char* testname)
{
	test_func_t *tmp;
	int res;
	int i;

	for(i=0; i<sizeof(test_func)/sizeof(test_func_t); i++)
	{
		tmp = &test_func[i];
		if(strcmp(testname,tmp->name)==0){
			res = tmp->func();
			if(strcmp(testname,"all")){
				if(res < 0)
					printf("... test failed\n");
				else
					printf("... test successs ret = %d\n", res);
			}
		}
	}
	return 0;
}


void print_usage(char* progname)
{
	test_func_t *tmp;
	int i;
	
	printf("[usage] %s <cmd> ...\n",progname);
	printf(" - available cmd list\n");

	for(i=0; i<sizeof(test_func)/sizeof(test_func_t); i++)
	{
		tmp = &test_func[i];
		printf("\t%s : %s\n",tmp->name, tmp->desc);
		printf("\t\t%s\n",tmp->usage);
	}

}


static Eina_Bool run_func(void *data)
{
	callfunc(cmd);

	if(strcmp(cmd,"run_svc_res") == 0 )
		return 0;
	else
		ecore_main_loop_quit();

	return 0;
}


int main(int argc, char** argv)
{
	if(argc < 3){
		print_usage(argv[0]);
		exit(0);
	}
	
	ecore_init();

	cmd = argv[1];
	gargc = argc;
	gargv = argv;
	apn_pid = atoi(argv[2]);
	
	aul_launch_init(NULL,NULL); 

	//aul_listen_app_dead_signal(dead_tracker,NULL);
	//aul_listen_app_dead_signal(NULL,NULL);

	ecore_idler_add(run_func, NULL);
	
	ecore_main_loop_begin();
					
	return 0;
}



CC ?= gcc

TARGETS = 	utc_ApplicationFW_appsvc_set_operation_func \
		utc_ApplicationFW_appsvc_set_uri_func \
		utc_ApplicationFW_appsvc_set_mime_func \
		utc_ApplicationFW_appsvc_add_data_func \
		utc_ApplicationFW_appsvc_add_data_array_func \
		utc_ApplicationFW_appsvc_set_pkgname_func \
		utc_ApplicationFW_appsvc_run_service_func \
		utc_ApplicationFW_appsvc_get_list_func \
		utc_ApplicationFW_appsvc_get_operation_func \
		utc_ApplicationFW_appsvc_get_mime_func \
		utc_ApplicationFW_appsvc_get_uri_func \
		utc_ApplicationFW_appsvc_get_pkgname_func \
		utc_ApplicationFW_appsvc_is_defapp_func \
		utc_ApplicationFW_appsvc_data_is_array_func \
		utc_ApplicationFW_appsvc_set_defapp_func \
		utc_ApplicationFW_appsvc_unset_defapp_func \
		utc_ApplicationFW_appsvc_get_data_func \
		utc_ApplicationFW_appsvc_get_data_array_func \
		utc_ApplicationFW_appsvc_create_result_bundle_func \
		utc_ApplicationFW_appsvc_send_result_func

PKGS = bundle appsvc

LDFLAGS = `pkg-config --libs $(PKGS)`
LDFLAGS += $(TET_ROOT)/lib/tet3/tcm_s.o
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -ltcm_s
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -lapi_s

CFLAGS = -I. `pkg-config --cflags $(PKGS)`
CFLAGS += -I$(TET_ROOT)/inc/tet3
CFLAGS += -Wall

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGETS)

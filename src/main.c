#define _GNU_SOURCE

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <libserialport.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "become_daemon.h"
#include "signal_handler.h"
#include "logger.h"
#include "error_handler.h"

static int on_method(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method, struct blob_attr *msg);

static int get_method(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method, struct blob_attr *msg);

static int devices(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method, struct blob_attr *msg)
{
    (void)ctx;
    (void)obj;
    (void)req;
    (void)method;
    (void)msg;

    return 0;
}

static int off_method(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method, struct blob_attr *msg)
{
    (void)ctx;
    (void)obj;
    (void)req;
    (void)method;
    (void)msg;

    return 0;
}

enum {
	PIN_NAME,
    PORT_NAME,
	__ON_MAX
};

enum {
    GET_PORT,
    GET_PIN,
    GET_MODEL,
    GET_SENSOR,
    __GET_MAX
};

static const struct blobmsg_policy on_policy[__ON_MAX] = {
	[PIN_NAME]  = { .name = "pin", .type = BLOBMSG_TYPE_INT32 },
    [PORT_NAME] = { .name = "port", .type = BLOBMSG_TYPE_INT32 },
};

static const struct blobmsg_policy get_policy[__GET_MAX] = {
    [GET_PORT]   = { .name = "port",   .type = BLOBMSG_TYPE_INT32 },
    [GET_PIN]    = { .name = "pin",    .type = BLOBMSG_TYPE_INT32 },
    [GET_MODEL]  = { .name = "model",  .type = BLOBMSG_TYPE_STRING },
    [GET_SENSOR] = { .name = "sensor", .type = BLOBMSG_TYPE_STRING },
};

static const struct ubus_method esp_methods[] = {
    UBUS_METHOD_NOARG("devices", devices),
    UBUS_METHOD("on", on_method, on_policy),
    UBUS_METHOD("off", off_method, on_policy),
    UBUS_METHOD("get", get_method, get_policy),
};

static struct ubus_object_type esp_object_type =
    UBUS_OBJECT_TYPE("esp.controller", esp_methods);

static struct ubus_object esp_object = {
    .name = "esp.controller",
    .type = &esp_object_type,
    .methods = esp_methods,
    .n_methods = ARRAY_SIZE(esp_methods),
};

static int on_method(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method, struct blob_attr *msg)
{
    struct blob_attr *tb[__ON_MAX];
    blobmsg_parse(get_policy, __ON_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[PORT_NAME] || !tb[PIN_NAME])
        return UBUS_STATUS_INVALID_ARGUMENT;

    uint32_t req_port = blobmsg_get_u32(tb[PORT_NAME]);
    uint32_t req_pin = blobmsg_get_u32(tb[PIN_NAME]);

    struct blob_buf buf = {};
	
	blob_buf_init(&buf, 0);

    blobmsg_add_u32(&buf, "port", req_port);
    blobmsg_add_u32(&buf, "pin", req_pin);

    ubus_send_reply(ctx, req, buf.head);
	blob_buf_free(&buf);

    return 0;
}

static int get_method(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req,
                      const char *method, struct blob_attr *msg)
{
    struct blob_attr *tb[__GET_MAX];
    blobmsg_parse(get_policy, __GET_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[GET_PORT] || !tb[GET_PIN] || !tb[GET_MODEL] || !tb[GET_SENSOR])
        return UBUS_STATUS_INVALID_ARGUMENT;

    uint32_t req_port    = blobmsg_get_u32(tb[GET_PORT]);
    uint32_t req_pin     = blobmsg_get_u32(tb[GET_PIN]);
    const char *req_model  = blobmsg_get_string(tb[GET_MODEL]);
    const char *req_sensor = blobmsg_get_string(tb[GET_SENSOR]);

    struct blob_buf buf = {};
	
	blob_buf_init(&buf, 0);

    blobmsg_add_u32(&buf, "port", req_port);
    blobmsg_add_u32(&buf, "pin", req_pin);
    blobmsg_add_string(&buf, "model", req_model);
    blobmsg_add_string(&buf, "sensor", req_sensor);

    syslog(LOG_INFO, "Sending data");
    int ret = ubus_send_reply(ctx, req, buf.head);
    //if (ret) syslog(LOG_INFO, "Failed to send data");  ?? wtf neveikia 
    syslog(LOG_INFO, "Data sent");
	blob_buf_free(&buf);

    return 0;
}

int main(void)
{
    setup_signal_handlers();
    logger_init();

    static struct ubus_context *ctx;

    int val = become_daemon(0);
    if (val) {
        print_error(ERROR_DAEMON);
        syslog(stdout, "Daemon failed to start");
        return EXIT_FAILURE;
    }

    syslog(LOG_INFO, "Daemon started");

    uloop_init();

    syslog(LOG_INFO, "Connecting to ubus...");
    ctx = ubus_connect(NULL);
    if (!ctx) {
        syslog(LOG_ERR, "ubus_connect(NULL) failed");
        return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "Connected");

    ubus_add_uloop(ctx);
    int ret = ubus_add_object(ctx, &esp_object);
    if (ret) {
        syslog(LOG_ERR, "ubus_add_object failed: %s", ubus_strerror(ret));
        return EXIT_FAILURE;
    }
    uloop_run();
    ubus_free(ctx);
    uloop_done();
    syslog(LOG_INFO, "Stopping");

    return EXIT_SUCCESS;
}
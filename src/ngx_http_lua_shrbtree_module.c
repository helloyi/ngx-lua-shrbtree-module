
/*
 * Copyright (C) helloyi
 */


#include "ngx_http_lua_shrbtree_common.h"
#include "ngx_http_lua_shrbtree_lapi.h"

#include <ngx_http_lua_api.h>


static void *ngx_http_lua_shrbtree_create_main_conf(ngx_conf_t *cf);
static ngx_int_t ngx_http_lua_shrbtree_init(ngx_conf_t *cf);
static char *ngx_http_lua_shared_rbtree(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


static ngx_command_t ngx_http_lua_shrbtree_cmds[] = {

    { ngx_string("lua_shared_rbtree"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE2,
      ngx_http_lua_shared_rbtree,
      0,
      0,
      NULL },

    ngx_null_command
};

static ngx_http_module_t  ngx_http_lua_shrbtree_module_ctx = {
    NULL,                                   /* preconfiguration */
    ngx_http_lua_shrbtree_init,             /* postconfiguration */
    ngx_http_lua_shrbtree_create_main_conf, /* create main configuration */
    NULL,                                   /* init main configuration */

    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */

    NULL,                                   /* create location configuration */
    NULL,                                   /* merge location configuration */
};

ngx_module_t  ngx_http_lua_shrbtree_module = {
    NGX_MODULE_V1,
    &ngx_http_lua_shrbtree_module_ctx, /* module context */
    ngx_http_lua_shrbtree_cmds,        /* module directives */
    NGX_HTTP_MODULE,                   /* module type */
    NULL,                              /* init master */
    NULL,                              /* init module */
    NULL,                              /* init process */
    NULL,                              /* init thread */
    NULL,                              /* exit thread */
    NULL,                              /* exit process */
    NULL,                              /* exit master */
    NGX_MODULE_V1_PADDING
};


ngx_http_lua_shrbtree_main_conf_t *
ngx_http_lua_shrbtree_get_main_conf(ngx_cycle_t *cycle)
{
    ngx_http_lua_shrbtree_main_conf_t *lsmcf;
    ngx_http_conf_ctx_t               *hmcf_ctx;

    hmcf_ctx = (ngx_http_conf_ctx_t *)cycle->conf_ctx[ngx_http_module.index];
    lsmcf = hmcf_ctx->main_conf[ngx_http_lua_shrbtree_module.ctx_index];

    return lsmcf;
}


static void *
ngx_http_lua_shrbtree_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_lua_shrbtree_main_conf_t *lsmcf;

    lsmcf = ngx_pcalloc(cf->pool, sizeof(*lsmcf));
    if (lsmcf == NULL) {
        return NULL;
    }

    return lsmcf;
}


static ngx_int_t
ngx_http_lua_shrbtree_init(ngx_conf_t *cf)
{
    ngx_http_lua_add_package_preload(cf, "shrbtree",
        ngx_http_lua_shrbtree_preload);
    return NGX_OK;
}


static char *
ngx_http_lua_shared_rbtree(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_lua_shrbtree_main_conf_t  *lsmcf = conf;

    ngx_str_t                  *value, name;
    ngx_shm_zone_t             *zone;
    ngx_shm_zone_t            **zp;
    ngx_http_lua_shrbtree_ctx_t  *ctx;
    ssize_t                     size;

    if (lsmcf->shm_zones == NULL) {
        lsmcf->shm_zones = ngx_palloc(cf->pool, sizeof(ngx_array_t));
        if (lsmcf->shm_zones == NULL) {
            return NGX_CONF_ERROR;
        }

        if (ngx_array_init(lsmcf->shm_zones, cf->pool, 2,
                           sizeof(ngx_shm_zone_t *))
            != NGX_OK)
        {
            return NGX_CONF_ERROR;
        }
    }

    value = cf->args->elts;

    ctx = NULL;

    if (value[1].len == 0) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "invalid lua shared rbtree name \"%V\"", &value[1]);
        return NGX_CONF_ERROR;
    }

    name = value[1];

    size = ngx_parse_size(&value[2]);

    if (size <= 8191) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "invalid lua shared rbtree size \"%V\"", &value[2]);
        return NGX_CONF_ERROR;
    }

    ctx = ngx_pcalloc(cf->pool, sizeof(ngx_http_lua_shrbtree_ctx_t));
    if (ctx == NULL) {
        return NGX_CONF_ERROR;
    }

    ctx->name = name;
    ctx->main_conf = lsmcf;
    ctx->log = &cf->cycle->new_log;

    /* zone = ngx_http_lua_shared_memory_add(cf, &name, (size_t) size, */
                                          /* &ngx_http_lua_shrbtree_module); */
    zone = ngx_shared_memory_add(cf, &name, (size_t) size,
                                 &ngx_http_lua_shrbtree_module);
    if (zone == NULL) {
        return NGX_CONF_ERROR;
    }

    if (zone->data) {
        ctx = zone->data;

        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "lua_shared_rbtree \"%V\" is already defined as "
                           "\"%V\"", &name, &ctx->name);
        return NGX_CONF_ERROR;
    }

    zone->init = ngx_http_lua_shrbtree_init_zone;
    zone->data = ctx;

    zp = ngx_array_push(lsmcf->shm_zones);
    if (zp == NULL) {
        return NGX_CONF_ERROR;
    }

    *zp = zone;

    return NGX_CONF_OK;
}

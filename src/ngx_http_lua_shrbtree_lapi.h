
/*
 * Copyright (C) helloyi
 */


#ifndef _NGX_HTTP_LUA_SHRBTREE_LAPI_H_INCLUDED_
#define _NGX_HTTP_LUA_SHRBTREE_LAPI_H_INCLUDED_


#include "ngx_http_lua_shrbtree_common.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


typedef struct {
    ngx_rbtree_t                  rbtree;
    ngx_rbtree_node_t             sentinel;
} ngx_http_lua_shrbtree_shctx_t;

typedef struct {
    ngx_http_lua_shrbtree_main_conf_t *main_conf;
    ngx_http_lua_shrbtree_shctx_t  *sh;
    ngx_slab_pool_t                *shpool;
    ngx_str_t                      name;
    ngx_log_t                      *log;
} ngx_http_lua_shrbtree_ctx_t;


ngx_int_t ngx_http_lua_shrbtree_init_zone(ngx_shm_zone_t *shm_zone, void *data);
int ngx_http_lua_shrbtree_preload(lua_State *L);


#endif /* _NGX_HTTP_LUA_SHRBTREE_LAPI_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */

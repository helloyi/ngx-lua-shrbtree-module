
/*
 * Copyright (C) helloyi
 */


#ifndef _NGX_HTTP_LUA_SHRBTREE_COMMON_H_INCLUDED_
#define _NGX_HTTP_LUA_SHRBTREE_COMMON_H_INCLUDED_


#include <nginx.h>
#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>


typedef struct {
    ngx_array_t *shm_zones;

} ngx_http_lua_shrbtree_main_conf_t;


ngx_http_lua_shrbtree_main_conf_t *ngx_http_lua_shrbtree_get_main_conf(
    ngx_cycle_t *cycle);


#endif /* _NGX_HTTP_LUA_SHRBTREE_COMMON_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */

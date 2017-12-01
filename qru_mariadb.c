#include <mysql.h>
#include <errno.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "qru_mariadb.h"
#include "qru_mariadb_schema.h"
#include <dlfcn.h>

#ifdef DEBUG
    #define QRUDB_DEBUG(args...) {fprintf(stderr, args);}
    #define ASSERT(args...) assert(args)
    #define ASSERT_INT(args...) assert(args)
    #define ASSERT_POINT(args...) assert(args)
#else
    #define QRUDB_DEBUG(args...)
    #define ASSERT(args...) if (!(args)) {return;}
    #define ASSERT_INT(args...) if (!(args)) {return -1;}
    #define ASSERT_POINT(args...) if (!(args)) {return NULL;}
#endif

#ifdef INNO_DB
	#define DEFAULT_ENGINE "SET storage_engine=InnoDB;"
#else
	#define DEFAULT_ENGINE "SET storage_engine=MyISAM;"
#endif

void* dl_handle = NULL;

int (*dl_mysql_next_result)(MYSQL*);
int (*dl_mysql_stmt_fetch)(MYSQL_STMT*);
int (*dl_mysql_stmt_execute)(MYSQL_STMT*);
int (*dl_mysql_query)(MYSQL*, const char*);
int (*dl_mysql_options)(MYSQL*, enum mysql_option, const void*);
int (*dl_mysql_stmt_prepare)(MYSQL_STMT*, const char*, unsigned long);
int (*dl_mysql_stmt_next_result)(MYSQL_STMT*);
int (*dl_mysql_stmt_store_result)(MYSQL_STMT*);
int (*dl_mysql_set_character_set)(MYSQL*, const char*);
void (*dl_mysql_close)(MYSQL*);
void (*dl_mysql_free_result)(MYSQL_RES*);
MYSQL* (*dl_mysql_init)(MYSQL*);
MYSQL* (*dl_mysql_real_connect)(MYSQL*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long);
my_bool (*dl_mysql_commit)(MYSQL*);
my_bool (*dl_mysql_rollback)(MYSQL*);
my_bool (*dl_mysql_stmt_close)(MYSQL_STMT*);
my_bool (*dl_mysql_autocommit)(MYSQL*, my_bool);
my_bool (*dl_mysql_stmt_free_result)(MYSQL_STMT*);
my_bool (*dl_mysql_stmt_bind_param)(MYSQL_STMT*, MYSQL_BIND*);
my_bool (*dl_mysql_stmt_bind_result)(MYSQL_STMT*, MYSQL_BIND*);
MYSQL_RES* (*dl_mysql_use_result)(MYSQL*);
MYSQL_RES* (*dl_mysql_store_result)(MYSQL*);
MYSQL_STMT* (*dl_mysql_stmt_init)(MYSQL*);
const char* (*dl_mysql_error)(MYSQL*);
my_ulonglong (*dl_mysql_stmt_num_rows)(MYSQL_STMT*);
my_ulonglong (*dl_mysql_num_rows)(MYSQL_RES*);
unsigned int (*dl_mysql_num_fields)(MYSQL_RES*);
const char* (*dl_mysql_stmt_error)(MYSQL_STMT*);
MYSQL_ROW (*dl_mysql_fetch_row)(MYSQL_RES*);

int load_mysql_library() {
    QRUDB_DEBUG("call load_mysql_library()\n");
    
    if (dl_handle) {
        return 0;
    }
    
	dl_handle = dlopen("/usr/local/mariadb/lib/libmysqlclient.so", RTLD_NOW);
    ASSERT_INT(dl_handle != NULL);
    
    dl_mysql_init 		= dlsym(dl_handle, "mysql_init");
    ASSERT_INT(dl_mysql_init != NULL);
    dl_mysql_close 		= dlsym(dl_handle, "mysql_close");
    ASSERT_INT(dl_mysql_close != NULL);
    dl_mysql_error		= dlsym(dl_handle, "mysql_error");
    ASSERT_INT(dl_mysql_error != NULL);
    dl_mysql_query              = dlsym(dl_handle, "mysql_query");
    ASSERT_INT(dl_mysql_query != NULL);
    dl_mysql_commit             = dlsym(dl_handle, "mysql_commit");
    ASSERT_INT(dl_mysql_commit != NULL);
    dl_mysql_options            = dlsym(dl_handle, "mysql_options");
    ASSERT_INT(dl_mysql_options != NULL);
    dl_mysql_rollback           = dlsym(dl_handle, "mysql_rollback");
    ASSERT_INT(dl_mysql_rollback != NULL);
    dl_mysql_stmt_init          = dlsym(dl_handle, "mysql_stmt_init");
    ASSERT_INT(dl_mysql_stmt_init != NULL);
    dl_mysql_stmt_close         = dlsym(dl_handle, "mysql_stmt_close");
    ASSERT_INT(dl_mysql_stmt_close != NULL);
    dl_mysql_stmt_fetch         = dlsym(dl_handle, "mysql_stmt_fetch");
    ASSERT_INT(dl_mysql_stmt_fetch != NULL);
    dl_mysql_use_result         = dlsym(dl_handle, "mysql_use_result");
    ASSERT_INT(dl_mysql_use_result != NULL);
    dl_mysql_store_result       = dlsym(dl_handle, "mysql_store_result");
    ASSERT_INT(dl_mysql_store_result != NULL);
    dl_mysql_autocommit         = dlsym(dl_handle, "mysql_autocommit");
    ASSERT_INT(dl_mysql_autocommit != NULL);
    dl_mysql_free_result        = dlsym(dl_handle, "mysql_free_result");
    ASSERT_INT(dl_mysql_free_result != NULL);
    dl_mysql_next_result        = dlsym(dl_handle, "mysql_next_result");
    ASSERT_INT(dl_mysql_next_result != NULL);
    dl_mysql_real_connect       = dlsym(dl_handle, "mysql_real_connect");
    ASSERT_INT(dl_mysql_real_connect != NULL);
    dl_mysql_stmt_execute       = dlsym(dl_handle, "mysql_stmt_execute");
    ASSERT_INT(dl_mysql_stmt_execute != NULL);
    dl_mysql_stmt_prepare       = dlsym(dl_handle, "mysql_stmt_prepare");
    ASSERT_INT(dl_mysql_stmt_prepare != NULL);
    dl_mysql_num_rows           = dlsym(dl_handle, "mysql_num_rows");
    ASSERT_INT(dl_mysql_num_rows != NULL);
    dl_mysql_num_fields         = dlsym(dl_handle, "mysql_num_fields");
    ASSERT_INT(dl_mysql_num_fields != NULL);
    dl_mysql_stmt_num_rows      = dlsym(dl_handle, "mysql_stmt_num_rows");
    ASSERT_INT(dl_mysql_stmt_num_rows != NULL);
    dl_mysql_stmt_bind_param    = dlsym(dl_handle, "mysql_stmt_bind_param");
    ASSERT_INT(dl_mysql_stmt_bind_param != NULL);
    dl_mysql_stmt_bind_result   = dlsym(dl_handle, "mysql_stmt_bind_result");
    ASSERT_INT(dl_mysql_stmt_bind_result != NULL);
    dl_mysql_stmt_free_result   = dlsym(dl_handle, "mysql_stmt_free_result");
    ASSERT_INT(dl_mysql_stmt_free_result != NULL);
    dl_mysql_stmt_next_result   = dlsym(dl_handle, "mysql_stmt_next_result");
    ASSERT_INT(dl_mysql_stmt_next_result != NULL);
    dl_mysql_stmt_store_result  = dlsym(dl_handle, "mysql_stmt_store_result");
    ASSERT_INT(dl_mysql_stmt_store_result != NULL);
    dl_mysql_set_character_set  = dlsym(dl_handle, "mysql_set_character_set");
    ASSERT_INT(dl_mysql_set_character_set != NULL);
    dl_mysql_stmt_error     = dlsym(dl_handle, "mysql_stmt_error");
    ASSERT_INT(dl_mysql_stmt_error != NULL);
    dl_mysql_fetch_row      = dlsym(dl_handle, "mysql_fetch_row");
    ASSERT_INT(dl_mysql_fetch_row != NULL);
    
    return 0;
}

typedef enum bind_type {
    BIND_CHAR,
    BIND_STRING,
    BIND_SHORT,
    BIND_INT,
    BIND_LONG,
    BIND_FLOAT,
    BIND_DOUBLE,
    BIND_NULL
} bind_type;

void bind_param(MYSQL_BIND* bind, const void* value, bind_type type, size_t size) {
    bind->buffer = (void*) value;
    
    switch (type) {
        case BIND_CHAR: {
            bind->buffer_type   = MYSQL_TYPE_STRING;
            bind->buffer_length = 1;
        } break;
        
        case BIND_STRING: {
            bind->buffer_type   = MYSQL_TYPE_STRING;
            bind->buffer_length = size;
        } break;
        
        case BIND_SHORT: {
            bind->buffer_type   = MYSQL_TYPE_SHORT;
        } break;
        
        case BIND_INT: {
            bind->buffer_type   = MYSQL_TYPE_LONG;
        } break;
        
        case BIND_LONG: {
            bind->buffer_type   = MYSQL_TYPE_LONGLONG;
        } break;
        
        case BIND_FLOAT: {
            bind->buffer_type   = MYSQL_TYPE_FLOAT;
        } break;
        
        case BIND_DOUBLE: {
            bind->buffer_type   = MYSQL_TYPE_DOUBLE;
        } break;
        
        case BIND_NULL: {
            bind->buffer = (void*) NULL;
            bind->buffer_type   = MYSQL_TYPE_NULL;
        }
    }
}

MYSQL* create_connection(const char* db, const int auto_commit) {
    QRUDB_DEBUG("call create_connection()\n");
    
    if (load_mysql_library()) {
        QRUDB_DEBUG("load library fail\n");
        return NULL;
    }
    
    MYSQL* real_conn = NULL;
    
    if ((real_conn  = (*dl_mysql_init)(NULL)) == NULL) {
        QRUDB_DEBUG("dl_mysql_init fail\n");
        return NULL;
    }

    if ((*dl_mysql_options)(real_conn, MYSQL_INIT_COMMAND, (void *) DEFAULT_ENGINE)) {
        if (real_conn) QRUDB_DEBUG("%s\n", (*dl_mysql_error)(real_conn));
        return NULL;
    }
    
    if ((*dl_mysql_options)(real_conn, MYSQL_SET_CHARSET_NAME, "utf8")) {
        if (real_conn) QRUDB_DEBUG("%s\n", (*dl_mysql_error)(real_conn));
        return NULL;
    }
    
    if ((*dl_mysql_real_connect)(real_conn, "localhost", "root", "qnapqnap", db, 0, "/tmp/qru/mysql_qru.sock", CLIENT_MULTI_RESULTS) == NULL) {
        if (real_conn) (*dl_mysql_close)(real_conn);
        QRUDB_DEBUG("real_connect fail\n");
        return NULL;
    }
    
    if ((*dl_mysql_set_character_set)(real_conn, "utf8")) {
        if (real_conn) QRUDB_DEBUG("%s\n", (*dl_mysql_error)(real_conn));
        if (real_conn) (*dl_mysql_close)(real_conn);
    }
    
    if (auto_commit == 0 && (*dl_mysql_autocommit)(real_conn, auto_commit)) {
        if (real_conn) QRUDB_DEBUG("%s\n", (*dl_mysql_error)(real_conn));
        if (real_conn) (*dl_mysql_close)(real_conn);
        return NULL;
    }
    
    return real_conn;
}

void close_connection(MYSQL* conn) {
    QRUDB_DEBUG("call close_connection()\n");
    
    ASSERT(conn != NULL);
    
    (*dl_mysql_close)(conn);
    
    conn = NULL;
}

void qru_execute_command(const char command[]) {
    QRUDB_DEBUG("call qru_execute_command()\n");
    
    MYSQL* conn = NULL;
    
    ASSERT(command != NULL);
    conn = create_connection("qru", 1);
    ASSERT(conn != NULL);
    
    if ((*dl_mysql_query)(conn, command)) {
        QRUDB_DEBUG("%s\n", command);
        goto exception;
    }
    
    if (conn->status) {
        (*dl_mysql_free_result)((*dl_mysql_use_result)(conn));
        (*dl_mysql_next_result)(conn);
    }
    
exception:
    if (conn) QRUDB_DEBUG("%s\n", (*dl_mysql_error)(conn));
    
    close_connection(conn);
}

void qru_initial_mariadb() {
    QRUDB_DEBUG("call qru_initial_mariadb()\n");
    
    MYSQL* conn = NULL;
    MYSQL_STMT* stmt = NULL;
    
    const char** schema = NULL;
    const char* initial_schmea[] = {

        // create database and tables
        QRU_CREATE_DB,
        QRU_CHANGE_DB,
        QRU_CREATE_CPU_DAY_TABLE,
        QRU_CREATE_CPU_WEEK_TABLE,
        QRU_CREATE_CPU_MONTH_TABLE,
        QRU_CREATE_CPU_YEAR_TABLE,
        QRU_CREATE_MEM_DAY_TABLE,
        QRU_CREATE_MEM_WEEK_TABLE,
        QRU_CREATE_MEM_MONTH_TABLE,
        QRU_CREATE_MEM_YEAR_TABLE,
        QRU_CREATE_NIC_DAY_TABLE,
        QRU_CREATE_NIC_WEEK_TABLE,
        QRU_CREATE_NIC_MONTH_TABLE,
        QRU_CREATE_NIC_YEAR_TABLE,
        QRU_CREATE_POOL_DAY_TABLE,
        QRU_CREATE_POOL_WEEK_TABLE,
        QRU_CREATE_POOL_MONTH_TABLE,
        QRU_CREATE_POOL_YEAR_TABLE,
        QRU_CREATE_PROC_DAY_TABLE,
        QRU_CREATE_PROC_WEEK_TABLE,
        QRU_CREATE_PROC_MONTH_TABLE,
        QRU_CREATE_PROC_YEAR_TABLE,

		// cleanup procedure & function



		// create procedure & function



        // end
        NULL
    };
    conn = create_connection(NULL, 1);
    ASSERT(conn != NULL);
    
    for (schema = initial_schmea ; schema != NULL && *schema != NULL ; ++schema) {
        if ((*dl_mysql_query)(conn, *schema)) {
            QRUDB_DEBUG("%s\n", *schema);
            goto exception;
        }
        
        if (conn->status) {
            (*dl_mysql_free_result)((*dl_mysql_use_result)(conn));
            (*dl_mysql_next_result)(conn);
        }
    }
    
    if (!(stmt = (*dl_mysql_stmt_init)(conn))) {
        goto exception;
    }

    if ((*dl_mysql_stmt_prepare)(stmt, QBOX_CALL_UPDATE_PATTERN, strlen(QBOX_CALL_UPDATE_PATTERN))) {
        goto exception;
    }
    
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));
    
    size_t i, size = sizeof(pattern) / sizeof(QBOX_TEMPLATE);
    for (i = 0 ; i < size ; ++i) {
        bind_param(&bind[0], pattern[i].name,      BIND_STRING, strlen(pattern[i].name));
        bind_param(&bind[1], pattern[i].pattern,   BIND_STRING, strlen(pattern[i].pattern));
        
        if ((*dl_mysql_stmt_bind_param)(stmt, bind)) {
            goto exception;
        }
        
        if ((*dl_mysql_stmt_execute)(stmt)) {
            goto exception;
        }
    }
    
exception:
    if (conn) QRUDB_DEBUG("%s\n", (*dl_mysql_error)(conn));
    
    if (stmt) (*dl_mysql_stmt_free_result)(stmt);
    if (stmt) (*dl_mysql_stmt_close)(stmt);
    
    close_connection(conn);
    
    {
	/* alter description */
	qbox_alter_team_folder_description();

        /* unknow user case please set user_id to this one */
        qbox_user_info unknow_user;
        unknow_user.user_id = -1;
        unknow_user.user_name[0] = 0;
        unknow_user.user_description[0] = 0;
        
        qbox_update_user_info(&unknow_user, 1);
#ifdef QBOX_CLUSTER
	qbox_user_slave unknow_user_slave;
	unknow_user_slave.user_id=-1;
	unknow_user_slave.slave_id[0]=0;
	unknow_user_slave.status = 0;
	qbox_update_user_slave(&unknow_user_slave, 1);	
	qbox_group_slave unknow_group_slave;
        unknow_group_slave.group_id=-1;
        unknow_group_slave.slave_id[0]=0;
	unknow_group_slave.status = 0;
        qbox_update_group_slave(&unknow_group_slave, 1);
#endif
    }
}








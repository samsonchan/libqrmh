#include <mysql.h>
#include <errno.h>

#include "qrmh_mariadb.h"
#include "qrmh_mariadb_schema.h"

#include <assert.h>

// ----------------------------------------------------------------------------
// Debug
// ----------------------------------------------------------------------------
enum {
	D_ERROR=0,
	D_INFO,
	D_DEBUG
};

static int debug_lvl = 1;

#define MSG_E(format, args...) if(debug_lvl > D_ERROR) fprintf(stderr, "%s(%d): " format, __func__, __LINE__, ##args)
#define MSG_I(format, args...) if(debug_lvl > D_INFO) printf("%s(%d): " format, __func__, __LINE__, ##args)
#define MSG_D(format, args...) if(debug_lvl > D_DEBUG) printf("%s(%d): " format, __func__, __LINE__, ##args)

#define STMT_ERR(S,M) \
{ \
	MSG_E("%s : error %u (%s): %s\n",   \
		(M) ? (M) : "", \
		(S) ? mysql_stmt_errno(S) : 0,  \
		(S) ? mysql_stmt_sqlstate(S) : 0,   \
		(S) ? mysql_stmt_error (S) : "");   \
}

#ifdef INNO_DB
	#define DEFAULT_ENGINE "SET storage_engine=InnoDB;"
#else
	#define DEFAULT_ENGINE "SET storage_engine=MyISAM;"
#endif

// ----------------------------------------------------------------------------
// Local Variables for APIs
// ----------------------------------------------------------------------------
static int initialized = 0;

#define BIND_PARAM(B,P,T,S,U) \
{ \
	(B)->buffer = (void *)P; \
	(B)->buffer_type = T; \
	(B)->buffer_length = S; \
	(B)->is_unsigned = U; \
}

// ----------------------------------------------------------------------------
// Local Funtions
// ----------------------------------------------------------------------------
static MYSQL* create_connection(const char* db, const int auto_commit)
{
	MYSQL* real_conn = NULL;

	if((real_conn  = mysql_init(NULL)) == NULL) {
		MSG_E("mysql_init fail\n");
		return NULL;
	}

	if(mysql_options(real_conn, MYSQL_INIT_COMMAND, (void *) DEFAULT_ENGINE)) {
		if(real_conn) MSG_E("mysql_options(MYSQL_INIT_COMMAND) error: %s\n", mysql_error(real_conn));
			return NULL;
	}

	if(mysql_options(real_conn, MYSQL_SET_CHARSET_NAME, "utf8")) {
		if(real_conn) MSG_E("mysql_options(MYSQL_SET_CHARSET_NAME) error: %s\n", mysql_error(real_conn));
			return NULL;
	}

	if(mysql_real_connect(real_conn, "localhost", "root", "qnapqnap", db, 0, "/tmp/mysql_qrmh.sock", CLIENT_MULTI_RESULTS) == NULL) {
		if(real_conn) mysql_close(real_conn);
			return NULL;
	}

	if(mysql_set_character_set(real_conn, "utf8")) {
		if(real_conn) MSG_E("mysql_set_character_set() error: %s\n", mysql_error(real_conn));
		if(real_conn) mysql_close(real_conn);
			return NULL;
	}

	if(auto_commit == 0 && mysql_autocommit(real_conn, auto_commit)) {
		if(real_conn) MSG_E("mysql_autocommit() error : %s\n", mysql_error(real_conn));
		if(real_conn) mysql_close(real_conn);
			return NULL;
	}

	return real_conn;
}

static void close_connection(MYSQL* conn)
{
	if(conn)
		mysql_close(conn);
}

// ----------------------------------------------------------------------------
// MariaDB APIs
// ----------------------------------------------------------------------------
int qrmh_execute_command(const char *cmd)
{
	if(!cmd)
		return -1;

	int ret = -1;
	MYSQL* conn = NULL;
	MYSQL_STMT* stmt = NULL;

	conn = create_connection("qrmh", 1);
	if(!conn) {
		MSG_E("create_connection failed\n");
		return 0;
	}
	if(mysql_query(conn, cmd)) {
		MSG_E("mysql_query() error cmd: %s\n", cmd);
		goto exception;
	}
	if(conn->status) {
		mysql_free_result(mysql_use_result(conn));
		mysql_next_result(conn);
		ret = 0;
	}

exception:
	if(conn && mysql_errno(conn)) MSG_E("error: %s\n", mysql_error(conn));
	if(stmt) mysql_stmt_free_result(stmt);
	if(stmt) mysql_stmt_close(stmt);
	close_connection(conn);
	return ret;
}

void qrmh_initial_mariadb(void)
{
	if(initialized)
		return ;
	initialized = 1;

	MYSQL* conn = NULL;
	MYSQL_STMT* stmt = NULL;

	const char** schema = NULL;
	const char* initial_schmea[] = {
        // repair mysql proc table
        QRMH_REPAIR_MYSQL_PROC,

		// create database, tables, and views
		QRMH_CREATE_DB,
		QRMH_CHANGE_DB,
		






		NULL
	};

	assert((conn = create_connection(NULL, 1)) != NULL);

	for (schema = initial_schmea ; schema != NULL && *schema != NULL ; ++schema) {
		if(mysql_query(conn, *schema)) {
			MSG_E("mysql_query() error schma: %s\n", *schema);
			goto exception;
		}

		if(conn->status) {
			mysql_free_result(mysql_use_result(conn));
			mysql_next_result(conn);
		}
	}

exception:
	if(conn && mysql_errno(conn)) MSG_E("error: %s\n", mysql_error(conn));
	if(stmt) mysql_stmt_free_result(stmt);
	if(stmt) mysql_stmt_close(stmt);

	close_connection(conn);
}








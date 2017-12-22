#ifndef __QRU_MARIADB_SCHEMA_H__
#define __QRU_MARIADB_SCHEMA_H__

#ifndef SQL_COMMAND
#define SQL_COMMAND(args...) #args
#endif

const char* QRU_SET_MAX_CONNECTION = SQL_COMMAND(
    set global max_connections = 1024;
);

typedef struct QRU_TEMPLATE {
	const char* name;
	const char* pattern;
} QRU_TEMPLATE;

const char* QRU_CREATE_DB = SQL_COMMAND(
	CREATE DATABASE IF NOT EXISTS `task_manager`;
);

const char* QRU_CHANGE_DB = SQL_COMMAND(
	USE `task_manager`;
);

const char* QRU_START_TRANSACTION = SQL_COMMAND(
	START TRANSACTION;
);

/**
 * PROCEDURE table_exist()
 */
const char* QRU_TABLE_EXIST = SQL_COMMAND(
    CREATE PROCEDURE
        table_exist(
            IN _table_name VARCHAR(64) CHARACTER SET UTF8,
            OUT _table_exist BOOL
        )
    BEGIN
        SELECT COUNT(*) FROM information_schema.tables
        WHERE table_schema = 'task_manager' AND table_name = _table_name
        LIMIT 1 INTO _table_exist;
    END;
);

const char* QRU_DROP_TABLE_EXIST = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `table_exist`;
);

/**
 * PROCEDURE update_pattern()
 */
const char* QRU_DROP_UPDATE_PATTERN = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_pattern`;
);

const char* QRU_UPDATE_PATTERN = SQL_COMMAND(
    CREATE PROCEDURE
        update_pattern(
            IN _name VARCHAR(255) CHARACTER SET UTF8,
            IN _pattern TEXT CHARACTER SET UTF8
        )
    BEGIN
        INSERT INTO `pattern` VALUES(
            _name,
            _pattern
        ) ON DUPLICATE KEY UPDATE
            `pattern` = _pattern;
    END;
);

/**
 * PROCEDURE update_cpu()
 */
const char* QRU_DROP_UPDATE_CPU = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_cpu`;
);

const char* QRU_UPDATE_CPU = SQL_COMMAND(
	CREATE PROCEDURE
		update_cpu(
			IN _monitor_time INT UNSIGNED DEFAULT 0,
			IN _cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _table_name TINYTEXT CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @monitor_time := _monitor_time;
		SET @cpu_usage := _cpu_usage;
		SET @table_name := _table_name;

	IF _check_table_exist THEN 
		CALL table_exist(@table_name, @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `{TABLE_NAME}` (\
				`id` INT NOT NULL PRIMARY KEY AUTO_INCREMENT,\
				`monitor_time` INT UNSIGNED DEFAULT 0,\
				`cpu_usage` DOUBLE(3,1) UNSIGNED DEFAULT 0.0,\
				INDEX (`id`, `monitor_time`, `cpu_usage`)\
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'\
			";

			SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
			SET @pattern := REPLACE(@pattern, '{NEXT_ID}', '1');
			PREPARE cpu_table FROM @pattern;
			EXECUTE cpu_table;
			DROP PREPARE cpu_table;
		END IF;
	END IF;

	SET @pattern := "\
		INSERT INTO `{TABLE_NAME}` VALUES(\
			NULL,\
			?,\
			?\
	)";

	SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
	PREPARE append_cpu FROM @pattern;
	EXECUTE append_cpu USING
	    @monitor_time,
	    @cpu_usage
	;
	DROP PREPARE append_cpu;
	END;
);

const char* QRU_CALL_UPDATE_CPU = SQL_COMMAND(
    CALL update_cpu (
		?, // @monitor_time INT UNSIGNED DEFAULT 0
		?, // @cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?  // @check_table_exist BOOL
    );
);

/**
 * PROCEDURE drop_cpu()
 */
const char* QRU_DROP_DROP_CPU = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `drop_cpu`;
);

const char* QRU_DROP_CPU = SQL_COMMAND(
    CREATE PROCEDURE
        drop_cpu(
            IN _table_name VARCHAR(40) CHARACTER SET UTF8
        )
    BEGIN
        DECLARE CONTINUE HANDLER FOR 1051 BEGIN END;
        
        SET @pattern := CONVERT('DROP TABLE `{TABLE_NAME}' USING UTF8);
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', _table_name);

        PREPARE statment FROM @pattern;
        EXECUTE statment;
        DROP PREPARE statment;
    END;
);

const char* QRU_CALL_DROP_CPU = SQL_COMMAND(
    CALL drop_cpu (
        ?  // @_table_name VARCHAR(40) CHARACTER SET UTF8
    );
);

/**
 * PROCEDURE query_cpu()
 */
QRU_TEMPLATE QRU_QUERY_CPU_PATTERN = {
    "query_cpu_pattern",
    SQL_COMMAND(
        SELECT SQL_CALC_FOUND_ROWS * FROM `{TABLE_NAME}`
        WHERE
            {LOWER_CPU_USAGE} AND
            {UPPER_CPU_USAGE} AND
            {LOWER_MONITOR_TIME} AND
            {UPPER_MONITOR_TIME}
        ORDER BY {ORDER_BY} {ORDER_TYPE}
        LIMIT ?, ?;
    )
};

const char* QRU_DROP_QUERY_CPU = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `query_cpu`;
);

const char* QRU_QUERY_CPU = SQL_COMMAND(
    CREATE PROCEDURE
        query_cpu(
			IN _lower_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _upper_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _lower_monitor_time INT,
			IN _upper_monitor_time INT,
            IN _table_name TINYTEXT CHARACTER SET UTF8,
            IN _order_by SMALLINT,
            IN _skip INT,
            IN _limit INT,
            OUT _total_records INT
        )
    BEGIN
        DECLARE EXIT HANDLER FOR 1146 BEGIN END;
        
        SET _total_records := 0;
        
        SET @table_name := _table_name;
        SELECT `pattern` FROM `pattern` WHERE `name` = 'query_cpu_pattern' INTO @pattern;
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);

        IF _lower_cpu_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_CPU_USAGE}', '?');
            SET @lower_cpu_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_CPU_USAGE}', '`cpu_usage` >= ?');
            SET @lower_cpu_usage = _lower_cpu_usage;
        END IF;

        IF _upper_cpu_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_CPU_USAGE}', '?');
            SET @upper_cpu_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_CPU_USAGE}', '`cpu_usage` <= ?');
            SET @upper_cpu_usage = _upper_cpu_usage;
        END IF;

        IF _lower_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '?');
            SET @lower_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '`monitor_time` >= ?');
            SET @lower_monitor_time = _lower_monitor_time;
        END IF;

        IF _upper_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '?');
            SET @upper_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '`monitor_time` <= ?');
            SET @upper_monitor_time = _upper_monitor_time;
        END IF;

        IF _order_by < 0 THEN
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'DESC');
        ELSE
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'ASC');
        END IF;
        
        CASE ABS(_order_by)
        WHEN 1 THEN SET @order_by := '`monitor_time`';
		WHEN 2 THEN SET @order_by := '`cpu_usage`';
        ELSE SET @order_by := 'NULL';
        END CASE;
        
        SET @pattern := REPLACE(@pattern, '{ORDER_BY}', CONVERT(@order_by USING UTF8));
        
        IF _skip < 0 THEN
            SET @skip := 0;
        ELSE
            SET @skip := _skip;
        END IF;
        
        IF _limit <= 0 THEN
            SET @limit := 100000;
        ELSE
            SET @limit := _limit;
        END IF;
	
        PREPARE stmtment FROM @pattern;
        EXECUTE stmtment USING
            @lower_cpu_usage,
            @upper_cpu_usage,
            @lower_monitor_time,
            @upper_monitor_time,
            @skip,
            @limit;
        DROP PREPARE stmtment;
        
        SET _total_records := FOUND_ROWS();
    END;
);

const char* QRU_CALL_QUERY_CPU = SQL_COMMAND(
    CALL query_cpu (
		?, // @lower_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @upper_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
        ?, // @lower_monitor_time INT
        ?, // @upper_monitor_time INT
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?, // @order_by SMALLINT
        ?, // @skip INT
        ?, // @limit INT
        ?  // $total_records INT
    );
);


/**
 * PROCEDURE update_mem()
 */
const char* QRU_DROP_UPDATE_MEM = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_mem`;
);

const char* QRU_UPDATE_MEM = SQL_COMMAND(
	CREATE PROCEDURE
		update_mem(
			IN _monitor_time INT UNSIGNED DEFAULT 0,
			IN _mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _mem_used INT UNSIGNED DEFAULT 0,
			IN _table_name TINYTEXT CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @monitor_time := _monitor_time;
		SET @mem_usage := _mem_usage;
		SET @mem_used := _mem_used;
		SET @table_name := _table_name;

	IF _check_table_exist THEN 
		CALL table_exist(@table_name, @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `{TABLE_NAME}` (\
				`id` INT NOT NULL PRIMARY KEY AUTO_INCREMENT,\
				`monitor_time` INT UNSIGNED DEFAULT 0,\
				`mem_usage` DOUBLE(3,1) UNSIGNED DEFAULT 0.0,\
				`mem_used` INT UNSIGNED DEFAULT 0,\
				INDEX (`id`, `monitor_time`, `mem_usage`, `mem_used`)\
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'\
			";

			SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
			SET @pattern := REPLACE(@pattern, '{NEXT_ID}', '1');
			PREPARE mem_table FROM @pattern;
			EXECUTE mem_table;
			DROP PREPARE mem_table;
		END IF;
	END IF;

	SET @pattern := "\
		INSERT INTO `{TABLE_NAME}` VALUES(\
			NULL,\
			?,\
			?,\
			?\
	)";

	SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
	PREPARE append_mem FROM @pattern;
	EXECUTE append_mem USING
	    @monitor_time,
	    @mem_usage,
	    @mem_used
	;
	DROP PREPARE append_mem;
	END;
);

const char* QRU_CALL_UPDATE_MEM = SQL_COMMAND(
    CALL update_mem (
		?, // @monitor_time INT UNSIGNED DEFAULT 0
		?, // @mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @mem_used INT UNSIGNED DEFAULT 0
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?  // @check_table_exist BOOL
    );
);

/**
 * PROCEDURE drop_mem()
 */
const char* QRU_DROP_DROP_MEM = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `drop_mem`;
);

const char* QRU_DROP_MEM = SQL_COMMAND(
    CREATE PROCEDURE
        drop_mem(
            IN _table_name VARCHAR(40) CHARACTER SET UTF8
        )
    BEGIN
        DECLARE CONTINUE HANDLER FOR 1051 BEGIN END;
        
        SET @pattern := CONVERT('DROP TABLE `{TABLE_NAME}' USING UTF8);
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', _table_name);

        PREPARE statment FROM @pattern;
        EXECUTE statment;
        DROP PREPARE statment;
    END;
);

const char* QRU_CALL_DROP_MEM = SQL_COMMAND(
    CALL drop_mem (
        ?  // @_table_name VARCHAR(40) CHARACTER SET UTF8
    );
);

/**
 * PROCEDURE query_mem()
 */
QRU_TEMPLATE QRU_QUERY_MEM_PATTERN = {
    "query_mem_pattern",
    SQL_COMMAND(
        SELECT SQL_CALC_FOUND_ROWS * FROM `{TABLE_NAME}`
        WHERE
            {LOWER_MEM_USAGE} AND
            {UPPER_MEM_USAGE} AND
            {LOWER_MONITOR_TIME} AND
            {UPPER_MONITOR_TIME}
        ORDER BY {ORDER_BY} {ORDER_TYPE}
        LIMIT ?, ?;
    )
};

const char* QRU_DROP_QUERY_MEM = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `query_mem`;
);

const char* QRU_QUERY_MEM = SQL_COMMAND(
    CREATE PROCEDURE
        query_mem(
			IN _lower_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _upper_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _lower_monitor_time INT,
			IN _upper_monitor_time INT,
            IN _table_name TINYTEXT CHARACTER SET UTF8,
            IN _order_by SMALLINT,
            IN _skip INT,
            IN _limit INT,
            OUT _total_records INT
        )
    BEGIN
        DECLARE EXIT HANDLER FOR 1146 BEGIN END;
        
        SET _total_records := 0;
        
        SET @table_name := _table_name;
        SELECT `pattern` FROM `pattern` WHERE `name` = 'query_mem_pattern' INTO @pattern;
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);

        IF _lower_mem_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MEM_USAGE}', '?');
            SET @lower_mem_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MEM_USAGE}', '`mem_usage` >= ?');
            SET @lower_mem_usage = _lower_mem_usage;
        END IF;

        IF _upper_mem_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MEM_USAGE}', '?');
            SET @upper_mem_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MEM_USAGE}', '`mem_usage` <= ?');
            SET @upper_mem_usage = _upper_mem_usage;
        END IF;

        IF _lower_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '?');
            SET @lower_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '`monitor_time` >= ?');
            SET @lower_monitor_time = _lower_monitor_time;
        END IF;

        IF _upper_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '?');
            SET @upper_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '`monitor_time` <= ?');
            SET @upper_monitor_time = _upper_monitor_time;
        END IF;

        IF _order_by < 0 THEN
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'DESC');
        ELSE
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'ASC');
        END IF;
        
        CASE ABS(_order_by)
        WHEN 1 THEN SET @order_by := '`monitor_time`';
		WHEN 2 THEN SET @order_by := '`mem_usage`';
		WHEN 3 THEN SET @order_by := '`mem_used`';
        ELSE SET @order_by := 'NULL';
        END CASE;
        
        SET @pattern := REPLACE(@pattern, '{ORDER_BY}', CONVERT(@order_by USING UTF8));
        
        IF _skip < 0 THEN
            SET @skip := 0;
        ELSE
            SET @skip := _skip;
        END IF;
        
        IF _limit <= 0 THEN
            SET @limit := 100000;
        ELSE
            SET @limit := _limit;
        END IF;
	
        PREPARE stmtment FROM @pattern;
        EXECUTE stmtment USING
            @lower_mem_usage,
            @upper_mem_usage,
            @lower_monitor_time,
            @upper_monitor_time,
            @skip,
            @limit;
        DROP PREPARE stmtment;
        
        SET _total_records := FOUND_ROWS();
    END;
);

const char* QRU_CALL_QUERY_MEM = SQL_COMMAND(
    CALL query_mem (
		?, // @lower_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @upper_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
        ?, // @lower_monitor_time INT
        ?, // @upper_monitor_time INT
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?, // @order_by SMALLINT
        ?, // @skip INT
        ?, // @limit INT
        ?  // $total_records INT
    );
);


/**
 * PROCEDURE update_proc()
 */
const char* QRU_DROP_UPDATE_PROC = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_proc`;
);

const char* QRU_UPDATE_PROC = SQL_COMMAND(
	CREATE PROCEDURE
		update_proc(
			IN _monitor_time INT UNSIGNED DEFAULT 0,
			IN _pid INT UNSIGNED,
			IN _user_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _proc_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _proc_app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _proc_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _proc_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _proc_mem_used INT UNSIGNED DEFAULT 0,
			IN _proc_tx BIGINT UNSIGNED DEFAULT 0,
			IN _proc_rx BIGINT UNSIGNED DEFAULT 0,
			IN _table_name TINYTEXT CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @monitor_time := _monitor_time;
		SET @pid := _pid;
		SET @user_name := _user_name;
		SET @proc_name := _proc_name;
		SET @proc_app_name := _proc_app_name;
		SET @proc_cpu_usage := _proc_cpu_usage;
		SET @proc_mem_usage := _proc_mem_usage;
		SET @proc_mem_used := _proc_mem_used;
		SET @proc_tx := _proc_tx;
		SET @proc_rx := _proc_rx;
		SET @table_name := _table_name;

	IF _check_table_exist THEN 
		CALL table_exist(@table_name, @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `{TABLE_NAME}` (\
				`id` INT NOT NULL PRIMARY KEY AUTO_INCREMENT,\
				`monitor_time` INT UNSIGNED DEFAULT 0,\
				`pid` INT UNSIGNED,\
				`user_name` VARCHAR(256) CHARACTER SET UTF8 NOT NULL,\
				`proc_name` VARCHAR(256) CHARACTER SET UTF8 NOT NULL,\
				`proc_app_name` VARCHAR(256) CHARACTER SET UTF8 NOT NULL,\
				`proc_cpu_usage` DOUBLE(3,1) UNSIGNED DEFAULT 0.0,\
				`proc_mem_usage` DOUBLE(3,1) UNSIGNED DEFAULT 0.0,\
				`proc_mem_used` INT UNSIGNED DEFAULT 0,\
				`proc_tx` BIGINT UNSIGNED DEFAULT 0,\
				`proc_rx` BIGINT UNSIGNED DEFAULT 0,\
				INDEX (`id`, `monitor_time`, `pid`, `proc_name`)\
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'\
			";

			SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
			SET @pattern := REPLACE(@pattern, '{NEXT_ID}', '1');
			PREPARE proc_table FROM @pattern;
			EXECUTE proc_table;
			DROP PREPARE proc_table;
		END IF;
	END IF;

	SET @pattern := "\
		INSERT INTO `{TABLE_NAME}` VALUES(\
			NULL,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?\
	)";

	SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
	PREPARE append_proc FROM @pattern;
	EXECUTE append_proc USING
	    @monitor_time,
	    @pid,
	    @user_name,
		@proc_name,
		@proc_app_name,
		@proc_cpu_usage,
		@proc_mem_usage,
		@proc_mem_used,
		@proc_tx,
		@proc_rx
	;
	DROP PREPARE append_proc;
	END;
);

const char* QRU_CALL_UPDATE_PROC = SQL_COMMAND(
    CALL update_proc (
		?, // @monitor_time INT UNSIGNED DEFAULT 0
		?, // @pid INT UNSIGNED
		?, // @user_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @proc_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @proc_app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @proc_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @proc_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @proc_mem_used INT UNSIGNED DEFAULT 0
		?, // @proc_tx BIGINT UNSIGNED DEFAULT 0
		?, // @proc_rx BIGINT UNSIGNED DEFAULT 0
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?  // @check_table_exist BOOL
    );
);

/**
 * PROCEDURE drop_proc()
 */
const char* QRU_DROP_DROP_PROC = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `drop_proc`;
);

const char* QRU_DROP_PROC = SQL_COMMAND(
    CREATE PROCEDURE
        drop_proc(
            IN _table_name VARCHAR(40) CHARACTER SET UTF8
        )
    BEGIN
        DECLARE CONTINUE HANDLER FOR 1051 BEGIN END;
        
        SET @pattern := CONVERT('DROP TABLE `{TABLE_NAME}' USING UTF8);
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', _table_name);

        PREPARE statment FROM @pattern;
        EXECUTE statment;
        DROP PREPARE statment;
    END;
);

const char* QRU_CALL_DROP_PROC = SQL_COMMAND(
    CALL drop_proc (
        ?  // @_table_name VARCHAR(40) CHARACTER SET UTF8
    );
);

/**
 * PROCEDURE query_proc()
 */
QRU_TEMPLATE QRU_QUERY_PROC_PATTERN = {
    "query_proc_pattern",
    SQL_COMMAND(
        SELECT SQL_CALC_FOUND_ROWS * FROM `{TABLE_NAME}`
        WHERE
            {PID} AND
            {USER_NAME} AND
            {PROC_NAME} AND
            {PROC_APP_NAME} AND
            {LOWER_CPU_USAGE} AND
            {UPPER_CPU_USAGE} AND
            {LOWER_MEM_USAGE} AND
            {UPPER_MEM_USAGE} AND
            {LOWER_MONITOR_TIME} AND
            {UPPER_MONITOR_TIME}
        ORDER BY {ORDER_BY} {ORDER_TYPE}
        LIMIT ?, ?;
    )
};

const char* QRU_DROP_QUERY_PROC = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `query_proc`;
);

const char* QRU_QUERY_PROC = SQL_COMMAND(
    CREATE PROCEDURE
        query_proc(
			IN _pid INT UNSIGNED,
			IN _user_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _proc_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _proc_app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _lower_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _upper_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _lower_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _upper_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _lower_monitor_time INT,
			IN _upper_monitor_time INT,
            IN _table_name TINYTEXT CHARACTER SET UTF8,
            IN _order_by SMALLINT,
            IN _skip INT,
            IN _limit INT,
            OUT _total_records INT
        )
    BEGIN
        DECLARE EXIT HANDLER FOR 1146 BEGIN END;
        
        SET _total_records := 0;
        
        SET @table_name := _table_name;
        SELECT `pattern` FROM `pattern` WHERE `name` = 'query_proc_pattern' INTO @pattern;
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);

		IF _pid < 0 THEN
			SET @pattern := REPLACE(@pattern, '{PID}', '?');
			SET @pid = TRUE;
		ELSE
			SET @pattern := REPLACE(@pattern, '{PID}', '`pid` = ?');
			SET @pid = _pid;
		END IF;

        IF ISNULL(_user_name) || _user_name = '' THEN
            SET @pattern := REPLACE(@pattern, '{USER_NAME}', '?');
            SET @user_name = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{USER_NAME}', '`user_name` = ?');
            SET @user_name = _user_name;
        END IF;

        IF ISNULL(_proc_name) || _proc_name = '' THEN
            SET @pattern := REPLACE(@pattern, '{PROC_NAME}', '?');
            SET @proc_name = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{PROC_NAME}', '`proc_name` = ?');
            SET @proc_name = _proc_name;
        END IF;

        IF ISNULL(_proc_app_name) || _proc_app_name = '' THEN
            SET @pattern := REPLACE(@pattern, '{PROC_APP_NAME}', '?');
            SET @proc_app_name = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{PROC_APP_NAME}', '`proc_app_name` = ?');
            SET @proc_app_name = _proc_app_name;
        END IF;

        IF _lower_cpu_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_CPU_USAGE}', '?');
            SET @lower_cpu_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_CPU_USAGE}', '`proc_cpu_usage` >= ?');
            SET @lower_cpu_usage = _lower_cpu_usage;
        END IF;

        IF _upper_cpu_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_CPU_USAGE}', '?');
            SET @upper_cpu_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_CPU_USAGE}', '`proc_cpu_usage` <= ?');
            SET @upper_cpu_usage = _upper_cpu_usage;
        END IF;

        IF _lower_mem_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MEM_USAGE}', '?');
            SET @lower_mem_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MEM_USAGE}', '`proc_mem_usage` >= ?');
            SET @lower_mem_usage = _lower_mem_usage;
        END IF;

        IF _upper_mem_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MEM_USAGE}', '?');
            SET @upper_mem_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MEM_USAGE}', '`proc_mem_usage` <= ?');
            SET @upper_mem_usage = _upper_mem_usage;
        END IF;

        IF _lower_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '?');
            SET @lower_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '`monitor_time` >= ?');
            SET @lower_monitor_time = _lower_monitor_time;
        END IF;

        IF _upper_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '?');
            SET @upper_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '`monitor_time` <= ?');
            SET @upper_monitor_time = _upper_monitor_time;
        END IF;

        IF _order_by < 0 THEN
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'DESC');
        ELSE
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'ASC');
        END IF;
        
        CASE ABS(_order_by)
        WHEN 1 THEN SET @order_by := '`monitor_time`';
		WHEN 2 THEN SET @order_by := '`pid`';
		WHEN 3 THEN SET @order_by := '`user_name`';
		WHEN 4 THEN SET @order_by := '`proc_name`';
		WHEN 5 THEN SET @order_by := '`proc_app_name`';
		WHEN 6 THEN SET @order_by := '`proc_cpu_usage`';
		WHEN 7 THEN SET @order_by := '`proc_mem_usage`';
		WHEN 8 THEN SET @order_by := '`proc_mem_used`';
		WHEN 9 THEN SET @order_by := '`proc_tx`';
		WHEN 10 THEN SET @order_by := '`proc_rx`';
        ELSE SET @order_by := 'NULL';
        END CASE;
        
        SET @pattern := REPLACE(@pattern, '{ORDER_BY}', CONVERT(@order_by USING UTF8));
        
        IF _skip < 0 THEN
            SET @skip := 0;
        ELSE
            SET @skip := _skip;
        END IF;
        
        IF _limit <= 0 THEN
            SET @limit := 100000;
        ELSE
            SET @limit := _limit;
        END IF;
	
        PREPARE stmtment FROM @pattern;
        EXECUTE stmtment USING
			@pid,
			@user_name,
			@proc_name,
			@proc_app_name,
			@lower_cpu_usage,
			@upper_cpu_usage,
			@lower_mem_usage,
			@upper_mem_usage,
            @lower_monitor_time,
            @upper_monitor_time,
            @skip,
            @limit;
        DROP PREPARE stmtment;
        
        SET _total_records := FOUND_ROWS();
    END;
);

const char* QRU_CALL_QUERY_PROC = SQL_COMMAND(
    CALL query_proc (
		?, // @pid INT UNSIGNED
		?, // @user_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @proc_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @proc_app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @lower_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
		?, // @upper_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
		?, // @lower_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
		?, // @upper_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
        ?, // @lower_monitor_time INT
        ?, // @upper_monitor_time INT
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?, // @order_by SMALLINT
        ?, // @skip INT
        ?, // @limit INT
        ?  // $total_records INT
    );
);


/**
 * PROCEDURE update_app()
 */
const char* QRU_DROP_UPDATE_APP = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_app`;
);

const char* QRU_UPDATE_APP = SQL_COMMAND(
	CREATE PROCEDURE
		update_app(
			IN _monitor_time INT UNSIGNED DEFAULT 0,
			IN _app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _app_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _app_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _app_mem_used INT UNSIGNED DEFAULT 0,
			IN _app_proc_tx BIGINT UNSIGNED DEFAULT 0,
			IN _app_proc_rx BIGINT UNSIGNED DEFAULT 0,
			IN _table_name TINYTEXT CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @monitor_time := _monitor_time;
		SET @app_name := _app_name;
		SET @app_cpu_usage := _app_cpu_usage;
		SET @app_mem_usage := _app_mem_usage;
		SET @app_mem_used := _app_mem_used;
		SET @app_proc_tx := _app_proc_tx;
		SET @app_proc_rx := _app_proc_rx;
		SET @table_name := _table_name;

	IF _check_table_exist THEN 
		CALL table_exist(@table_name, @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `{TABLE_NAME}` (\
				`id` INT NOT NULL PRIMARY KEY AUTO_INCREMENT,\
				`monitor_time` INT UNSIGNED DEFAULT 0,\
				`app_name` VARCHAR(256) CHARACTER SET UTF8 NOT NULL,\
				`app_cpu_usage` DOUBLE(3,1) UNSIGNED DEFAULT 0.0,\
				`app_mem_usage` DOUBLE(3,1) UNSIGNED DEFAULT 0.0,\
				`app_mem_used` INT UNSIGNED DEFAULT 0,\
				`app_proc_tx` BIGINT UNSIGNED DEFAULT 0,\
				`app_proc_rx` BIGINT UNSIGNED DEFAULT 0,\
				INDEX (`id`, `monitor_time`, `app_name`)\
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'\
			";

			SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
			SET @pattern := REPLACE(@pattern, '{NEXT_ID}', '1');
			PREPARE app_table FROM @pattern;
			EXECUTE app_table;
			DROP PREPARE app_table;
		END IF;
	END IF;

	SET @pattern := "\
		INSERT INTO `{TABLE_NAME}` VALUES(\
			NULL,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?\
	)";

	SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
	PREPARE append_app FROM @pattern;
	EXECUTE append_app USING
	    @monitor_time,
	    @app_name,
	    @app_cpu_usage,
	    @app_mem_usage,
		@app_mem_used,
		@app_proc_tx,
		@app_proc_rx
	;
	DROP PREPARE append_app;
	END;
);

const char* QRU_CALL_UPDATE_APP = SQL_COMMAND(
    CALL update_app (
		?, // @monitor_time INT UNSIGNED DEFAULT 0
		?, // @app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @app_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @app_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0
		?, // @app_mem_used INT UNSIGNED DEFAULT 0
		?, // @app_proc_tx BIGINT UNSIGNED DEFAULT 0
		?, // @app_proc_rx BIGINT UNSIGNED DEFAULT 0
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?  // @check_table_exist BOOL
    );
);

/**
 * PROCEDURE drop_app()
 */
const char* QRU_DROP_DROP_APP = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `drop_app`;
);

const char* QRU_DROP_APP = SQL_COMMAND(
    CREATE PROCEDURE
        drop_app(
            IN _table_name VARCHAR(40) CHARACTER SET UTF8
        )
    BEGIN
        DECLARE CONTINUE HANDLER FOR 1051 BEGIN END;
        
        SET @pattern := CONVERT('DROP TABLE `{TABLE_NAME}' USING UTF8);
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', _table_name);

        PREPARE statment FROM @pattern;
        EXECUTE statment;
        DROP PREPARE statment;
    END;
);

const char* QRU_CALL_DROP_APP = SQL_COMMAND(
    CALL drop_app (
        ?  // @_table_name VARCHAR(40) CHARACTER SET UTF8
    );
);

/**
 * PROCEDURE query_app()
 */
QRU_TEMPLATE QRU_QUERY_APP_PATTERN = {
    "query_app_pattern",
    SQL_COMMAND(
        SELECT SQL_CALC_FOUND_ROWS * FROM `{TABLE_NAME}`
        WHERE
            {APP_NAME} AND
            {LOWER_CPU_USAGE} AND
            {UPPER_CPU_USAGE} AND
            {LOWER_MEM_USAGE} AND
            {UPPER_MEM_USAGE} AND
            {LOWER_MONITOR_TIME} AND
            {UPPER_MONITOR_TIME}
        ORDER BY {ORDER_BY} {ORDER_TYPE}
        LIMIT ?, ?;
    )
};

const char* QRU_DROP_QUERY_APP = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `query_app`;
);

const char* QRU_QUERY_APP = SQL_COMMAND(
    CREATE PROCEDURE
        query_app(
			IN _app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _lower_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _upper_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _lower_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _upper_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
			IN _lower_monitor_time INT,
			IN _upper_monitor_time INT,
            IN _table_name TINYTEXT CHARACTER SET UTF8,
            IN _order_by SMALLINT,
            IN _skip INT,
            IN _limit INT,
            OUT _total_records INT
        )
    BEGIN
        DECLARE EXIT HANDLER FOR 1146 BEGIN END;
        
        SET _total_records := 0;
        
        SET @table_name := _table_name;
        SELECT `pattern` FROM `pattern` WHERE `name` = 'query_app_pattern' INTO @pattern;
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);

        IF ISNULL(_app_name) || _app_name = '' THEN
            SET @pattern := REPLACE(@pattern, '{APP_NAME}', '?');
            SET @app_name = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{APP_NAME}', '`app_name` = ?');
            SET @app_name = _app_name;
        END IF;

        IF _lower_cpu_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_CPU_USAGE}', '?');
            SET @lower_cpu_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_CPU_USAGE}', '`app_cpu_usage` >= ?');
            SET @lower_cpu_usage = _lower_cpu_usage;
        END IF;

        IF _upper_cpu_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_CPU_USAGE}', '?');
            SET @upper_cpu_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_CPU_USAGE}', '`app_cpu_usage` <= ?');
            SET @upper_cpu_usage = _upper_cpu_usage;
        END IF;

        IF _lower_mem_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MEM_USAGE}', '?');
            SET @lower_mem_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MEM_USAGE}', '`app_mem_usage` >= ?');
            SET @lower_mem_usage = _lower_mem_usage;
        END IF;

        IF _upper_mem_usage < 0.0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MEM_USAGE}', '?');
            SET @upper_mem_usage = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MEM_USAGE}', '`app_mem_usage` <= ?');
            SET @upper_mem_usage = _upper_mem_usage;
        END IF;

        IF _lower_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '?');
            SET @lower_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '`monitor_time` >= ?');
            SET @lower_monitor_time = _lower_monitor_time;
        END IF;

        IF _upper_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '?');
            SET @upper_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '`monitor_time` <= ?');
            SET @upper_monitor_time = _upper_monitor_time;
        END IF;

        IF _order_by < 0 THEN
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'DESC');
        ELSE
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'ASC');
        END IF;
        
        CASE ABS(_order_by)
        WHEN 1 THEN SET @order_by := '`monitor_time`';
		WHEN 2 THEN SET @order_by := '`app_name`';
		WHEN 3 THEN SET @order_by := '`app_cpu_usage`';
		WHEN 4 THEN SET @order_by := '`app_mem_usage`';
		WHEN 5 THEN SET @order_by := '`app_mem_used`';
		WHEN 6 THEN SET @order_by := '`app_proc_tx`';
		WHEN 7 THEN SET @order_by := '`app_proc_rx`';
        ELSE SET @order_by := 'NULL';
        END CASE;
        
        SET @pattern := REPLACE(@pattern, '{ORDER_BY}', CONVERT(@order_by USING UTF8));
        
        IF _skip < 0 THEN
            SET @skip := 0;
        ELSE
            SET @skip := _skip;
        END IF;
        
        IF _limit <= 0 THEN
            SET @limit := 100000;
        ELSE
            SET @limit := _limit;
        END IF;
	
        PREPARE stmtment FROM @pattern;
        EXECUTE stmtment USING
			@app_name,
			@lower_cpu_usage,
			@upper_cpu_usage,
			@lower_mem_usage,
			@upper_mem_usage,
            @lower_monitor_time,
            @upper_monitor_time,
            @skip,
            @limit;
        DROP PREPARE stmtment;
        
        SET _total_records := FOUND_ROWS();
    END;
);

const char* QRU_CALL_QUERY_APP = SQL_COMMAND(
    CALL query_app (
		?, // @app_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @lower_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
		?, // @upper_cpu_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
		?, // @lower_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
		?, // @upper_mem_usage DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
        ?, // @lower_monitor_time INT
        ?, // @upper_monitor_time INT
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?, // @order_by SMALLINT
        ?, // @skip INT
        ?, // @limit INT
        ?  // $total_records INT
    );
);


/**
 * PROCEDURE update_nic()
 */
const char* QRU_DROP_UPDATE_NIC = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_nic`;
);

const char* QRU_UPDATE_NIC = SQL_COMMAND(
	CREATE PROCEDURE
		update_nic(
			IN _monitor_time INT UNSIGNED DEFAULT 0,
			IN _nic_type INT UNSIGNED DEFAULT 0,
			IN _nic_name VARCHAR(16) CHARACTER SET UTF8 NOT NULL,
			IN _nic_display_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL,
			IN _nic_member VARCHAR(256) CHARACTER SET UTF8,
			IN _nic_tx BIGINT UNSIGNED DEFAULT 0,
			IN _nic_rx BIGINT UNSIGNED DEFAULT 0,
			IN _table_name TINYTEXT CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @monitor_time := _monitor_time;
		SET @nic_type := _nic_type;
		SET @nic_name := _nic_name;
		SET @nic_display_name := _nic_display_name;
		SET @nic_member := _nic_member;
		SET @nic_tx := _nic_tx;
		SET @nic_rx := _nic_rx;
		SET @table_name := _table_name;

	IF _check_table_exist THEN 
		CALL table_exist(@table_name, @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `{TABLE_NAME}` (\
				`id` INT NOT NULL PRIMARY KEY AUTO_INCREMENT,\
				`monitor_time` INT UNSIGNED DEFAULT 0,\
				`nic_type` INT UNSIGNED DEFAULT 0,\
				`nic_name` VARCHAR(16) CHARACTER SET UTF8 NOT NULL,\
				`nic_display_name` VARCHAR(256) CHARACTER SET UTF8 NOT NULL,\
				`nic_member` VARCHAR(256) CHARACTER SET UTF8,\
				`nic_tx` BIGINT UNSIGNED DEFAULT 0,\
				`nic_rx` BIGINT UNSIGNED DEFAULT 0,\
				INDEX (`id`, `monitor_time`, `nic_type`, `nic_name`)\
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'\
			";

			SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
			SET @pattern := REPLACE(@pattern, '{NEXT_ID}', '1');
			PREPARE nic_table FROM @pattern;
			EXECUTE nic_table;
			DROP PREPARE nic_table;
		END IF;
	END IF;

	SET @pattern := "\
		INSERT INTO `{TABLE_NAME}` VALUES(\
			NULL,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?\
	)";

	SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
	PREPARE append_nic FROM @pattern;
	EXECUTE append_nic USING
	    @monitor_time,
	    @nic_type,
	    @nic_name,
	    @nic_display_name,
		@nic_member,
		@nic_tx,
		@nic_rx
	;
	DROP PREPARE append_nic;
	END;
);

const char* QRU_CALL_UPDATE_NIC = SQL_COMMAND(
    CALL update_nic (
		?, // @monitor_time INT UNSIGNED DEFAULT 0
		?, // @nic_type INT UNSIGNED DEFAULT 0
		?, // @nic_name VARCHAR(16) CHARACTER SET UTF8 NOT NULL
		?, // @nic_display_name VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @nic_member VARCHAR(256) CHARACTER SET UTF8
		?, // @nic_tx BIGINT UNSIGNED DEFAULT 0
		?, // @nic_rx BIGINT UNSIGNED DEFAULT 0
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?  // @check_table_exist BOOL
    );
);

/**
 * PROCEDURE drop_nic()
 */
const char* QRU_DROP_DROP_NIC = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `drop_nic`;
);

const char* QRU_DROP_NIC = SQL_COMMAND(
    CREATE PROCEDURE
        drop_nic(
            IN _table_name VARCHAR(40) CHARACTER SET UTF8
        )
    BEGIN
        DECLARE CONTINUE HANDLER FOR 1051 BEGIN END;
        
        SET @pattern := CONVERT('DROP TABLE `{TABLE_NAME}' USING UTF8);
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', _table_name);

        PREPARE statment FROM @pattern;
        EXECUTE statment;
        DROP PREPARE statment;
    END;
);

const char* QRU_CALL_DROP_NIC = SQL_COMMAND(
    CALL drop_nic (
        ?  // @_table_name VARCHAR(40) CHARACTER SET UTF8
    );
);

/**
 * PROCEDURE query_nic()
 */
QRU_TEMPLATE QRU_QUERY_NIC_PATTERN = {
    "query_nic_pattern",
    SQL_COMMAND(
        SELECT SQL_CALC_FOUND_ROWS * FROM `{TABLE_NAME}`
        WHERE
            {NIC_TYPE} AND
            {NIC_NAME} AND
            {LOWER_MONITOR_TIME} AND
            {UPPER_MONITOR_TIME}
        ORDER BY {ORDER_BY} {ORDER_TYPE}
        LIMIT ?, ?;
    )
};

const char* QRU_DROP_QUERY_NIC = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `query_nic`;
);

const char* QRU_QUERY_NIC = SQL_COMMAND(
    CREATE PROCEDURE
        query_nic(
			IN _nic_type INT UNSIGNED DEFAULT 0,
			IN _nic_name VARCHAR(16) CHARACTER SET UTF8 NOT NULL,
			IN _lower_monitor_time INT,
			IN _upper_monitor_time INT,
            IN _table_name TINYTEXT CHARACTER SET UTF8,
            IN _order_by SMALLINT,
            IN _skip INT,
            IN _limit INT,
            OUT _total_records INT
        )
    BEGIN
        DECLARE EXIT HANDLER FOR 1146 BEGIN END;
        
        SET _total_records := 0;
        
        SET @table_name := _table_name;
        SELECT `pattern` FROM `pattern` WHERE `name` = 'query_nic_pattern' INTO @pattern;
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);

		IF _nic_type < 0 THEN
			SET @pattern := REPLACE(@pattern, '{NIC_TYPE}', '?');
			SET @nic_type = TRUE;
		ELSE
			SET @pattern := REPLACE(@pattern, '{NIC_TYPE}', '`nic_type` = ?');
			SET @nic_type = _nic_type;
		END IF;

        IF ISNULL(_nic_name) || _nic_name = '' THEN
            SET @pattern := REPLACE(@pattern, '{NIC_NAME}', '?');
            SET @nic_name = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{NIC_NAME}', '`nic_name` = ?');
            SET @nic_name = _nic_name;
        END IF;

        IF _lower_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '?');
            SET @lower_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '`monitor_time` >= ?');
            SET @lower_monitor_time = _lower_monitor_time;
        END IF;

        IF _upper_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '?');
            SET @upper_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '`monitor_time` <= ?');
            SET @upper_monitor_time = _upper_monitor_time;
        END IF;

        IF _order_by < 0 THEN
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'DESC');
        ELSE
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'ASC');
        END IF;

        CASE ABS(_order_by)
        WHEN 1 THEN SET @order_by := '`monitor_time`';
		WHEN 2 THEN SET @order_by := '`nic_type`';
		WHEN 3 THEN SET @order_by := '`nic_name`';
		WHEN 4 THEN SET @order_by := '`nic_display_name`';
		WHEN 5 THEN SET @order_by := '`nic_member`';
		WHEN 6 THEN SET @order_by := '`nic_tx`';
		WHEN 7 THEN SET @order_by := '`nic_rx`';
        ELSE SET @order_by := 'NULL';
        END CASE;
        
        SET @pattern := REPLACE(@pattern, '{ORDER_BY}', CONVERT(@order_by USING UTF8));
        
        IF _skip < 0 THEN
            SET @skip := 0;
        ELSE
            SET @skip := _skip;
        END IF;
        
        IF _limit <= 0 THEN
            SET @limit := 100000;
        ELSE
            SET @limit := _limit;
        END IF;
	
        PREPARE stmtment FROM @pattern;
        EXECUTE stmtment USING
			@nic_type,
			@nic_name,
            @lower_monitor_time,
            @upper_monitor_time,
            @skip,
            @limit;
        DROP PREPARE stmtment;
        
        SET _total_records := FOUND_ROWS();
    END;
);

const char* QRU_CALL_QUERY_NIC = SQL_COMMAND(
    CALL query_nic (
		?, // @nic_type VARCHAR(256) CHARACTER SET UTF8 NOT NULL
		?, // @nic_name DOUBLE(3,1) UNSIGNED DEFAULT 0.0,
        ?, // @lower_monitor_time INT
        ?, // @upper_monitor_time INT
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?, // @order_by SMALLINT
        ?, // @skip INT
        ?, // @limit INT
        ?  // $total_records INT
    );
);


const char* QRU_CREATE_POOL_DAILY_TABLE = SQL_COMMAND(
	CREATE PROCEDURE
		create_pool_daily_table(
			IN _daily_name VARCHAR(40) CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @daily_name := _daily_name;

	IF _check_table_exist THEN 
		CALL table_exist(CONCAT('pool_daily#', @daily_name), @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `pool_daily#{DAILY_NAME}` (
				`record_time` INT UNSIGNED DEFAULT 0,
				`pool_id` INT UNSIGNED,
				`pool_name` VARCHAR(64) CHARACTER SET UTF8 NOT NULL,
				`read_iops` INT UNSIGNED DEFAULT 0,
				`write_iops` INT UNSIGNED DEFAULT 0,
				`read_latency` INT UNSIGNED DEFAULT 0,
				`write_latency` INT UNSIGNED DEFAULT 0,
				`read_throughput` INT UNSIGNED DEFAULT 0,
				`write_throughput` INT UNSIGNED DEFAULT 0
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'
			";

			SET @pattern := REPLACE(@pattern, '{DAILY_NAME}', @daily_name);
			PREPARE pool_daily_table FROM @pattern;
			EXECUTE pool_daily_table;
			DROP PREPARE pool_daily_table;
		END IF;
	END IF;
    END;
);


/**
 * PROCEDURE update_pool()
 */
const char* QRU_DROP_UPDATE_POOL = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `update_pool`;
);

const char* QRU_UPDATE_POOL = SQL_COMMAND(
	CREATE PROCEDURE
		update_pool(
			IN _monitor_time INT UNSIGNED DEFAULT 0,
			IN _pool_id INT UNSIGNED,
			IN _pool_name VARCHAR(64) CHARACTER SET UTF8 NOT NULL,
			IN _read_iops INT UNSIGNED DEFAULT 0,
			IN _write_iops INT UNSIGNED DEFAULT 0,
			IN _read_latency INT UNSIGNED DEFAULT 0,
			IN _write_latency INT UNSIGNED DEFAULT 0,
			IN _read_throughput INT UNSIGNED DEFAULT 0,
			IN _write_throughput INT UNSIGNED DEFAULT 0,
			IN _table_name TINYTEXT CHARACTER SET UTF8,
			IN _check_table_exist BOOL
		)
	BEGIN
		DECLARE EXIT HANDLER FOR 1452 BEGIN END;
		SET @monitor_time := _monitor_time;
		SET @pool_id := _pool_id;
		SET @pool_name := _pool_name;
		SET @read_iops := _read_iops;
		SET @write_iops := _write_iops;
		SET @read_latency := _read_latency;
		SET @write_latency := _write_latency;
		SET @read_throughput := _read_throughput;
		SET @write_throughput := _write_throughput;
		SET @table_name := _table_name;

	IF _check_table_exist THEN 
		CALL table_exist(@table_name, @table_exist);

		IF NOT @table_exist THEN
			SET @pattern="
			CREATE TABLE IF NOT EXISTS `{TABLE_NAME}` (\
				`id` INT NOT NULL PRIMARY KEY AUTO_INCREMENT,\
				`monitor_time` INT UNSIGNED DEFAULT 0,\
				`pool_id` INT UNSIGNED,\
				`pool_name` VARCHAR(64) CHARACTER SET UTF8 NOT NULL,\
				`read_iops` INT UNSIGNED DEFAULT 0,\
				`write_iops` INT UNSIGNED DEFAULT 0,\
				`read_latency` INT UNSIGNED DEFAULT 0,\
				`write_latency` INT UNSIGNED DEFAULT 0,\
				`read_throughput` INT UNSIGNED DEFAULT 0,\
				`write_throughput` INT UNSIGNED DEFAULT 0,\
				INDEX (`id`, `monitor_time`, `pool_id`, `pool_name`)\
			) AUTO_INCREMENT={NEXT_ID} DEFAULT CHARSET=utf8 COMMENT='1'\
			";

			SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
			SET @pattern := REPLACE(@pattern, '{NEXT_ID}', '1');
			PREPARE pool_table FROM @pattern;
			EXECUTE pool_table;
			DROP PREPARE pool_table;
		END IF;
	END IF;

	SET @pattern := "\
		INSERT INTO `{TABLE_NAME}` VALUES(\
			NULL,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?,\
			?\
	)";

	SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);
	PREPARE append_pool FROM @pattern;
	EXECUTE append_pool USING
	    @monitor_time,
	    @pool_id,
	    @pool_name,
	    @read_iops,
		@write_iops,
		@read_latency,
		@write_latency,
		@read_throughput,
		@write_throughput
	;
	DROP PREPARE append_pool;
	END;
);

const char* QRU_CALL_UPDATE_POOL = SQL_COMMAND(
    CALL update_pool (
		?, // @monitor_time INT UNSIGNED DEFAULT 0
		?, // @pool_id INT UNSIGNED
		?, // @pool_name VARCHAR(64) CHARACTER SET UTF8 NOT NULL
		?, // @read_iops INT UNSIGNED DEFAULT 0
		?, // @write_iops INT UNSIGNED DEFAULT 0
		?, // @read_latency INT UNSIGNED DEFAULT 0
		?, // @write_latency INT UNSIGNED DEFAULT 0
		?, // @read_throughput INT UNSIGNED DEFAULT 0
		?, // @write_throughput INT UNSIGNED DEFAULT 0
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?  // @check_table_exist BOOL
    );
);

/**
 * PROCEDURE drop_pool()
 */
const char* QRU_DROP_DROP_POOL = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `drop_pool`;
);

const char* QRU_DROP_POOL = SQL_COMMAND(
    CREATE PROCEDURE
        drop_pool(
            IN _table_name VARCHAR(40) CHARACTER SET UTF8
        )
    BEGIN
        DECLARE CONTINUE HANDLER FOR 1051 BEGIN END;
        
        SET @pattern := CONVERT('DROP TABLE `{TABLE_NAME}' USING UTF8);
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', _table_name);

        PREPARE statment FROM @pattern;
        EXECUTE statment;
        DROP PREPARE statment;
    END;
);

const char* QRU_CALL_DROP_POOL = SQL_COMMAND(
    CALL drop_pool (
        ?  // @_table_name VARCHAR(40) CHARACTER SET UTF8
    );
);

/**
 * PROCEDURE query_pool()
 */
QRU_TEMPLATE QRU_QUERY_POOL_PATTERN = {
    "query_pool_pattern",
    SQL_COMMAND(
        SELECT SQL_CALC_FOUND_ROWS * FROM `{TABLE_NAME}`
        WHERE
            {POOL_ID} AND
            {POOL_NAME} AND
            {LOWER_MONITOR_TIME} AND
            {UPPER_MONITOR_TIME}
        ORDER BY {ORDER_BY} {ORDER_TYPE}
        LIMIT ?, ?;
    )
};

const char* QRU_DROP_QUERY_POOL = SQL_COMMAND(
    DROP PROCEDURE IF EXISTS `query_pool`;
);

const char* QRU_QUERY_POOL = SQL_COMMAND(
    CREATE PROCEDURE
        query_pool(
			IN _pool_id INT UNSIGNED,
			IN _pool_name VARCHAR(64) CHARACTER SET UTF8 NOT NULL,
			IN _lower_monitor_time INT,
			IN _upper_monitor_time INT,
            IN _table_name TINYTEXT CHARACTER SET UTF8,
            IN _order_by SMALLINT,
            IN _skip INT,
            IN _limit INT,
            OUT _total_records INT
        )
    BEGIN
        DECLARE EXIT HANDLER FOR 1146 BEGIN END;
        
        SET _total_records := 0;
        
        SET @table_name := _table_name;
        SELECT `pattern` FROM `pattern` WHERE `name` = 'query_pool_pattern' INTO @pattern;
        SET @pattern := REPLACE(@pattern, '{TABLE_NAME}', @table_name);

		IF _pool_id < 0 THEN
			SET @pattern := REPLACE(@pattern, '{POOL_ID}', '?');
			SET @pool_id = TRUE;
		ELSE
			SET @pattern := REPLACE(@pattern, '{POOL_ID}', '`pool_id` = ?');
			SET @pool_id = _pool_id;
		END IF;

        IF ISNULL(_pool_name) || _pool_name = '' THEN
            SET @pattern := REPLACE(@pattern, '{POOL_NAME}', '?');
            SET @pool_name = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{POOL_NAME}', '`pool_name` = ?');
            SET @pool_name = _pool_name;
        END IF;

        IF _lower_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '?');
            SET @lower_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{LOWER_MONITOR_TIME}', '`monitor_time` >= ?');
            SET @lower_monitor_time = _lower_monitor_time;
        END IF;

        IF _upper_monitor_time < 0 THEN
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '?');
            SET @upper_monitor_time = TRUE;
        ELSE
            SET @pattern := REPLACE(@pattern, '{UPPER_MONITOR_TIME}', '`monitor_time` <= ?');
            SET @upper_monitor_time = _upper_monitor_time;
        END IF;

        IF _order_by < 0 THEN
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'DESC');
        ELSE
            SET @pattern := REPLACE(@pattern, '{ORDER_TYPE}', 'ASC');
        END IF;

        CASE ABS(_order_by)
        WHEN 1 THEN SET @order_by := '`monitor_time`';
		WHEN 2 THEN SET @order_by := '`pool_id`';
		WHEN 3 THEN SET @order_by := '`pool_name`';
		WHEN 4 THEN SET @order_by := '`read_iops`';
		WHEN 5 THEN SET @order_by := '`write_iops`';
		WHEN 6 THEN SET @order_by := '`read_latency`';
		WHEN 7 THEN SET @order_by := '`write_latency`';
		WHEN 8 THEN SET @order_by := '`read_throughput`';
		WHEN 9 THEN SET @order_by := '`write_throughput`';
        ELSE SET @order_by := 'NULL';
        END CASE;
        
        SET @pattern := REPLACE(@pattern, '{ORDER_BY}', CONVERT(@order_by USING UTF8));
        
        IF _skip < 0 THEN
            SET @skip := 0;
        ELSE
            SET @skip := _skip;
        END IF;
        
        IF _limit <= 0 THEN
            SET @limit := 100000;
        ELSE
            SET @limit := _limit;
        END IF;
	
        PREPARE stmtment FROM @pattern;
        EXECUTE stmtment USING
			@pool_id,
			@pool_name,
            @lower_monitor_time,
            @upper_monitor_time,
            @skip,
            @limit;
        DROP PREPARE stmtment;
        
        SET _total_records := FOUND_ROWS();
    END;
);

const char* QRU_CALL_QUERY_POOL = SQL_COMMAND(
    CALL query_pool (
		?, // @pool_id INT UNSIGNED
		?, // @pool_name VARCHAR(64) CHARACTER SET UTF8 NOT NULL
        ?, // @lower_monitor_time INT
        ?, // @upper_monitor_time INT
        ?, // @table_name VARCHAR(40) CHARACTER SET UTF8
        ?, // @order_by SMALLINT
        ?, // @skip INT
        ?, // @limit INT
        ?  // $total_records INT
    );
);


#undef SQL_COMMAND

#endif

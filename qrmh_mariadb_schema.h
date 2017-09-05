#ifndef __QRMH_MARIADB_SCHEMA_H__
#define __QRMH_MARIADB_SCHEMA_H__

#ifndef SQL_COMMAND
#define SQL_COMMAND(args...) #args
#endif

const char* QRMH_REPAIR_MYSQL_PROC = SQL_COMMAND(
	REPAIR TABLE `mysql`.`proc`;
);

const char* QRMH_CREATE_DB = SQL_COMMAND(
	CREATE DATABASE IF NOT EXISTS `qrmh`;
);

const char* QRMH_REPAIR_TABLE = SQL_COMMAND(
    REPAIR TABLE `qrmh`.`cpu_day`;
    REPAIR TABLE `qrmh`.`cpu_week`;
    REPAIR TABLE `qrmh`.`cpu_month`;
    REPAIR TABLE `qrmh`.`cpu_year`;
    REPAIR TABLE `qrmh`.`mem_day`;
    REPAIR TABLE `qrmh`.`mem_week`;
    REPAIR TABLE `qrmh`.`mem_month`;
    REPAIR TABLE `qrmh`.`mem_year`;
    REPAIR TABLE `qrmh`.`proc_day`;
    REPAIR TABLE `qrmh`.`proc_week`;
    REPAIR TABLE `qrmh`.`proc_month`;
    REPAIR TABLE `qrmh`.`proc_year`;
);

const char* QRMH_CHANGE_DB = SQL_COMMAND(
	USE `qrmh`;
);

const char* QRMH_START_TRANSACTION = SQL_COMMAND(
	START TRANSACTION;
);

const char* QRMH_CREATE_CPU_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_day` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`total_usage` INT UNSIGNED,
		`user_usage` INT UNSIGNED,
		`sys_usage` INT UNSIGNED,
		`other_usage` INT UNSIGNED,
		`iowait_usage` INT UNSIGNED,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);












#undef SQL_COMMAND

#endif

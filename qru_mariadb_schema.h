#ifndef __QRU_MARIADB_SCHEMA_H__
#define __QRU_MARIADB_SCHEMA_H__

#ifndef SQL_COMMAND
#define SQL_COMMAND(args...) #args
#endif

const char* QRU_REPAIR_MYSQL_PROC = SQL_COMMAND(
	REPAIR TABLE `mysql`.`proc`;
);

const char* QRU_CREATE_DB = SQL_COMMAND(
	CREATE DATABASE IF NOT EXISTS `qru`;
);

const char* QRU_REPAIR_TABLE = SQL_COMMAND(
    REPAIR TABLE `qru`.`cpu_day`;
    REPAIR TABLE `qru`.`cpu_week`;
    REPAIR TABLE `qru`.`cpu_month`;
    REPAIR TABLE `qru`.`cpu_year`;
    REPAIR TABLE `qru`.`mem_day`;
    REPAIR TABLE `qru`.`mem_week`;
    REPAIR TABLE `qru`.`mem_month`;
    REPAIR TABLE `qru`.`mem_year`;
    REPAIR TABLE `qru`.`proc_day`;
    REPAIR TABLE `qru`.`proc_week`;
    REPAIR TABLE `qru`.`proc_month`;
    REPAIR TABLE `qru`.`proc_year`;
);

const char* QRU_CHANGE_DB = SQL_COMMAND(
	USE `qru`;
);

const char* QRU_START_TRANSACTION = SQL_COMMAND(
	START TRANSACTION;
);

const char* QRU_CREATE_CPU_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_day` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`total_usage` INT UNSIGNED DEFAULT 0,
		`user_usage` INT UNSIGNED DEFAULT 0,
		`sys_usage` INT UNSIGNED DEFAULT 0,
		`other_usage` INT UNSIGNED DEFAULT 0,
		`iowait_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_CPU_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_week` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`total_usage` INT UNSIGNED DEFAULT 0,
		`user_usage` INT UNSIGNED DEFAULT 0,
		`sys_usage` INT UNSIGNED DEFAULT 0,
		`other_usage` INT UNSIGNED DEFAULT 0,
		`iowait_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_CPU_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_month` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`total_usage` INT UNSIGNED DEFAULT 0,
		`user_usage` INT UNSIGNED DEFAULT 0,
		`sys_usage` INT UNSIGNED DEFAULT 0,
		`other_usage` INT UNSIGNED DEFAULT 0,
		`iowait_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_CPU_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_year` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`total_usage` INT UNSIGNED DEFAULT 0,
		`user_usage` INT UNSIGNED DEFAULT 0,
		`sys_usage` INT UNSIGNED DEFAULT 0,
		`other_usage` INT UNSIGNED DEFAULT 0,
		`iowait_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_day` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`mem_used` INT UNSIGNED DEFAULT 0,
		`mem_buf` INT UNSIGNED DEFAULT 0,
		`mem_cache` INT UNSIGNED DEFAULT 0,
		`mem_free` INT UNSIGNED DEFAULT 0,
		`swap_used` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_week` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`mem_used` INT UNSIGNED DEFAULT 0,
		`mem_buf` INT UNSIGNED DEFAULT 0,
		`mem_cache` INT UNSIGNED DEFAULT 0,
		`mem_free` INT UNSIGNED DEFAULT 0,
		`swap_used` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_month` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`mem_used` INT UNSIGNED DEFAULT 0,
		`mem_buf` INT UNSIGNED DEFAULT 0,
		`mem_cache` INT UNSIGNED DEFAULT 0,
		`mem_free` INT UNSIGNED DEFAULT 0,
		`swap_used` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_year` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`mem_used` INT UNSIGNED DEFAULT 0,
		`mem_buf` INT UNSIGNED DEFAULT 0,
		`mem_cache` INT UNSIGNED DEFAULT 0,
		`mem_free` INT UNSIGNED DEFAULT 0,
		`swap_used` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_day` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_state` VARCHAR(4) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_week` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_state` VARCHAR(4) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_week` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_state` VARCHAR(4) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_week` (
		`recode_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_state` VARCHAR(4) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);






#undef SQL_COMMAND

#endif

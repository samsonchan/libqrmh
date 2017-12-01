#ifndef __QRU_MARIADB_SCHEMA_H__
#define __QRU_MARIADB_SCHEMA_H__

#ifndef SQL_COMMAND
#define SQL_COMMAND(args...) #args
#endif

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
    REPAIR TABLE `qru`.`nic_day`;
    REPAIR TABLE `qru`.`nic_week`;
    REPAIR TABLE `qru`.`nic_month`;
    REPAIR TABLE `qru`.`nic_year`;
    REPAIR TABLE `qru`.`pool_day`;
    REPAIR TABLE `qru`.`pool_week`;
    REPAIR TABLE `qru`.`pool_month`;
    REPAIR TABLE `qru`.`pool_year`;
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
		`record_time` INT UNSIGNED DEFAULT 0,
		`cpu_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_CPU_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_week` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`cpu_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_CPU_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_month` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`cpu_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_CPU_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `cpu_year` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`cpu_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_day` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`mem_used_usage` INT UNSIGNED DEFAULT 0,
		`mem_buf_usage` INT UNSIGNED DEFAULT 0,
		`mem_cache_usage` INT UNSIGNED DEFAULT 0,
		`mem_free_usage` INT UNSIGNED DEFAULT 0,
		`swap_used_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_week` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`mem_used_usage` INT UNSIGNED DEFAULT 0,
		`mem_buf_usage` INT UNSIGNED DEFAULT 0,
		`mem_cache_usage` INT UNSIGNED DEFAULT 0,
		`mem_free_usage` INT UNSIGNED DEFAULT 0,
		`swap_used_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_month` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`mem_used_usage` INT UNSIGNED DEFAULT 0,
		`mem_buf_usage` INT UNSIGNED DEFAULT 0,
		`mem_cache_usage` INT UNSIGNED DEFAULT 0,
		`mem_free_usage` INT UNSIGNED DEFAULT 0,
		`swap_used_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_MEM_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `mem_year` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`mem_used_usage` INT UNSIGNED DEFAULT 0,
		`mem_buf_usage` INT UNSIGNED DEFAULT 0,
		`mem_cache_usage` INT UNSIGNED DEFAULT 0,
		`mem_free_usage` INT UNSIGNED DEFAULT 0,
		`swap_used_usage` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_NIC_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `nic_day` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`nic_name` VARCHAR(32) CHARACTER SET UTF8 NOT NULL,
		`nic_display_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`nic_tx` BIGINT UNSIGNED DEFAULT 0,
		`nic_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_NIC_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `nic_week` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`nic_name` VARCHAR(32) CHARACTER SET UTF8 NOT NULL,
		`nic_display_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`nic_tx` BIGINT UNSIGNED DEFAULT 0,
		`nic_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_NIC_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `nic_month` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`nic_name` VARCHAR(32) CHARACTER SET UTF8 NOT NULL,
		`nic_display_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`nic_tx` BIGINT UNSIGNED DEFAULT 0,
		`nic_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_NIC_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `nic_year` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`nic_name` VARCHAR(32) CHARACTER SET UTF8 NOT NULL,
		`nic_display_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`nic_tx` BIGINT UNSIGNED DEFAULT 0,
		`nic_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_POOL_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `pool_day` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pool_id` INT DEFAULT -1,
		`read_iops` INT UNSIGNED DEFAULT 0,
		`write_iops` INT UNSIGNED DEFAULT 0,
		`read_latency` INT UNSIGNED DEFAULT 0,
		`write_latency` INT UNSIGNED DEFAULT 0,
		`read_throughput` INT UNSIGNED DEFAULT 0,
		`write_throughput` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_POOL_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `pool_week` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pool_id` INT DEFAULT -1,
		`read_iops` INT UNSIGNED DEFAULT 0,
		`write_iops` INT UNSIGNED DEFAULT 0,
		`read_latency` INT UNSIGNED DEFAULT 0,
		`write_latency` INT UNSIGNED DEFAULT 0,
		`read_throughput` INT UNSIGNED DEFAULT 0,
		`write_throughput` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_POOL_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `pool_month` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pool_id` INT DEFAULT -1,
		`read_iops` INT UNSIGNED DEFAULT 0,
		`write_iops` INT UNSIGNED DEFAULT 0,
		`read_latency` INT UNSIGNED DEFAULT 0,
		`write_latency` INT UNSIGNED DEFAULT 0,
		`read_throughput` INT UNSIGNED DEFAULT 0,
		`write_throughput` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_POOL_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `pool_year` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pool_id` INT DEFAULT -1,
		`read_iops` INT UNSIGNED DEFAULT 0,
		`write_iops` INT UNSIGNED DEFAULT 0,
		`read_latency` INT UNSIGNED DEFAULT 0,
		`write_latency` INT UNSIGNED DEFAULT 0,
		`read_throughput` INT UNSIGNED DEFAULT 0,
		`write_throughput` INT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_DAY_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_day` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_WEEK_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_week` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_MONTH_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_month` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);

const char* QRU_CREATE_PROC_YEAR_TABLE = SQL_COMMAND(
	CREATE TABLE IF NOT EXISTS `proc_year` (
		`record_time` INT UNSIGNED DEFAULT 0,
		`pid` INT DEFAULT -1,
		`user_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_app_name` VARCHAR(255) CHARACTER SET UTF8 NOT NULL,
		`proc_cpu_us` INT UNSIGNED DEFAULT 0,
		`proc_mem_us` INT UNSIGNED DEFAULT 0,
		`proc_tx` BIGINT UNSIGNED DEFAULT 0,
		`proc_rx` BIGINT UNSIGNED DEFAULT 0,
	) DEFAULT CHARSET=utf8, ENGINE=INNODB;
);






#undef SQL_COMMAND

#endif

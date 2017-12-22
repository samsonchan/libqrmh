
typedef struct QRU_CPU_INFO{
	unsigned long long monitor_time;
	double	cpu_usage; /* (%) */
}QRU_CPU_INFO;

typedef struct QRU_MEM_INFO{
	unsigned long long monitor_time;
	double	mem_usage; /* (%) */
	uint mem_used;	/* (KB) */
}QRU_MEM_INFO;

typedef struct QRU_PROC_INFO{
	unsigned long long monitor_time;
	uint pid;
	char user[256];
	char proc_name[256];
    char proc_app_name[256];
	double proc_cpu_usage; /* (%) */
	double proc_mem_usage;	/* (%) */
	uint proc_mem_used;	/* (KB) */
    unsigned long long proc_tx; /* (byte/s) */
    unsigned long long proc_rx; /* (byte/s) */
}QRU_PROC_INFO;

typedef struct QRU_APP_INFO{
	unsigned long long monitor_time;
    char app_name[256];
	double app_cpu_usage; /* (%) */
	double app_mem_usage; /* (%) */
	uint app_mem_used;	/* (KB) */
    unsigned long long app_proc_tx; /* (byte/s) */
    unsigned long long app_proc_rx; /* (byte/s) */
}QRU_APP_INFO;

typedef enum QRU_INTERFACE_TYPE{
	QRU_ETHERNET=0,
	QRU_WLAN,
	QRU_BONDING,
	QRU_THUNDERBOLT,
	QRU_QAPORT,
	QRU_BRIDGE,
	QRU_NONE
}QRU_INTERFACE_TYPE;

typedef struct QRU_NIC_INFO{
	unsigned long long monitor_time;
	QRU_INTERFACE_TYPE nic_type;
	char nic_name[16];
	char nic_display_name[256];
	char nic_member[256];
    unsigned long long nic_tx; /* (byte/s) */
    unsigned long long nic_rx; /* (byte/s) */
}QRU_NIC_INFO;


typedef struct QRU_POOL_INFO{
	unsigned long long monitor_time;
	int pool_id;
	char pool_name[64];
    int rd_iops; /* (io/s) */
    int wr_iops; /* (io/s) */
    int rd_latency; /* (ms) */
    int wr_latency; /* (ms) */
    int rd_throughput; /* (KB/s) */
    int wr_throughput; /* (KB/s) */
}QRU_POOL_INFO;






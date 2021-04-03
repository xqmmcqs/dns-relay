#if !defined(__RUNTIME_H__)
#define __RUNTIME_H__

// 处理参数

// 上一级 DNS 服务器 IP
char* src_host;
// 配置文件名
char* config_filename;
/**
 * 0: trace
 * 1: debug
 * 2: info
 * 3: error
 * 4: fatal
 */
int output_level = 0;

#endif  // __RUNTIME_H__

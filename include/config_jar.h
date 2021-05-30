/**
 * @file      config_jar.h
 * @brief     命令行参数解析
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件中声明了五个参数，以及一个分析命令行参数的函数接口。
*/

#ifndef DNSR_CONFIG_JAR_H
#define DNSR_CONFIG_JAR_H

extern char * REMOTE_HOST; ///< 远程DNS服务器地址
extern int LOG_MASK; ///< log打印等级，一个四位二进制数，从低位到高位依次表示DEBUG、INFO、ERROR、FATAL
extern int CLIENT_PORT; ///< 本地DNS客户端端口
extern char * HOSTS_PATH; ///< hosts文件路径
extern char * LOG_PATH; ///< 日志文件路径

/**
 * @brief 解析命令行参数
 *
 * @param argc 参数个数
 * @param argv 参数字符串的数组
 */
void init_config(int argc, char * const * argv);

#endif //DNSR_CONFIG_JAR_H

//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_CONFIG_JAR_H
#define DNSR_CONFIG_JAR_H

extern char * REMOTE_HOST;
extern int LOG_MASK;
extern int CLIENT_PORT;
extern char * HOSTS_PATH;
extern char * LOG_PATH;

/**
 * @brief pconfig_jar 构造函数
 *
 * @param argc
 * @param argv
 * @return pconfig_jar
 */
void init_config(int argc, char * const * argv);

#endif //DNSR_CONFIG_JAR_H

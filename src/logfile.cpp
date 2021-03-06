﻿#include <iostream>
#include <string>
#include <time.h>

#ifdef __unix
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <direct.h>
#include <io.h>
#endif

#include "logfile.h"

namespace llog {
#ifdef __unix
static const char logdir[] = "/work/log/";
#else
static const char logdir[] = "D:\\log\\";
#endif

LogFile::LogFile()
//: mutex_()
{
    time_t timep;
    struct tm* pTm;
    time(&timep);
    pTm = localtime(&timep);

    date_.day_ = pTm->tm_mday;
    date_.month_ = pTm->tm_mon + 1;
    date_.year_ = pTm->tm_year + 1900;

    createdir(); //创建存放Log的目录
    openfile(); //打开对应日志文件
}

LogFile::~LogFile()
{
}

void LogFile::writelog(const char* msg, int len)
{
    LogFile::instance()->writelogfile(msg, len);
}

// 单例缓存清空功能
void LogFile::flushbuffer()
{
    LogFile::instance()->flushlogfile();
}

// 写日志
void LogFile::writelogfile(const char* msg, int len)
{
    //boost::mutex::scoped_lock lock(mutex_);
    if (file_ != NULL) {
        ::fwrite(msg, 1, len, file_);
    }
}

// 清空缓存
void LogFile::flushlogfile()
{
    //boost::mutex::scoped_lock lock(mutex_);
    ::fflush(file_);
}

// 单例模式
LogFile* LogFile::instance()
{
    static LogFile inst;
    return &inst;
}

void LogFile::createdir()
{
#ifdef __unix
    if (access(logdir, F_OK) == -1) {
        //不存在这个目录，创建它
        mkdir(logdir, 0777);
    }
#else
	if (access(logdir, 0) == -1) {
		mkdir(logdir);
		// rmdir(const char *_Path);
	}
#endif
}

// 获取日志文件名
std::string LogFile::getfilename(void)
{
    char buffer[64]; //绝对路径文件名
    memset(buffer, 0x00, sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d-Log.txt",
        date_.year_, date_.month_, date_.day_);

    return buffer;
}

// 打开日志文件
void LogFile::openfile()
{
#ifndef _unix
    char buffer[64]; //绝对文件名
    //std::string fileName = getLogFileName();
    std::string fileName = getfilename();
    memset(buffer, 0x00, sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%s/%s", logdir, fileName.c_str());

    //boost::mutex::scoped_lock lock(mutex_);
    file_ = fopen(buffer, "a");
#else
    // Q_OS_WIN
    std::cout << "open file!\n";
#endif
}

// 如果是新的一天则重新创建日志文件并更新日期结构体
void LogFile::updatefile(Date* date)
{
#ifndef __unix
    if (date == NULL)
        return;

    if (date_.year_ != date->year_ || date_.month_ != date->month_ || date_.day_ != date->day_) {
        //新的一天开始了，则关闭之前的，重新创建个对应日期的日志文件。
        date_.year_ = date->year_;
        date_.month_ = date->month_;
        date_.day_ = date->day_;

        flushlogfile();
        fclose(file_);
        file_ = NULL;

        openfile();
    }
#else

#endif
}
}

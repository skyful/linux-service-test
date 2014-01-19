/*************************************************************************
    > File Name: ConfigFile.h
    > Author: skyful
    > Mail: wtl1989371@hotmail.com 
    > Created Time: 2014年01月19日 星期日 10时20分03秒
 ************************************************************************/
#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include<map>
#define LINE_LEN 256
using namespace std;
class ConfigFile
{
public:
	ConfigFile();
	~ConfigFile();
public:
	bool open(const string path);
	bool save();
	void close();
	string get_string(const string key);
	int  get_int(const string key);
	bool set_string(const string key,const string value);
	bool set_int(const string key,const int value); 
private:
	void trim_left_right(string &str);
	void set_vector_value(const string key,const string value);
private:
	 vector<string> m_data;
	 map<string,string> m_map;
	 string         m_path;
};
#endif

/*************************************************************************
    > File Name: configfile_test.cpp
    > Author: skyful
    > Mail: wtl1989371@hotmail.com 
    > Created Time: 2014年01月20日 星期一 23时10分20秒
 ************************************************************************/

#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<iostream>
#include"ConfigFile.h"
using namespace std;
int main()
{
	ConfigFile conf;
	conf.open(".conf");
	cout<<"value1:"<<conf.get_int("value1")<<endl;
	cout<<"value2:"<<conf.get_int("value2")<<endl;
	cout<<"value3:"<<conf.get_string("value3")<<endl;
	conf.set_int("value1",1);
	conf.set_int("value2",2);
	conf.set_string("value3","con test");
	conf.save();
	conf.close();
}


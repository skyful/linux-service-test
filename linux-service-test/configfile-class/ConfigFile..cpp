/*************************************************************************
    > File Name: ConfigFile..cpp
    > Author: skyful
    > Mail: wtl1989371@hotmail.com 
    > Created Time: 2014年01月19日 星期日 11时19分14秒
 ************************************************************************/
#include "ConfigFile.h"
ConfigFile::ConfigFile()
{

}
ConfigFile::~ConfigFile()
{

}
bool ConfigFile::open(const string path)
{
	int ret=false;
	if(path.empty())
		return false;
	m_path=path;
	ifstream file_in(m_path.c_str(),ios_base::in);
	if(file_in.is_open())
	{
		char szBuff[LINE_LEN+1];
		int len=0,pos=0;
		string str;
		bzero(szBuff,LINE_LEN);
		while(file_in.getline(szBuff,LINE_LEN))
		{
			len=strlen(szBuff);
			if(len>0)
			{
				str.assign(szBuff,len);
				m_data.push_back(str);
				//解析一行数据×××=××××
				trim_left_right(str);
				if(str[0]!='#')
				{
					string left,right;
					pos=str.find('=');
					if(pos>=0)
					{
						left=str.substr(0,pos);
						right=str.substr(pos+1);
						trim_left_right(left);
						trim_left_right(right);
						m_map.insert(pair<string,string>(left,right));
					}
				}
			}
			bzero(szBuff,LINE_LEN);
		}
		file_in.close();	
		ret=true;
	}
	return ret;
}
 bool ConfigFile::save()
{
	bool ret=false;
	ofstream file_out(m_path.c_str(),ios_base::out);
	if(file_out.is_open())
	{
		vector<string>::iterator iter;
		for(iter=m_data.begin();iter!=m_data.end();++iter)
		{
			string str=(*iter);
			file_out<<str<<endl;
		}
		file_out.close();
		ret=true;
	}
	return ret;
}
void ConfigFile::close()
{
	if(!m_data.empty())
		m_data.clear();
	if(!m_map.empty())
		m_map.clear();
}
string ConfigFile::get_string(const string key)
{
	string str("");
	if(!m_map.empty())
	{
		str=m_map[key];
	}
	return str;
}
int ConfigFile::get_int(const string key)
{
	int value=0;
	string str=get_string(key);
	value=atoi(str.c_str());
	return value;
}

bool ConfigFile::set_string(const string key,const string value)
{
	m_map[key]=value;
	set_vector_value(key,value);
	return true;
}
bool ConfigFile::set_int(const string key,const int value)
{
	char szValue[32];
	bzero(szValue,sizeof(szValue));
	sprintf(szValue,"%d",value);
	m_map[key]=szValue;
	set_vector_value(key,szValue);
	return true;
}

void ConfigFile::trim_left_right(string &str)
{
	if(!str.empty())
	{
		int len=str.size();
		int i=0;
		for(i=0;i<len;i++)
		{
			if(str[i]!=' ')
				break;
		}
		str.erase(0,i);

		for(i=len-1;i>0;i--)
		{
			if(str[i]!=' ')
				break;
		}
		str.erase(i+1,len);
	}
}
void ConfigFile::set_vector_value(const string key,const string value)
{
	bool findkey=false;
	if(!key.empty())
	{
		vector<string>::iterator iter;
		for(iter=m_data.begin();iter!=m_data.end();++iter)
		{
			string str=(*iter);
			if(str[0]=='#')
				continue;
			else
			{
				int pos=str.find('=');
				int len=str.size();
				if(pos>=0)
				{
					string left=str.substr(0,pos);
					if(left.find(key))
					{
						(*iter).replace(pos+1,len-pos-1,value);
						findkey=true;
					}
				}
			}
		}
		if(!findkey)
		{
			string str=key+'='+value;
		}
	}
}


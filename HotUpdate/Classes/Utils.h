#pragma once
#include <string>
#include <vector>
#include "json\document.h"

// ��Documentת���ַ���
std::string serializeRapidjsonDocument(const rapidjson::Document &doc);
// ��intתΪ�ַ��� (��C++11��ʵӦ��ʹ��std::to_string����)
std::string intToStr(int i);
// д�������ļ�
bool writeFile(const char *path, const char *data, int length);
// ����javascript��array.join(str)��python��str.join(array)
std::string joinStrings(const std::vector<std::string> &strs, const std::string &delim);
// �ָ��ַ���������ᱻ������res��
void splitString(const std::string &str, const std::string &delim, std::vector<std::string>& res);
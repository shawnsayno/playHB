#pragma once

#include <iostream>
#include <string>
#include <vector>

//删除字符串左右两侧的空格
void InplaceTrim(std::string& strValue);

//字符串折成整数数组
void split(std::vector<std::string>& vct, const char* src, char sp, bool bReserveEmptyString = true);
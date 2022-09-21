#include "comm.h"
using namespace std;

void InplaceTrim(std::string& strValue) {
  strValue.erase(strValue.find_last_not_of("\r\n") + 1);
  strValue.erase(0, strValue.find_first_not_of(" "));
  strValue.erase(strValue.find_last_not_of(" ") + 1);
}

void split(vector<string>& vct, const char* src, char sp, bool bReserveEmptyString) {
  string strMain = src;
  vct.clear();

  if (strMain.empty()) return;

  size_t nPrevPos = 0;
  size_t nPos;
  std::string strTemp;
  while ((nPos = strMain.find(sp, nPrevPos)) != string::npos) {
    strTemp.assign(strMain, nPrevPos, nPos - nPrevPos);
    InplaceTrim(strTemp);
    if (bReserveEmptyString || !strTemp.empty()) vct.push_back(strTemp);
    nPrevPos = nPos + 1;
  }

  strTemp.assign(strMain, nPrevPos, strMain.length() - nPrevPos);
  InplaceTrim(strTemp);
  if (bReserveEmptyString || !strTemp.empty()) vct.push_back(strTemp);
}

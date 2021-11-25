// playHB.cpp : 定义控制台应用程序的入口点。
//

#include "Cchess.h"

static const auto _____ = []() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  return nullptr;
}();

int main(int argc, const char *argv[]) {
  Cchess mychess;
  // FILE *fp = stdin;  //默认使用标准输入
  FILE *fp = fopen("../start_map.txt", "r");
  // FILE *fp = fopen("D:\\workspace\\result\\cppwork\\playHB\\start_map.txt", "r");
  // //调试输入内容用
  int nRet = mychess.init(fp);

  if (nRet) {
    std::cout << nRet;
  } else {
    //抢与发通过命令行参数控制
    string cmd = argv[1];

    if (cmd.compare("F") == 0) {
      std::cout << mychess.faHongBao().c_str() << "\n";
    } else if (cmd.compare("Q") == 0) {
      std::cout << mychess.qiangHb().c_str() << "\n";
      //放置障碍策略需要玩家自己补充
    } else {
      std::cout << "请输入玩法类型：F-发红包 Q-抢红包" << endl;
      return -1;
    }
  }

  if (fp != stdin) {
    fclose(fp);  //如果使用文件作为输入，需要关闭文件句柄
  }
  return 0;
}

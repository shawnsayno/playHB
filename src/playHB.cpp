// playHB.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "Cchess.h"

static const auto _____ = []() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  return nullptr;
}();

int main(int argc, const char *argv[]) {
  Cchess mychess;
  // FILE *fp = stdin;  //Ĭ��ʹ�ñ�׼����
  FILE *fp = fopen("../start_map.txt", "r");
  // FILE *fp = fopen("D:\\workspace\\result\\cppwork\\playHB\\start_map.txt", "r");
  // //��������������
  int nRet = mychess.init(fp);

  if (nRet) {
    std::cout << nRet;
  } else {
    //���뷢ͨ�������в�������
    string cmd = argv[1];

    if (cmd.compare("F") == 0) {
      std::cout << mychess.faHongBao().c_str() << "\n";
    } else if (cmd.compare("Q") == 0) {
      std::cout << mychess.qiangHb().c_str() << "\n";
      //�����ϰ�������Ҫ����Լ�����
    } else {
      std::cout << "�������淨���ͣ�F-����� Q-�����" << endl;
      return -1;
    }
  }

  if (fp != stdin) {
    fclose(fp);  //���ʹ���ļ���Ϊ���룬��Ҫ�ر��ļ����
  }
  return 0;
}

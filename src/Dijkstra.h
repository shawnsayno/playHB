#pragma once
#include <iostream>
#include <string>

#define DIS_MAX 999999999

using namespace std;

struct Dis {
  string path;
  int value;
  bool visit;
  Dis() {
    visit = false;
    value = 0;
    path = "";
  }
};

class Graph_DG {
 private:
  int vexnum;  //ͼ�Ķ������
  int edge;    //ͼ�ı���
  int **arc;   //�ڽӾ���
  Dis *dis;    //·��

 public:
  //���캯��
  Graph_DG(int vexnum, int edge);

  //��������
  ~Graph_DG();

  // �ж�����ÿ������ĵıߵ���Ϣ�Ƿ�Ϸ�
  //�����1��ʼ���
  bool check_edge_value(int start, int end, int weight);

  //���ӱ�Ȩ��
  void add_edge(int start, int end, int weight, bool bothway = true);

  //����ͼ
  void createGraph();

  //��ӡ�ڽӾ���
  void print();

  //�����·��
  void Dijkstra(int begin);

  //��ӡ���·��
  void print_path();

  //��ȡ����
  int getValue(int pos);

  //��ȡ·��
  string getPath(int pos);
};

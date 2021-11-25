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
  int vexnum;  //图的顶点个数
  int edge;    //图的边数
  int **arc;   //邻接矩阵
  Dis *dis;    //路径

 public:
  //构造函数
  Graph_DG(int vexnum, int edge);

  //析构函数
  ~Graph_DG();

  // 判断我们每次输入的的边的信息是否合法
  //顶点从1开始编号
  bool check_edge_value(int start, int end, int weight);

  //增加边权重
  void add_edge(int start, int end, int weight, bool bothway = true);

  //创建图
  void createGraph();

  //打印邻接矩阵
  void print();

  //求最短路径
  void Dijkstra(int begin);

  //打印最短路径
  void print_path();

  //获取长度
  int getValue(int pos);

  //获取路径
  string getPath(int pos);
};

#pragma once
#include <memory.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Dijkstra.h"

using namespace std;
using std::map;
using std::string;
using std::vector;

//地图尺寸
#define SIZE 40
#define MONEY 200
#define PACKETS 10
#define USER_NUM 4

const int g_edge = (SIZE - 1) * (SIZE - 1) + (SIZE - 2) * 3 * 4 + 2 * 4;

//格子类型
enum nodeType {
  nodeType_null = 0,    //空
  nodeType_player = 1,  //用户
  nodeType_hb = 2,      //红包
  nodeType_block = 3,   //路障
};

struct Tnode {
  nodeType type;    // 类型，1，用户；2，红包；3，路障；
  int x;            // 本地坐标x
  int y;            // 本地坐标y
  int value;        // 金额，或者路障次数，根据类型区分
  int owner;        // 所属
  double neighbor;  // 附近的障碍物占附近点的百分比，最大1
  int id;           // 顺序编号

  Tnode() { memset(this, 0, sizeof(Tnode)); }

  void operator=(const Tnode& node) {
    this->type = node.type;
    this->x = node.x;
    this->y = node.y;
    this->value = node.value;
    this->owner = node.owner;
    this->neighbor = node.neighbor;
    this->id = node.id;
  }
};

class Cchess {
 public:
  Cchess();

  ~Cchess();

  //初始化
  int init(FILE* fp);

  //发红包流程
  string faHongBao();

  //抢红包流程
  string qiangHb();

 protected:
  //获取指定类型或者空的点
  void pushSpecific(int x, int y, vector<Tnode>& vCoop, nodeType specific);

  //获取附近类型的指定类型点type: 0、8邻居 1、4邻居， 2、4对角
  void getCoopTag(const Tnode& p, vector<Tnode>& vCoop, int type = 1,
                  nodeType specific = nodeType_null);

  //直接距离
  int distance(const Tnode& from, const Tnode& to);

  //图距离
  int distanceDG(int player, const Tnode& to);

  //打印棋盘
  void printChess(Tnode chess[SIZE][SIZE], int type);

  //获取通向路径点
  Tnode& getPathNode(int player, int id);

  //计算方向
  string calcDirt(const Tnode& from, const Tnode& to);

  //找到目标点, needNear比其他对手近
  bool findTarget(Tnode& p, bool needNear = true);

  //找到障碍点
  bool findBlockNode(Tnode& p, Tnode& tar);

  //是否是只有一个出口，如果是返回出口
  bool onewayOut(Tnode& from, Tnode& to);

  //是否可以堵住，如果是返回堵住位置
  bool onewayBlock(Tnode& from, Tnode& block);

 protected:
  //读文件
  int readFp(FILE* fp);

  //获取领接矩阵
  void mapNeighborBlock();

  //生成图加权边
  void genPath();

  //初始空置点
  void initGeoSpace();

  //算距离设红包点位
  void setHongbao(vector<pair<Tnode*, int> >& vecHB, int level);

  //简易设红包点位
  void setHBEasy(vector<pair<Tnode*, int> >& vecHB);

  //构造红包金额
  void initMoney(vector<pair<Tnode*, int> >& vecHB);

  //设置走路位置
  string setPace();

  //随机走
  string blindGo();

  //最短路径走
  string dijkstraGo();

  //设置障碍
  string setBlock();

  //补刀堵
  string budao();

  //堵路
  string dulu(bool pursue = false);

  //疯狂堵路
  string crazyDu();

  //初始化最短路径
  void initGraph(int user);

 private:
  Tnode m_chess[SIZE][SIZE];  //地图
  int m_curTurn;              //当前轮数
  int m_self;                 //自己的角色
  int m_balance[USER_NUM];    //玩家当前余额
  Tnode* m_pos[USER_NUM];     //玩家当前位置

  vector<Tnode*> m_redpackets;  //地图上全部红包
  vector<Tnode*> m_geo;         //地形空置区域
  vector<Tnode*> m_setBlock;    //玩家设置的障碍
  vector<int> m_sortbalance;    //排序的余额
  Graph_DG* m_dg[USER_NUM];     //加权路径图
};

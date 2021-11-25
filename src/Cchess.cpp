#include "Cchess.h"

#include <string.h>

#include <algorithm>
#include <sstream>

bool isspace(char a) { return a == ' '; }

void split(const char* str, const char* delimiter, int max_tokens, std::vector<std::string>& data) {
  data.clear();

  // 去掉前面的空格
  while (isspace(*str)) {
    ++str;
  }

  const char* s = strstr(str, delimiter);

  if (s) {
    int del_len = strlen(delimiter);

    do {
      int len = s - str;
      int rel_len = len;

      // 去掉后面的空格
      for (const char* p = s - 1; p > str && isspace(*p); --p) {
        --rel_len;
      }

      if (rel_len) data.push_back(std::string(str, rel_len));
      str = s + del_len;

      // 去掉前面的空格
      while (isspace(*str)) {
        ++str;
      }

      s = strstr(str, delimiter);
    } while (--max_tokens && s && *s);
  }

  if (*str) {
    // 去掉后面的空格
    int rel_len = strlen(str);

    for (const char* p = str + rel_len - 1; p > str && isspace(*p); --p) {
      --rel_len;
    }

    if (rel_len) data.push_back(std::string(str, rel_len));
  }

  return;
}

bool compTnodeNei(Tnode* l, Tnode* r) { return l->neighbor > r->neighbor; }

Cchess::Cchess() : init_flag(0), m_curTurn(0), m_self(-1) {
  memset(m_chess, 0, sizeof(m_chess));
  memset(m_balance, 0, sizeof(m_balance));

  for (auto& it : m_dg) {
    it = new Graph_DG(SIZE * SIZE, g_edge);
  }
};

Cchess::~Cchess() {
  for (auto& it : m_dg) {
    if (it) delete it;
    it = NULL;
  }
}

int Cchess::readFp(FILE* fp) {
  if (fp == NULL) {
    return -2;
  }

  char line[7 * SIZE] = {0};
  char* ret = fgets(line, sizeof(line) - 1, fp);
  int lineNo = 0;

  while (ret != NULL && strlen(line) > 0) {
    //前几行没有空格
    if (lineNo == 0) {
      string tmp = line;
      tmp.erase(0, tmp.find_first_not_of(" "));
      tmp.erase(tmp.find_last_not_of(" ") + 1);
      m_self = atoi(tmp.substr(1).c_str()) - 1;

      if (m_self < 0 || m_self > USER_NUM) {
        return -3;
      }
    } else if (lineNo == 1) {
      string tmp = line;
      tmp.erase(0, tmp.find_first_not_of(" "));
      tmp.erase(tmp.find_last_not_of(" ") + 1);
      m_curTurn = atoi(tmp.substr(1).c_str());
    } else if (lineNo == 2) {
      string tmp = line;
      vector<string> result;
      split(tmp.c_str(), ";", SIZE, result);

      if (result.size() < USER_NUM) {
        printf("size=%lu\n", result.size());
        return -5;
      }

      for (int i = 0; i < USER_NUM; i++) {
        if (strstr(result[i].c_str(), ":") == NULL) {
          return -6;
        }

        m_balance[i] = atoi(result[i].substr(3).c_str());
        m_sortbalance.push_back(m_balance[i]);
      }
      ::sort(m_sortbalance.begin(), m_sortbalance.end(), [](int l, int r) { return l > r; });
    } else {
      string tmp = line;
      vector<string> result;
      split(tmp.c_str(), " ", SIZE, result);
      int chessNo = lineNo - 3;

      for (int i = 0; i < SIZE; i++) {
        m_chess[i][chessNo].x = i;
        m_chess[i][chessNo].y = chessNo;
        m_chess[i][chessNo].id = chessNo * SIZE + i + 1;

        switch (result[i][0]) {
          case '#': {
            break;
          }
          case 'p':
          case 'P': {
            int m = atoi(result[i].substr(1).c_str()) - 1;

            if (m < 0 || m > USER_NUM) {
              return -1;
            }

            Tnode& node = m_chess[i][chessNo];
            node.type = nodeType_player;
            node.owner = m;

            m_pos[m] = &node;
            break;
          }
          case 'r':
          case 'R': {
            int m = atoi(result[i].substr(1).c_str()) - 1;

            if (m < -1 || m > USER_NUM) {
              return -8;
            }

            Tnode& node = m_chess[i][chessNo];
            node.type = nodeType_hb;
            node.value = atoi(result[i].substr(3).c_str());
            node.owner = m;

            m_redpackets.push_back(&node);
            break;
          }

          case 'b':
          case 'B': {
            Tnode& node = m_chess[i][chessNo];
            node.type = nodeType_block;
            node.value = atoi(result[i].substr(1).c_str());

            if (node.value > 0) {
              m_setBlock.push_back(&node);
            }

            break;
          }
        }
      }
    }

    {
      if (lineNo == (3 + SIZE - 1)) {
        break;
      }

      memset(line, 0, sizeof(line));
      ret = fgets(line, sizeof(line) - 1, fp);
      lineNo++;
    }
  }

  return 0;
}

int Cchess::init(FILE* fp) {
  int ret = readFp(fp);
  genPath();

  init_flag = 1;
  return ret;
}

string Cchess::qiangHb() {
  //移动路线
  string cmd = setPace();

  //设置障碍
  string block = setBlock();
  if (!block.empty()) {
    cmd = cmd + ";" + block;
  }

  return cmd;
}

string Cchess::faHongBao() {
  //初始化空置点
  initGeoSpace();

  //发红包位置变量
  vector<pair<Tnode*, int> > vecHB;

  //发红包选取点间隔
  setHongbao(vecHB, 10);

  //降低标准
  if (vecHB.size() < PACKETS) {
    setHongbao(vecHB, 5);
  }
  //大概率是被围住了，选取备用点
  if (vecHB.size() < PACKETS) {
    setHBEasy(vecHB);
  }

  //给每个位置设置金额
  initMoney(vecHB);
  ostringstream o;
  for (auto it : vecHB) {
    o << "R" << it.second << "_" << it.first->x << "_" << it.first->y << ";";
  }

  return o.str();
}

void Cchess::pushSpecific(int x, int y, vector<Tnode>& vCoop, nodeType specific) {
  if (x < 0 || x > SIZE - 1 || y < 0 || y > SIZE - 1) {
    return;
  }

  if (nodeType_null == specific) {
    vCoop.push_back(m_chess[x][y]);
    return;
  }

  if (specific == m_chess[x][y].type) {
    vCoop.push_back(m_chess[x][y]);
  }
}

void Cchess::getCoopTag(const Tnode& p, vector<Tnode>& vCoop, int type, nodeType specific) {
  if (type == 0 || type == 1) {
    pushSpecific(p.x - 1, p.y, vCoop, specific);
    pushSpecific(p.x + 1, p.y, vCoop, specific);
    pushSpecific(p.x, p.y + 1, vCoop, specific);
    pushSpecific(p.x, p.y - 1, vCoop, specific);
  }
  if (type == 0 || type == 2) {
    pushSpecific(p.x + 1, p.y + 1, vCoop, specific);
    pushSpecific(p.x - 1, p.y + 1, vCoop, specific);
    pushSpecific(p.x + 1, p.y - 1, vCoop, specific);
    pushSpecific(p.x - 1, p.y - 1, vCoop, specific);
  }
}

void Cchess::mapNeighborBlock() {
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      if (m_chess[i][j].type == nodeType_block) {
        m_chess[i][j].neighbor = 1;
        continue;
      }

      //获取附近的点
      vector<Tnode> vCoop;
      getCoopTag(m_chess[i][j], vCoop);

      for (auto it : vCoop) {
        if (it.type == nodeType_block) {
          if (it.value > 0) {
            m_chess[i][j].neighbor += (double)it.value / 5;  //附近障碍权重会加上周围路障值/5
          } else {
            m_chess[i][j].neighbor += 1;  //地图障碍默认设置为1
          }
        }
      }

      //当前点障碍权重等于其周围点的平局值
      m_chess[i][j].neighbor = ((double)m_chess[i][j].neighbor + 4 - vCoop.size()) / 4;
    }
  }
}

void Cchess::printChess(Tnode chess[SIZE][SIZE], int type) {
  for (int i = 0; i < SIZE; ++i) {
    ostringstream o;
    for (int j = 0; j < SIZE; ++j) {
      switch (type) {
        case 1:
          o << chess[i][j].type << " ";
          break;
        case 2:
          o << chess[i][j].value << " ";
          break;
        case 3:
          o << chess[i][j].neighbor << " ";
          break;
      }
    }
    cout << o.str().substr(0, o.str().size() - 1) << endl;
  }
}

int Cchess::distance(const Tnode& from, const Tnode& to) {
  return ::abs(from.x - to.x) + ::abs(to.y - from.y);
}

int Cchess::distanceDG(int player, const Tnode& to) { return m_dg[player]->getValue(to.id); }

void Cchess::initMoney(vector<pair<Tnode*, int> >& vecHB) {
  double moneyBag[PACKETS] = {0};
  double total = 0;
  for (int i = 0; i < PACKETS; ++i) {
    //与其他对手的距离和/地图边长/2+0.01(避免除0)
    moneyBag[i] = (double)vecHB[i].second / SIZE / 2 + 0.01;
    total += moneyBag[i];
  }

  //根据权重占比，分配每个红包金额
  int totalMoney = 0;
  for (int i = PACKETS - 1; i >= 0; --i) {
    int num = int((double)MONEY * moneyBag[i] / total);
    if (num == 0) {
      num = 1;
    }
    if (i == 0) {
      num = MONEY - totalMoney;
    }
    vecHB[i].second = num;

    totalMoney += num;
  }
}

void Cchess::initGeoSpace() {
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      if (m_chess[i][j].type == nodeType_null) {
        m_geo.push_back(&m_chess[i][j]);
      }
    }
  }

  //相邻障碍多的在前
  ::sort(m_geo.begin(), m_geo.end(), compTnodeNei);
}

void Cchess::setHongbao(vector<pair<Tnode*, int> >& vecHB, int level) {
  for (auto i : m_geo) {
    bool find = true;
    int min_dg_dis = DIS_MAX;  //与其他对手的最小图距离
    int min_dis = DIS_MAX;     //与其他对手的最小坐标距离

    //自己红包不能抢,不能再堵自己
    if (distance(*m_pos[m_self], *i) < level) {
      continue;
    }

    //选障碍多，离对手远的点
    for (int k = 0; k < USER_NUM; ++k) {
      if (m_self == k) continue;
      int dis_dg = distanceDG(k, *i);
      if (dis_dg <= level) {
        find = false;
        break;
      }
      if (dis_dg < min_dg_dis) {
        min_dg_dis = dis_dg;
        //为和其他方式计算结果一致，采用坐标距离
        min_dis = distance(*m_pos[k], *i);
      }
    }

    //选离其他红包远的
    if (find == false) continue;
    for (auto it : vecHB) {
      if (distance(*it.first, *i) < level) {
        find = false;
        break;
      }
    }

    if (find == false) continue;
    for (auto iter : m_redpackets) {
      if (distance(*iter, *i) < level) {
        find = false;
        break;
      }
    }

    if (find) {
      vecHB.push_back(::make_pair(i, min_dis));
      //前10个满足要求的点
      if (vecHB.size() >= PACKETS) {
        break;
      }
    }
  }
}

void Cchess::setHBEasy(vector<pair<Tnode*, int> >& vecHB) {
  for (auto i : m_geo) {
    bool find = true;
    int min_dis = DIS_MAX;  //与其他对手的最小坐标距离
    //自己红包不能抢,不能再堵自己
    if (distance(*m_pos[m_self], *i) < 5) {
      continue;
    }

    //选离其他较远的点
    if (find == false) continue;
    for (int k = 0; k < USER_NUM; ++k) {
      if (m_self == k) continue;

      int dis = distance(*m_pos[k], *i);
      if (dis <= distance(*m_pos[m_self], *i)) {
        find = false;
        break;
      }
      if (dis < min_dis) {
        min_dis = dis;
      }
    }

    if (find == false) continue;
    for (auto it : vecHB) {
      //离别的红包较远
      if (distance(*it.first, *i) < 3) {
        find = false;
        break;
      }
    }
    if (find) {
      vecHB.push_back(::make_pair(i, min_dis));
      if (vecHB.size() >= PACKETS) {
        break;
      }
    }
  }

  if (vecHB.size() < PACKETS) {
    //如果一个点都没有 默认20，否则选最后一个点的一半
    int tmp_dis = vecHB.empty() ? 20 : vecHB.back().second / 2;
    for (auto i : m_geo) {
      //由于可能被堵了 不能再堵自己
      if (distance(*m_pos[m_self], *i) < 5) {
        continue;
      }

      bool find = true;
      for (auto it : vecHB) {
        //离别的红包较远
        if (distance(*it.first, *i) < 3) {
          find = false;
          break;
        }
      }

      if (find) {
        vecHB.push_back(::make_pair(i, tmp_dis));
        if (vecHB.size() >= PACKETS) {
          break;
        }
      }
    }
  }
}

string Cchess::setPace() {
  string pace;
  //没有红包就瞎走 否则选点最短路径
  if (m_redpackets.empty()) {
    pace = blindGo();
  } else {
    pace = dijkstraGo();
  }

  return pace;
}

bool Cchess::onewayOut(Tnode& from, Tnode& to) {
  vector<Tnode> vCoop;
  getCoopTag(from, vCoop);

  int i = 0;
  for (auto itr : vCoop) {
    if (itr.type == nodeType_block || (itr.type == nodeType_hb && itr.owner == from.owner)) {
      ++i;
    } else {
      to = itr;
    }
  }

  if ((vCoop.size() - i) == 1)  //该点只有1个出口
  {
    return true;
  }

  return false;
}

bool Cchess::onewayBlock(Tnode& from, Tnode& block) {
  vector<Tnode> vCoop;
  getCoopTag(from, vCoop);

  int i = 0;
  for (auto itr : vCoop) {
    if ((itr.type == nodeType_block && itr.value == 0) || (itr.type == nodeType_player) ||
        (itr.type == nodeType_hb && itr.owner == from.owner)) {
      ++i;
    } else {
      block = itr;
    }
  }

  if (((vCoop.size() - i) == 1) && block.type != nodeType_hb &&
      block.type != nodeType_block)  //该点只有1个出口
  {
    return true;
  }

  return false;
}

string Cchess::budao() {
  if (m_balance[m_self] < 50) {
    return "";
  }

  for (auto it : m_pos) {
    if (it->owner == m_self) continue;

    Tnode p;
    if (onewayBlock(*it, p) && ::abs(m_balance[m_self] - m_balance[it->owner]) < 100) {
      ostringstream o;
      o << "B" << p.x << "_" << p.y;
      return o.str();
    }
  }

  return "";
}

bool Cchess::findBlockNode(Tnode& p, Tnode& tar) {
  int min = DIS_MAX;
  vector<Tnode> vCoop;   //第一外圈
  vector<Tnode> vCoopL;  //第二外圈

  getCoopTag(p, vCoop);
  const int size = vCoop.size();
  for (int k = 0; k < size; ++k) {
    getCoopTag(vCoop[k], vCoopL);
  }

  //找到目标附近权重最大红包的路径
  Tnode tmp;
  for (auto it : vCoopL) {
    if (it.type != nodeType_hb || (it.type == nodeType_hb && it.owner == p.owner)) continue;

    int dis = distanceDG(p.owner, it);
    if (dis < min) {
      tmp = getPathNode(p.owner, it.id);
      if (tmp.type == nodeType_hb) continue;
      tar = tmp;
      min = dis;
    }
  }

  if (min != DIS_MAX) {
    return true;
  }

  return false;
}

string Cchess::dulu(bool pursue) {
  //没人堵 不堵
  if (m_setBlock.size() < 4 || m_balance[m_self] < 50) {
    return "";
  }

  if (m_balance[m_self] == m_sortbalance.front() && ((m_sortbalance[0] - m_sortbalance[1]) > 50)) {
    //有钱堵第二
    for (int i = 0; i < USER_NUM; ++i) {
      if (m_self == i) continue;
      if (m_balance[i] == m_sortbalance[1]) {
        Tnode tar;
        if (findBlockNode(*m_pos[i], tar)) {
          ostringstream o;
          o << "B" << tar.x << "_" << tar.y;
          return o.str();
        }
        break;
      }
    }
  } else if (pursue) {
    //没钱 堵第一
    for (int i = 0; i < USER_NUM; ++i) {
      if (m_self == i) continue;
      if (m_balance[i] == m_sortbalance.front()) {
        Tnode tar;
        if (findBlockNode(*m_pos[i], tar)) {
          ostringstream o;
          o << "B" << tar.x << "_" << tar.y;
          return o.str();
        }
        break;
      }
    }
  }

  return "";
}

string Cchess::crazyDu() {
  if (m_balance[m_self] < 50) {
    return "";
  }

  for (int i = 0; i < USER_NUM; ++i) {
    if (m_self == i) continue;
    Tnode tar;
    if (findBlockNode(*m_pos[i], tar)) {
      ostringstream o;
      o << "B" << tar.x << "_" << tar.y;
      return o.str();
    }
  }
  return "";
}

string Cchess::setBlock() {
  string blockCmd = budao();

  if (blockCmd.empty()) {
    // blockCmd = dulu();
  }

  return blockCmd;
}

string Cchess::calcDirt(const Tnode& from, const Tnode& to) {
  if (from.x == to.x && from.y + 1 == to.y) {
    return "D";
  }
  if (from.x == to.x && from.y == to.y + 1) {
    return "U";
  }
  if (from.x == to.x + 1 && from.y == to.y) {
    return "L";
  }
  if (from.x + 1 == to.x && from.y == to.y) {
    return "R";
  }
  return "S";
}

string Cchess::blindGo() {
  vector<Tnode> vCoop;
  getCoopTag(*m_pos[m_self], vCoop);

  Tnode p;
  p.x = -1;
  for (auto it : vCoop) {
    if (it.type == nodeType_hb && it.owner == m_self) continue;

    if (it.type == nodeType_player || it.type == nodeType_block) continue;

    Tnode tmp;
    if (onewayOut(it, tmp)) continue;

    p = it;
  }

  if (p.x != -1) {
    return calcDirt(*m_pos[m_self], p);
  }

  return "S";
}

Tnode& Cchess::getPathNode(int player, int id) {
  string path = m_dg[player]->getPath(id);

  vector<string> data;
  split(path.c_str(), "v", 3, data);
  int d = atoi(data[1].c_str()) - 1;

  int x = d % SIZE;
  int y = d / SIZE;
  return m_chess[x][y];
}

string Cchess::dijkstraGo() {
  string ret;
  Tnode node;  //红包
  //查找最优点，找到走，没找到瞎走, 比别人近效果不明显
  if (findTarget(node, true)) {
    //获取移动点，计算路径
    Tnode& tar = getPathNode(m_self, node.id);
    ret = calcDirt(*m_pos[m_self], tar);

    //走路的位置会影响设障碍的位置，需要修改
    m_pos[m_self]->type = nodeType_null;
    tar.type = nodeType_player;
    tar.owner = m_self;
    m_pos[m_self] = &tar;
  } else {
    ret = blindGo();
  }

  return ret;
}

void Cchess::genPath() {
  for (int u = 0; u < USER_NUM; ++u) {
    for (int i = 0; i < SIZE; ++i) {
      for (int j = 0; j < SIZE; ++j) {
        vector<Tnode> vCoop;
        getCoopTag(m_chess[i][j], vCoop);
        for (auto it : vCoop) {
          if (nodeType_block == it.type) continue;

          if (it.type == nodeType_hb && it.owner == u) continue;

          if (it.type == nodeType_player && it.owner != u) continue;

          //权重 = 基础2 + 目的周边障碍比例*4
          int weight = 2;

          m_dg[u]->add_edge(m_chess[i][j].id, it.id, weight, false);
        }
      }
    }
  }

  for (int u = 0; u < USER_NUM; ++u) {
    m_dg[u]->Dijkstra(m_pos[u]->id);
  }
}

bool Cchess::findTarget(Tnode& p, bool needNear) {
  int min = DIS_MAX - MONEY;
  bool find = false;
  for (auto it : m_redpackets) {
    if (it->owner == m_self) continue;
    Tnode tmp;
    if (m_setBlock.size() >= 4 && onewayOut(*it, tmp)) continue;

    int dis = distanceDG(m_self, *it);
    int value = dis - it->value;
    // cout << "target:" << it->id << ",dis:" << dis << ",value:" << it->value << ",bias:" << value
    //<< endl;
    if (value < min) {
      bool near = true;
      if (needNear) {
        for (int i = 0; i < USER_NUM; ++i) {
          if (i == m_self) continue;
          if (i == it->owner) continue;
          int idis = distanceDG(i, *it);
          if (idis < dis) {
            near = false;
            break;
          }
        }
      }

      if (near) {
        min = value;
        p = *it;
        find = true;
      }
    }
  }

  return find;
}

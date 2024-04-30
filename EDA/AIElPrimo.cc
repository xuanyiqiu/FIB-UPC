#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME ElPrimo


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  typedef priority_queue<pair<int,int>> cmd;
  typedef set<Pos> VisitedPos;
  const vector<Dir> dirs = {Up,Down,Left,Right};

  /**
   * Play method, invoked once per each round.
   */
  
  Pos bfs_gun(Pos ini, WeaponType w) {

      VisitedPos visitats;
      queue<Pos> q;
      q.push(ini);
      visitats.insert(ini);

      vector<int> perm = random_permutation(4);

      while(not q.empty()) {

        Pos u = q.front(); 
        q.pop();

        for (int i : perm) {
            
            Dir d = dirs[i];
            Pos v = u + d;

            if (pos_ok(v) and in_range(v, ini, 20)) {
              Cell c = cell(v);
              if (c.id == -1 and c.b_owner == -1 and c.type == Street) {

                auto it = visitats.find(v);

                if (it == visitats.end()) {

                  visitats.insert(v);
                  q.push(v);

                  if(w == Gun and cell(v).weapon == Bazooka) return u;
                  else if (w == Hammer and cell(v).weapon != NoWeapon) return u;
                }
              }
            }
        }
      }
    return {-1,-1};
  }

  Pos bfs_enemy(Pos ini, int myHp, WeaponType myWeapon) {

      VisitedPos visitats;
      queue<Pos> q;
      q.push(ini);
      visitats.insert(ini);
      vector<int> perm = random_permutation(4);

      while(not q.empty()) {

        Pos u = q.front(); 
        q.pop();

        for (int i : perm) {
            
            Dir d = dirs[i];
            Pos v = u + d;

            if (pos_ok(v) and in_range(v, ini, 15)) {
              Cell c = cell(v);
              if (c.type == Street and c.resistance == -1) {

                if(c.id != -1 and citizen(c.id).player != me()) {
                  WeaponType enW = citizen(c.id).weapon;
                  int enHp = citizen(c.id).life;

                  if((enW == myWeapon and myWeapon == Hammer and myHp >= enHp) or (myWeapon == Gun or myWeapon == Bazooka))  {
                    //cerr << "He encontrado un enemigo en: " << v;
                    return v;
                  }
                }

                auto it = visitats.find(v);

                if (it == visitats.end()) {

                  visitats.insert(v);
                  q.push(v);
                  
                }
              }
            }
        }
      }
    return {-1,-1};
  }

  Pos bfs_bonus(Pos ini, BonusType B) {

      VisitedPos visitats;
      queue<Pos> q;
      q.push(ini);
      visitats.insert(ini);

      vector<int> perm = random_permutation(4);

      while(not q.empty()) {

        Pos u = q.front(); 
        q.pop();

        for (int i : perm) {
            
            Dir d = dirs[i];
            Pos v = u + d;

            if (pos_ok(v) and in_range(v, ini, 15)) {
              Cell c = cell(v);
              if (c.id == -1 and c.b_owner == -1 and c.type == Street) {

                auto it = visitats.find(v);

                if (it == visitats.end()) {

                  visitats.insert(v);
                  q.push(v);

                  if(cell(u).bonus == B) return u;
                }
              }
            }
        }
      }
    return {-1,-1};
  }
  
  
  bool in_range(Pos p, Pos ini, int radius) {

    return p.i < ini.i + radius and p.i > ini.i - radius and p.j < ini.j + radius and p.j > ini.j - radius;  
  }
  bool movable (const Pos& p, const Dir& d) {
    Pos newpos = p + d;
    Cell c = cell(newpos);
    return pos_ok(newpos) and c.type == Street and (c.b_owner == -1 or c.b_owner == me());
  }

  Dir run(const Pos& p, const Pos& danger) {

  }

  Dir calculate_dir(const Pos& p, const Pos& money) {
    int i = random(0,7);
    if(money.i == p.i and money.j < p.j) { //LEFT
      if(movable(p, Left)) return Left;
      else if (i % 2 == 0 and movable(p, Up)) return Up;
      else if (movable(p, Down))return Down;
    }
    if(money.i == p.i and money.j > p.j) {  //RIGHT
      if(movable(p, Left)) return Right;
      else if (i % 2 == 0 and movable(p, Up)) return Up;
      else if (movable(p, Down))return Down;
    }
    if(money.i < p.i and money.j == p.j) { //UP
      if(movable(p, Left)) return Up;
      else if (i % 2 == 0 and movable(p, Left)) return Left;
      else if (movable(p, Right)) return Right;
    }
    if(money.i > p.i and money.j == p.j) { //DOWN
      if(movable(p, Left)) return Down;
      else if (i % 2 == 0 and movable(p,Left)) return Left;
      else if(movable(p,Right)) return Right;
    }
    
    //Diagonal move
    if(money.i < p.i and money.j < p.j) { //UP, LEFT
      if(i % 2 == 0 and movable(p, Up)) return Up;
      else if(movable(p, Left)) return Left;
    }
    if(money.i < p.i and money.j > p.j) { //UP, RIGHT
      if(i % 2 == 0 and movable(p, Up)) return Up;
      else if(movable(p, Right)) return Right;
    }
    if(money.i > p.i and money.j < p.j) { //DOWN, LEFT
      if(i % 2 == 0 and movable(p, Down)) return Down;
      else if(movable(p, Left)) return Left;
    }
    if(money.i > p.i and money.j > p.j) { //DOWN, RIGHT
      if(i % 2 == 0 and movable(p, Down)) return Down;
      else if(movable(p, Right)) return Right;
    }            
    return dirs[random(0,3)];

  }

  void move_builders(cmd& command) {
    vector<int> b = builders(me());
    vector<Pos> bs = barricades(me());
    int num_b = bs.size();
    for (int id : b) {
      Dir d;
      Pos p = citizen(id).pos;
      if (status(me()) >= 95.0) return;
      if(is_day()) {
        if(bs.size() < 3) {
          int i = random(0,3);
          while(not pos_ok(p + dirs[i]) and cell(p+dirs[i]).type != Street and cell(p+dirs[i]).id != -1) i = random(0,3);
          build(id, dirs[i]);
        }
        else if (round()%25 > 12) {
          bool in = false;
          set<pair<double, Pos>> closest;
          for (Pos b : bs) {
            closest.insert({sqrt((p.i + b.i)*(p.i + b.i) + (p.j + b.j)*(p.j + b.j)), b});
            in = b == p;
          }
          if (not in) {
            Pos protection = closest.begin()->second;
            d = calculate_dir(p, protection);
            move(id, d);
          }
        }
        else {
          Pos p = citizen(id).pos;
          Pos money = bfs_bonus(p, Money);
          Dir d = calculate_dir(p, money);
          move(id, d);
        }
      }
      else {
        bool in = false;
        set<pair<double, Pos>> closest;
        for (Pos b : bs) {
          closest.insert({sqrt((p.i + b.i)*(p.i + b.i) + (p.j + b.j)*(p.j + b.j)), b});
          in = b == p;
        }
        if (not in) {
          if (closest.empty()) {
            Pos p = citizen(id).pos;
            Pos money = bfs_bonus(p, Money);
            Dir d = calculate_dir(p, money);
            move(id, d);
          }
          else {
            Pos protection = closest.begin()->second;
            d = calculate_dir(p, protection);
            move(id, d);
          }

        }
      }
    }
  }

  void move_warriors(cmd& command) {
      vector<int> w = warriors(me()); 
      for(int id : w) {
        if (status(me()) >= 95.0) return;
        Dir d;
        Pos p = citizen(id).pos;

        WeaponType myWeapon = citizen(id).weapon;
        int myHealth = citizen(id).life;

        if(is_day()) {
          if(round()%25 < 12) {
            
            if(citizen(id).life < 30) {
              Pos health = bfs_bonus(p, Food);
              d = calculate_dir(p, health);    
            }
            else {
              Pos money = bfs_bonus(p, Money);
              d = calculate_dir(p, money);
              
            }
          }
          else if (citizen(id).weapon == Hammer or citizen(id).weapon == Gun) {
            Pos weapon = bfs_gun(p, citizen(id).weapon);
            d = calculate_dir(p, weapon);
            
          }
          else {
            Pos enemy = bfs_enemy(p, myHealth, myWeapon);
            d = calculate_dir(p, enemy);
        
          } 
        }
        else {
          Pos enemy = bfs_enemy(p, myHealth, myWeapon);
          d = calculate_dir(p, enemy);
          cerr << "Warrior (" << id << ")is moving from (" << p << ") -->(" << enemy <<')' << d << ')' << endl;
          
        }
       
        move(id,d); 

      }
  }
  virtual void play () {
    cmd command;
    move_builders(command);
    move_warriors(command);
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);


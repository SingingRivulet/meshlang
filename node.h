#ifndef MESHLANG_NODE
#define MESHLANG_NODE
#include "hbb.h"
#include <set>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <QDebug>
namespace meshlang{
    struct variable{//变量
        std::string name,type;
        inline variable():name(),type(){}
        inline variable(const std::string & n,const std::string & t){
            name = n;
            type = t;
        }
        inline variable(const variable & n){
            name = n.name;
            type = n.type;
        }
        inline const variable & operator=(const variable & n){
            name = n.name;
            type = n.type;
            return * this;
        }
    };
    
    struct function{//函数
        std::string             name;
        std::vector<variable>   input,output;
        HBB::vec                size;//方框大小
    };
    
    struct node;
    struct line;
    struct line{//连线
        variable var;
        node * start , * end;
        int startId  , endId;
        HBB::vec    startPosi;
        HBB::vec    endPosi;
        HBB::AABB * em;
        void remove(bool erst=true,bool ered=true);
    };
    
    struct node{
        function  * type;
        HBB::vec    position;   //在图中的位置
        std::vector<line*> input;
        std::vector<std::set<line*> > output;
        std::unordered_map<std::string,std::string> initval;
        line      * trueThen;
        line      * falseThen;
        int         id;
        std::set<line*> last;
        HBB::AABB * em;
        void getClickStatus(const HBB::vec & p , int & mode /* 1 input , 2 output , 3 center */ , int & port);
        void getPortPosition(HBB::vec & p , int  mode /* 1 input , 2 output , 3 center */ , int  port);
    };
    
    struct functions{
        std::map<std::string,function * > funcs;
        void addFunc(const std::string & name,const std::vector<variable> & input,const std::vector<variable> & output);
        ~functions();
    };
    
    struct program:functions{
        std::set<node * >       nodes;
        std::map<int,node * >   nodeMap;
        std::set<line * >       lines;
        HBB elements;
        HBB elementlines;
        node * addNode(const std::string & name,const HBB::vec & tposition,int id=0);
        void removeNode(node *);
        virtual void editNode(node *)=0;
        line * link(node * a,int ida,node * b,int idb);
        line * link(int a,int ida,int b,int idb);
        void removeLine(line * ,bool erst=true,bool ered=true);
        program();
        ~program();
        void clickTwoPoint(const HBB::vec & a , const HBB::vec & b);
        void clickToEdit(const HBB::vec & a);
        void clickToRemove(const HBB::vec & a);
        virtual void getInsertingName(std::string & name)=0;
        virtual void showMenu()=0;

        void import(const std::string & path);
        void save(const std::string & path);

        private:
        int lineNum;
        int nodeId;
    };
}
#endif

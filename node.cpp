#include "node.h"
#include <QMessageBox>
namespace meshlang{

void line::remove(bool erst,bool ered){
    if(erst){
        if(startId==-1){
            start->trueThen  = NULL;
        }else
        if(startId==-2){
            start->falseThen = NULL;
        }else{
            try{
                start->output.at(startId).erase(this);
            }catch(...){
                QMessageBox::information(NULL,"Error",QStringLiteral("找不到端口"),QMessageBox::Ok);
            }
        }
    }
    if(ered){
        if(endId<0){
            end->last.erase(this);
        }else{
            try{
                end->input.at(endId)=NULL;
            }catch(...){
                QMessageBox::information(NULL,"Error",QStringLiteral("找不到端口"),QMessageBox::Ok);
            }
        }
    }
    delete this;
}
void functions::addFunc(const std::string & name,const std::vector<variable> & input,const std::vector<variable> & output){
    if(funcs.find(name)!=funcs.end())
        return;
    auto p = new function;
    p->input  = input;
    p->output = output;
    p->name   = name;
    p->size.X = 4;
    p->size.Y = std::max(input.size(),output.size())*2 + 4;
    funcs[name]=p;
}
functions::~functions(){
    for(auto it:funcs){
        delete it.second;
    }
}
node * program::addNode(const std::string & name,const HBB::vec & tposition , int id){
    if(id>0 && nodeMap.find(id)!=nodeMap.end()){
        QMessageBox::information(NULL,"Error",QStringLiteral("id重名"),QMessageBox::Ok);
        return NULL;
    }
    auto it = funcs.find(name);//找到函数
    if(it==funcs.end()){
        QMessageBox::information(NULL,"Error",QStringLiteral("找不到函数"),QMessageBox::Ok);
        return NULL;
    }
    
    auto p = new node;
    p->type = it->second;
    
    p->input.resize(p->type->input.size());//设置输出
    p->output.resize(p->type->output.size());
    
    int len,i;
    len=p->input.size();
    for(i=0;i<len;++i){
        p->input[i]=NULL;
    }
    len=p->output.size();
    for(i=0;i<len;++i){
        p->output[i].clear();
    }
    
    p->trueThen=NULL;
    p->falseThen=NULL;
    p->last.clear();
    
    HBB::vec from,to;
    //p->position = tposition - (p->type->size/2);
    p->position = tposition;
    from = p->position;
    to   = from + p->type->size;
    p->em=elements.add(from,to,p);
    
    if(id>0){
        p->id = id;
        if(id > nodeId){
            nodeId = id;
        }
    }else{
        p->id = ++nodeId;
    }

    nodeMap[p->id]=p;
    nodes.insert(p);

    return p;
}
void program::removeNode(node * n){
    for(auto it:n->input){
        if(it){
            removeLine(it,true,false);
        }
    }
    for(auto itn:n->output){
        for(auto it:itn){
            if(it){
                removeLine(it,false,true);
            }
        }
    }
    if(n->trueThen){
        removeLine(n->trueThen,false,true);
    }
    if(n->falseThen){
        removeLine(n->falseThen,false,true);
    }

    for(auto it:n->last){
        removeLine(it,true,false);
    }
    n->last.clear();
    
    if(n->em){
        n->em->autodrop();
    }
    nodes.erase(n);
    nodeMap.erase(n->id);
    delete n;
}
line * program::link(int a, int ida, int b, int idb){
    auto ita = nodeMap.find(a);
    if(ita == nodeMap.end())
        return NULL;
    auto itb = nodeMap.find(b);
    if(itb == nodeMap.end())
        return NULL;
    return link(ita->second,ida,itb->second,idb);
}

line * program::link(node * a,int ida,node * b,int idb){
    try{

        if((ida>=0 && idb<0) || (ida<0 && idb>=0)){
            QMessageBox::information(NULL,"Error",QStringLiteral("流程端口与普通端口间无法连接"),QMessageBox::Ok);
            return NULL;
        }
        bool isProcess = false;
        if(ida>=0 && idb>=0){

            for(auto it:a->output.at(ida)){
                if(it->end == b && it->endId == idb)
                    return NULL;
            }

            if(b->input.at(idb)!=NULL)
                return NULL;
        
            if(a->type->output.at(ida).type!=b->type->input.at(idb).type)//类型不符
                return NULL;
        }else{
            if(ida==-1 && a->trueThen!=NULL)
                return NULL;
        
            if(ida==-2 && a->falseThen!=NULL)
                return NULL;

            for(auto it:b->last){
                if(it->start==a && it->startId==ida)
                    return NULL;
            }

            isProcess = true;
        }

        auto p = new line;
        if(!isProcess)
            p->var = a->type->output.at(ida);
        else
            p->var.type.clear();
        
        char buf[64];
        snprintf(buf,64,"line%d",lineNum);
        p->var.name = buf;
        ++lineNum;
        
        p->startId = ida>=0 ? ida : (ida==-1 ? -1 : -2);
        p->endId   = idb>=0 ? idb : -2;
        
        a->getPortPosition(p->startPosi,2,p->startId);
        b->getPortPosition(p->endPosi  ,1,p->endId);
        
        if(ida<0 || idb<0){
            if(ida==-1)
                a->trueThen = p;
            else
                a->falseThen = p;
            b->last.insert(p);
        }else{
            try{
                a->output.at(p->startId).insert(p);
                b->input.at(p->endId)         = p;
            }catch(...){
                
            }
        }
        
        p->start = a;
        p->end   = b;
        
        printf("link %d %d %d %d\n" , a->id , ida , b->id , idb);

        HBB::vec from,to;
        from.X = std::min(a->position.X - a->type->size.X , b->position.X - b->type->size.X);
        to.X   = std::max(a->position.X + a->type->size.X , b->position.X + b->type->size.X);
        from.Y = std::min(a->position.Y - a->type->size.Y , b->position.Y - b->type->size.Y);
        to.Y   = std::max(a->position.Y + a->type->size.Y , b->position.Y + b->type->size.Y);
        
        from.Y-=4;
        to.Y  +=4;
        
        p->em=elementlines.add(from,to,p);
        
        lines.insert(p);
        
        return p;
        
    }catch(...){
        QMessageBox::information(NULL,"Error",QStringLiteral("节点无法被连接"),QMessageBox::Ok);
        return NULL;
    }
}
void program::removeLine(line * l,bool erst,bool ered){
    if(l->em){
        l->em->autodrop();
    }
    lines.erase(l);
    l->remove(erst,ered);
}

program::program(){
    lineNum =0;
    nodeId  =0;
}

program::~program(){
    for(auto it:lines)
        delete it;
    for(auto it:nodes)
        delete it;
}
void node::getClickStatus(const HBB::vec & p , int & mode /* 1 input , 2 output , 3 center */ , int & port){
    auto realp = p-position;
    if(realp.X < 0 || realp.Y<0 || realp.X > 4){
        mode=0;
        return;
    }
    port = (int)(realp.Y/2) - 2;
    
    if(realp.X<1){//left
        mode = 1;
    }else
    if(realp.X>3){//right
        mode = 2;
    }else{//center
        mode = 3;
    }
    
}
void node::getPortPosition(HBB::vec & p , int  mode /* 1 input , 2 output , 3 center */ , int  port){
    p.Y = (port+2) * 2 + 1;
    if(mode==1){
        p.X = 0;
    }else
    if(mode==3){
        p.X = 2;
    }else{
        p.X = 4;
    }
    p = p+position;
}
void program::clickToRemove(const HBB::vec & a){
    node * A=NULL;
    elements.fetchByPoint(a,[](HBB::AABB * bx,void * arg){
        *((node**)arg) = (node*)bx->data;
    },&A);
    if(A)
        removeNode(A);
}
void program::clickToRemoveLine(const HBB::vec & a){
    node * A=NULL;
    elements.fetchByPoint(a,[](HBB::AABB * bx,void * arg){
        *((node**)arg) = (node*)bx->data;
    },&A);
    if(A){
        int Amode,Aport;
        A->getClickStatus(a,Amode,Aport);
        try{
            if(Amode==1){
                if(Aport==-2){
                    if(!A->last.empty()){
                        for(auto p:A->last){
                            if(p)
                                removeLine(p);
                        }
                    }
                }else{
                    auto p = A->input.at(Aport);
                    if(p)
                        removeLine(p);
                }
            }else
            if(Amode==2){
                if(Aport==-1){
                    if(A->trueThen){
                        removeLine(A->trueThen);
                    }
                }else
                if(Aport==-2){
                    if(A->falseThen){
                        removeLine(A->falseThen);
                    }
                }else{
                    auto p = A->output.at(Aport);
                    if(!p.empty()){
                        for(auto pt:p){
                            if(pt)
                                removeLine(pt);
                        }
                    }
                }
            }else
            if(Amode==3){
                removeNode(A);
            }
        }catch(...){
            
        }
    }
}
void program::clickTwoPoint(const HBB::vec & a , const HBB::vec & b){
    node * A=NULL;
    int Amode,Aport;
    node * B=NULL;
    int Bmode,Bport;
    
    elements.fetchByPoint(a,[](HBB::AABB * bx,void * arg){
        *((node**)arg) = (node*)bx->data;
    },&A);
    elements.fetchByPoint(b,[](HBB::AABB * bx,void * arg){
        *((node**)arg) = (node*)bx->data;
    },&B);
    if(A==NULL){
        if(B==NULL){//添加
            std::string inm;
            getInsertingName(inm);
            if(!inm.empty()){
                B = addNode(inm,b);
            }
        }
    }else{
        A->getClickStatus(a,Amode,Aport);
        if((Aport==-2 || Aport==-1) && Amode==2 && B==NULL){//添加并连线
            if(Aport==-1){
                if(A->trueThen)
                    return;
            }else
            if(Aport==-2){
                if(A->falseThen)
                    return;
            }
            std::string inm;
            getInsertingName(inm);
            if(!inm.empty()){
                B = addNode(inm,b);
                if(B){
                    link(A,Aport,B,-1);
                }
            }
        }else{
            if(A!=B && B!=NULL){
                B->getClickStatus(b,Bmode,Bport);
                if(Amode==1 && Bmode==2){
                    link(B,Bport,A,Aport);
                }else
                if(Amode==2 && Bmode==1){
                    link(A,Aport,B,Bport);
                }
            }
        }
    }
}
void program::import(const std::string & path){
    FILE * fp = fopen(path.c_str(),"r");
    if(fp){
        char buf[2048];
        while(!feof(fp)){
            bzero(buf,sizeof(buf));
            fgets(buf,sizeof(buf),fp);
            std::istringstream iss(buf);

            std::string active;
            iss>>active;

            if(active=="line"){
                int a,aid,b,bid;
                iss>>a;
                iss>>aid;
                iss>>b;
                iss>>bid;
                link(a,aid,b,bid);
            }else
            if(active=="node"){
                std::string type;
                int id=0;
                float x,y;
                iss>>type;
                iss>>id;
                iss>>x;
                iss>>y;
                addNode(type,HBB::vec(x,y),id);
            }else
            if(active=="function"){
                std::string name;
                iss>>name;
                QString str = buf;
                QStringList inlist = str.split("input:");
                std::vector<variable> input,output;
                input.clear();
                output.clear();
                if(inlist.size()>1){
                    QStringList flist = inlist[1].split("output:");
                    if(flist.size()>0){
                        QStringList inputs = flist[0].split(",");
                        for(auto it:inputs){
                            QString real = it.trimmed();
                            if(!real.isEmpty()){
                                input.push_back(variable(real.toStdString(),real.toStdString()));
                            }
                        }
                    }
                    if(flist.size()>1){
                        QStringList outputs = flist[1].split(",");
                        for(auto it:outputs){
                            QString real = it.trimmed();
                            if(!real.isEmpty()){
                                output.push_back(variable(real.toStdString(),real.toStdString()));
                            }
                        }
                    }
                }
                addFunc(name,input,output);
            }
        }
        fclose(fp);
    }else{
        QMessageBox::information(NULL,"Error",QString("载入文件\"")+path.c_str()+"\"失败",QMessageBox::Ok);
    }
}
void program::save(const std::string & path){
    FILE * fp = fopen(path.c_str(),"r");
    if(fp){
        for(auto it:funcs){
            fprintf(fp,"function %s input:",it.first.c_str());
            bool first=true;
            for(auto it2:it.second->input){
                if(first)
                    fprintf(fp,"%s",it2.type.c_str());
                else
                    fprintf(fp,",%s",it2.type.c_str());
                first = false;
            }
            first = true;
            fprintf(fp," output:");
            for(auto it2:it.second->output){
                if(first)
                    fprintf(fp,"%s",it2.type.c_str());
                else
                    fprintf(fp,",%s",it2.type.c_str());
                first = false;
            }
            fprintf(fp," \n");
        }
        fclose(fp);
        for(auto it:nodes){
            fprintf(fp,"node %s %d %f %f\n" , it->type->name.c_str() , it->id , it->position.X , it->position.Y);
        }
        for(auto it:lines){
            fprintf(fp,"line %d %d %d %d\n" , it->start->id , it->startId , it->end->id , it->endId);
        }
    }
}
}

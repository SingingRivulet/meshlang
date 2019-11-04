#include "window.h"
#include <stdlib.h>
namespace meshlang{

//将原来以(x1,y1)为起点 (x2,y2)为终点的线段
//截取矩形（xmin，ymin, xmax, ymax)内部的线段，
//替换原来的起点、终点。
static bool Liang_Barsky_clipper(double xmin, double ymin, double xmax, double ymax,
                          double& x1, double& y1, double& x2, double& y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double p[5];
    p[1] = -dx;
    p[2] = -p[1];
    p[3] = -dy;
    p[4] = -p[3];
    double q[5];
    q[1] = x1 - xmin;
    q[2] = xmax - x1;
    q[3] = y1 - ymin;
    q[4] = ymax - y1;
    double u1 = 0, u2 = 1;
    for (size_t i = 1; i <= 4; i++){
        double r = q[i]/p[i];
        if (p[i] < 0){
            if (r > u2) return false;
            if (r > u1) u1 = r;     
        }else
        if(p[i] > 0){
            if (r < u1) return false;
            if (r < u2) u2 = r;      
        }else
        if (q[i] < 0)   return false;
    }
    //do x2 y2 first 
    x2 = x1 + u2*dx;  y2 = y1 + u2*dy;
    x1 = x1 + u1*dx;  y1 = y1 + u1*dy;
    return true;
}//liang_barsky_clipper

void window::dragStart(const HBB::vec & p){
    dragStartPosi = p;
    dragRealPosi  = lookAt;
    draging = true;
}
void window::dragSet(const HBB::vec & p){
    lookAt = dragRealPosi - (p - dragStartPosi)/scale;
}
void window::dragRelease(const HBB::vec & p){
    auto delta = p - dragStartPosi;
    draging = false;
    if(fabs(delta.X)+fabs(delta.Y) < 2){
        clickToEdit(lookAt + p/scale);
    }else{
        
    }
}

void window::writeStart(const HBB::vec & p){
    writeStartPosi = p;
    writing = true;
    writeStartRealPosi = lookAt + p/scale;

    node * A=NULL;
    elements.fetchByPoint(writeStartRealPosi,[](HBB::AABB * bx,void * arg){
        *((node**)arg) = (node*)bx->data;
    },&A);
    if(A){
        int Amode,Aport;
        A->getClickStatus(writeStartRealPosi,Amode,Aport);
        if(Amode==1){
            if(Aport==-2 || Aport>=0){
                addingLine = true;
            }
        }else
        if(Amode==2){
            if(Aport==-2 || Aport==-1 || Aport>=0){
                addingLine = true;
            }
        }
    }
}
void window::writeSet(const HBB::vec & p){
    writePosi = p;
    //writeNowPosi = lookAt + p/scale;
}
void window::writeRelease(const HBB::vec & p){
    auto endp = lookAt + p/scale;
    clickTwoPoint(writeStartRealPosi , endp);
    writing = false;
    addingLine = false;
}
window::window():lookAt(0,0),scale(16){
    width = 1024;
    height= 768;
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL can not initialized!SDL Error:%s\n", SDL_GetError());
        return;
    }
    TTF_Init();
    font  = TTF_OpenFont("font.ttf",32);
    gWindow = SDL_CreateWindow(
        "mesh lang - Alpha", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        this->width,
        this->height, 
        SDL_WINDOW_SHOWN
    );
    if (gWindow == nullptr){
        printf("SDL can't create window!SDL Error:%s\n", SDL_GetError());
        return;
    }
    gScreenSurface = SDL_GetWindowSurface(gWindow);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr){
        return;
    }
    draging     = false;
    writing     = false;
    addingLine  = false;
}
window::~window(){
    for(auto it:textures){
        SDL_DestroyTexture(it.second.first);
    }
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    TTF_CloseFont(font);
    SDL_Quit();
}
void window::drawLineAbs(const HBB::vec & f , const HBB::vec & t){
    auto af = (f-lookAt) * scale;
    auto at = (t-lookAt) * scale;
    
    double x1 = af.X;
    double x2 = at.X;
    double y1 = af.Y;
    double y2 = at.Y;
    
    if(!Liang_Barsky_clipper(0,0,width,height,x1,y1,x2,y2))return;
    
    SDL_SetRenderDrawColor(gRenderer, 255, 192, 200, 255);
    SDL_RenderDrawLine(gRenderer , x1,y1,x2,y2);
}
void window::drawNodeAbs(node * n){
    int w,h;
    SDL_Texture * tx;
    const HBB::vec & p = n->position;
    int input = n->input.size();
    int output= n->output.size();

    auto rp = (p-lookAt) * scale;
    int height = std::max(input , output)*2 + 4;
    
    SDL_Rect r,sr;
    
    r.x = rp.X-2;
    r.y = rp.Y-2;
    r.w = 4*scale+4;
    r.h = height*scale+4;
    SDL_SetRenderDrawColor(gRenderer, 32, 32, 32, 255);
    SDL_RenderFillRect(gRenderer,&r);

    r.x = rp.X;
    r.y = rp.Y;
    r.w = 4*scale;
    r.h = height*scale;
    
    SDL_SetRenderDrawColor(gRenderer, 32, 32, 64, 255);
    SDL_RenderFillRect(gRenderer,&r);
    
    r.w = 0.5 * scale;
    r.h = r.w;
    
    SDL_SetRenderDrawColor(gRenderer, 64, 32, 128, 255);
    r.x = rp.X;
    for(int i=0;i<input;i++){
        r.y = rp.Y + (i * 2 + 4.75)*scale;
        SDL_RenderFillRect(gRenderer,&r);

        if(scale>10){
            char buf[64];
            snprintf(buf,sizeof(buf),"%s/%s",n->type->input[i].type.c_str(),n->type->input[i].name.c_str());
            tx = getTexture(buf , w , h);
            sr.x = r.x+scale*0.55;
            sr.y = r.y-scale*0.12;
            sr.h = h*scale*0.015;
            sr.w = w*scale*0.015;
            SDL_RenderCopy(gRenderer,tx,NULL,&sr);
        }
    }
    
    SDL_SetRenderDrawColor(gRenderer, 32, 64, 128, 255);
    r.x = rp.X + 3.5*scale;
    for(int i=0;i<output;i++){
        r.y = rp.Y + (i * 2 + 4.75)*scale;
        SDL_RenderFillRect(gRenderer,&r);

        if(scale>14){
            char buf[64];
            snprintf(buf,sizeof(buf),"%s/%s",n->type->output[i].type.c_str(),n->type->output[i].name.c_str());
            tx = getTexture(buf , w , h);
            sr.x = r.x;
            sr.y = r.y-scale*0.12;
            sr.h = h*scale*0.015;
            sr.w = w*scale*0.015;
            sr.x-= sr.w;
            SDL_RenderCopy(gRenderer,tx,NULL,&sr);
        }
    }
    
    SDL_SetRenderDrawColor(gRenderer, 64, 255, 64 , 255);
    r.x = rp.X;
    r.y = rp.Y + 0.75*scale;
    SDL_RenderFillRect(gRenderer,&r);
    
    SDL_SetRenderDrawColor(gRenderer, 255, 64, 64,  255);
    r.x = rp.X + 3.5*scale;
    //r.y = rp.Y + 0.75*scale;
    SDL_RenderFillRect(gRenderer,&r);
    
    SDL_SetRenderDrawColor(gRenderer, 64, 255, 128, 255);
    //r.x = rp.X + 3.5*scale;
    r.y = rp.Y + 2.75*scale;
    SDL_RenderFillRect(gRenderer,&r);

    if(scale>14){
        tx = getTexture(n->type->name.c_str(),w,h);
        sr.x = rp.X+scale;
        sr.y = rp.Y+scale*1.5;
        sr.h = h*scale*0.03;
        sr.w = w*scale*0.03;

        SDL_RenderCopy(gRenderer,tx,NULL,&sr);
    }
}
SDL_Texture *window:: getTexture(const char * str , int & w , int & h){
    SDL_Texture * tx;
    auto it = textures.find(str);
    if(it == textures.end()){
        SDL_Color color;
        color.r=128;
        color.g=128;
        color.b=128;
        color.a=255;
        SDL_Surface *surf = TTF_RenderText_Blended(font, str , color);
        w=surf->w;
        h=surf->h;
        tx = SDL_CreateTextureFromSurface(gRenderer, surf);
        SDL_FreeSurface(surf);
        textures[str] = std::pair<SDL_Texture*,HBB::vec>(tx,HBB::vec(w,h));
    }else{
        tx = it->second.first;
        w  = it->second.second.X;
        h  = it->second.second.Y;
    }
    return tx;
}
void window::draw(){
    SDL_RenderClear(gRenderer);
    HBB::AABB box;
    box.from = lookAt;
    box.to   = lookAt;
    box.to.X+= width/scale;
    box.to.Y+= height/scale;
    elementlines.collisionTest(&box , [](HBB::AABB * bx,void * arg){
        auto self = (window*)arg;
        auto l    = (line*)bx->data;
        self->drawLineAbs(l->startPosi,l->endPosi);
    },this);
    if(addingLine){
        auto lst = (writeStartRealPosi-lookAt) * scale;
        SDL_SetRenderDrawColor(gRenderer, 255, 192, 200, 255);
        SDL_RenderDrawLine(gRenderer , lst.X,lst.Y,mouse.X,mouse.Y);
    }
    elements.collisionTest(&box , [](HBB::AABB * bx,void * arg){
        auto self = (window*)arg;
        auto n    = (node*)bx->data;
        self->drawNodeAbs(n);
    },this);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderPresent(gRenderer);

    //auto rp = lookAt + mouse/scale;
    //printf("%f,%f\n",rp.X,rp.Y);
}
bool window::pollEvent(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){//按键
            return false;
        }else
        if (event.type == SDL_MOUSEBUTTONDOWN){//按键
            
            if(SDL_BUTTON_LEFT  == event.button.button){
                writeStart(HBB::vec(event.motion.x,event.motion.y));
            }else
            if(SDL_BUTTON_RIGHT == event.button.button){
                dragStart(HBB::vec(event.motion.x,event.motion.y));
            }
        }else
        if (event.type == SDL_MOUSEBUTTONUP){
            if(SDL_BUTTON_LEFT == event.button.button){
                writeRelease(HBB::vec(event.motion.x,event.motion.y));
            }else
            if(SDL_BUTTON_RIGHT == event.button.button){
                dragRelease(HBB::vec(event.motion.x,event.motion.y));
            }
        }else
        if (event.type == SDL_MOUSEMOTION){//移动鼠标
            mouse.X = event.motion.x;
            mouse.Y = event.motion.y;
            if(writing){
                writeSet(HBB::vec(event.motion.x,event.motion.y));
            }
            if(draging){
                dragSet(HBB::vec(event.motion.x,event.motion.y));
            }
        }else
        if (event.type == SDL_MOUSEWHEEL){
            if(event.wheel.y<0){
                scale-=1;
                if(scale<1)
                    scale=1;
            }else
            if(event.wheel.y>0){
                scale+=1;
                if(scale>128)
                    scale=128;
            }
        }else
        if (event.type == SDL_KEYUP){
            
        }
        if (event.type == SDL_KEYDOWN){
            
        }

    }
    return true;
}
void window::editNode(node *n){
    editor.setTable();
    editor.setRows(n->initval.size());
    int i=0;
    for(auto it:n->initval){
        editor.setRow(i,it.first,it.second);
        ++i;
    }
    editor.exec();
    if(editor.removeNode)
        removeNode(n);
    else{
        editor.getVar(n->initval);
    }
}
void window::getInsertingName(std::string & name){
    fTable.setTable();
    fTable.setRows(funcs.size());
    int i=0;
    for(auto it:funcs){
        std::string in,out;
        bool first=true;
        for(auto iit:it.second->input){
            if(!first)
                in+=",";
            in+=iit.type+"/"+iit.name;
            first=false;
        }
        first=true;
        for(auto oit:it.second->output){
            if(!first)
                out+=",";
            out+=oit.type+"/"+oit.name;
            first=false;
        }
        char gSize[64];
        snprintf(gSize,sizeof(gSize),"%d",(int)it.second->size.Y);
        fTable.setRow(i,it.second->name.c_str(),in.c_str(),out.c_str(),gSize);
        ++i;
    }
    fTable.exec();

    name = fTable.functionName;

    if(!name.empty() && fTable.createFunction){
        std::vector<variable> input,output;
        input.clear();
        output.clear();
        for(auto it:fTable.input){
            QStringList var = QString(it.c_str()).split("/");
            if(var.size()>1)
                input.push_back(variable(var[1].toStdString(),var[0].toStdString()));
        }
        for(auto it:fTable.output){
            QStringList var = QString(it.c_str()).split("/");
            if(var.size()>1)
                output.push_back(variable(var[1].toStdString(),var[0].toStdString()));
        }
        addFunc(name,input,output);
    }
}
void window::showMenu(){
    exec();
}
void window::importFile(const std::string & p){
    import(p);
}
void window::saveFile(const std::string & p){
    save(p);
}

}

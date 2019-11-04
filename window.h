#ifndef MESHLANG_WINDOW
#define MESHLANG_WINDOW
#include "menu.h"
#include "node.h"
#include "functable.h"
#include "funceditor.h"
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <QInputDialog>
namespace meshlang{
    class window:public program,menu{
        public:
            int width,height;
            HBB::vec lookAt;
            float scale;
            
            void writeStart(const HBB::vec & p);
            void writeSet(const HBB::vec & p);
            void writeRelease(const HBB::vec & p);
            
            void dragStart(const HBB::vec & p);
            void dragSet(const HBB::vec & p);
            void dragRelease(const HBB::vec & p);
            
            window();
            ~window();
            void draw();
            bool pollEvent();
        private:
            HBB::vec dragStartPosi,dragRealPosi;
            HBB::vec writeStartRealPosi;
            
            bool addingLine;

            SDL_Window* gWindow;
            SDL_Surface* gScreenSurface;
            SDL_Renderer* gRenderer;
            TTF_Font * font;

            HBB::vec mouse;
            funcTable fTable;
            funcEditor editor;
            
            void drawLineAbs(const HBB::vec & f , const HBB::vec & t);
            void drawNodeAbs(node * n);
            
            std::unordered_map<std::string,std::pair<SDL_Texture*,HBB::vec> > textures;

            virtual void getInsertingName(std::string & name);
            virtual void editNode(node *);
            virtual void showMenu();
            virtual void importFile(const std::string &);
            virtual void saveFile(const std::string &);

            bool draging,writing;

            SDL_Texture * getTexture(const char * str , int & w , int & h);
        public:
            HBB::vec writeStartPosi,writePosi;
    };
}
#endif

#ifndef MESHLANG_WINDOW
#define MESHLANG_WINDOW
#include "node.h"
#include "functable.h"
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <QInputDialog>
namespace meshlang{
    class window:public program{
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
            
            void drawLineAbs(const HBB::vec & f , const HBB::vec & t);
            void drawNodeAbs(node * n);
            
            std::unordered_map<std::string,SDL_Texture*> textures;

            virtual void getInsertingName(std::string & name);

            bool draging,writing;
        public:
            HBB::vec writeStartPosi,writePosi;
    };
}
#endif

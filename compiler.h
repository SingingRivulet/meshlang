#ifndef MESHLANG_COMPILER
#define MESHLANG_COMPILER
#include "node.h"
#include <set>
#include <unordered_map>
namespace meshlang{

class compiler:public program{
    public:
        compiler();
        void compile(const std::string & path);
    private:
        void compile_start();
        void compile_flag_init();
        std::map<std::string,node*> funcMap;
        std::set<int> markMap;
        void compile_function(node * );

        void compile_resetNode();
        void compile_function_begin(node * );
        void compile_function_end(node * );

        void compile_add_goto(node * );
        void compile_mark(node * );
        void compile_addNode(node * );
        void compile_declareNode(node * );
        void compile_addExec(node * );

        void compile_add_if(node * cond , node * t , node * f);
        void compile_add_return();

        void compile_add_ifFalseReturn(node * cond);
        void compile_add_ifTrueReturn(node * cond);

        FILE * fp;
};

}
#endif // COMPILER

#include "compiler.h"
#include <stack>
namespace meshlang{

compiler::compiler(){
}
void compiler::compile(const std::string & path){
    fp = fopen(path.c_str(),"w");
    if(fp){
        compile_start();
        fclose(fp);
    }
}
void compiler::compile_start(){
    funcMap.clear();
    for(auto it:nodes){
        if(!it->name.empty() && it->type->isPrivate){
            funcMap[it->name] = it;
        }
    }
    for(auto it:funcMap){
        compile_flag_init();
        compile_resetNode();
        compile_function_begin(it.second);
        it.second->compileFlag = 1;
        if(it.second->trueThen)
            compile_function(it.second->trueThen->end);
        compile_function_end(it.second);
    }
    compile_resetNode();
}
void compiler::compile_flag_init(){
    for(auto it:nodes){
        it->compileFlag = 0;
    }
}
void compiler::compile_function(node * nt){
    auto n = nt;
    while(1){
        if(n->compileFlag==1){//此节点被编译过
            compile_add_goto(n);
            return;
        }
        n->compileFlag=1;

        if(n->last.size()>1){//多于1个父节点，说明存在跳转，插入标号
            compile_mark(n);
        }

        //添加关联节点
        for(auto it:n->input){
            for(auto itn:it){
                compile_addNode(itn->start);
            }
        }
        for(auto it:n->output){
            for(auto itn:it){
                compile_addNode(itn->end);
            }
        }
        compile_addNode(n);//添加本身

        compile_addExec(n);//执行节点

        if(n->trueThen && n->falseThen){//是否存在继续节点

            if(n->trueThen->end == n->falseThen->end){
                n = n->trueThen->end;
                continue;
            }else{
                compile_add_if(n,n->trueThen->end,n->falseThen->end);

                if(n->trueThen->end->compileFlag!=1){
                    compile_mark(n->trueThen->end);
                    compile_function(n->trueThen->end);
                    compile_add_return();
                }

                if(n->falseThen->end->compileFlag!=1){
                    compile_mark(n->falseThen->end);
                    compile_function(n->falseThen->end);
                    compile_add_return();
                }
            }

            return;
        }else
        if(n->trueThen && n->falseThen==NULL){
            compile_add_ifFalseReturn(n);
            n = n->trueThen->end;
        }else
        if(n->trueThen==NULL && n->falseThen){
            compile_add_ifTrueReturn(n);
            n = n->falseThen->end;
        }else
            return;
    }
}

void compiler::compile_resetNode(){
    markMap.clear();
    marked.clear();
}
void compiler::compile_addNode(node * n){
    if(markMap.find(n->id)==markMap.end()){
        compile_declareNode(n);
        markMap.insert(n->id);
    }
}
#define dumpCode(s) \
    if(fp){ \
        fprintf(fp,"%s",s); \
    }

void compiler::compile_function_begin(node * n){
    std::string str = std::string("struct meshlang_module_")+n->name+"{\n";
    for(auto it:n->type->types){
        str += std::string("  ")+it.second+" "+it.first+";\n";
    }
    str+="};\n";
    str+=std::string("void meshlang_module_init_")+n->name+"(struct meshlang_module_"+n->name+" * m){\n";
    for(auto it:n->initval){
        if(it.second.empty())
            continue;
        if(n->type->types[it.first]=="string"){
            str+=std::string("  m->")+it.first+" = \""+it.second+"\";\n";
        }else{
            str+=std::string("  m->")+it.first+" = "+it.second+";\n";
        }
    }
    str+="}\n";
    char buf[128];
    snprintf(buf,128,"tmp_%d_p",n->id);
    markMap.insert(n->id);
    str+=std::string("void meshlang_module_exec_")+n->name+"(struct meshlang_module_"+n->name+" * "+buf+"){\n    "+buf+"->result = 0;\n";

    dumpCode(str.c_str());
}
void compiler::compile_function_end(node * ){
    dumpCode("\n}\n");
}

void compiler::compile_add_goto(node * n){
    char buf[128];
    snprintf(buf,128,"    goto nodeMk%d;\n",n->id);
    dumpCode(buf);
}
void compiler::compile_mark(node * n){
    if(marked.find(n->id)!=marked.end())
        return;
    marked.insert(n->id);
    char buf[128];
    snprintf(buf,128,"  nodeMk%d:\n",n->id);
    dumpCode(buf);
}
void compiler::compile_addExec(node * n){
    char buf[1024];
    snprintf(buf,1024,"    meshlang_module_exec_%s(tmp_%d_p);\n",n->type->name.c_str(),n->id);
    std::string str = buf;

    int len = n->output.size();
    for(int i=0;i<len;++i){//复制变量
        for(auto it:n->output[i]){
            snprintf(buf,sizeof(buf),"    tmp_%d_o.%s=tmp_%d_o.%s;\n" ,
                    it->end->id , it->end->type->input[it->endId].name.c_str() ,
                    n->id       , n->type->output[i].name.c_str());
            str+=buf;
        }
    }
    dumpCode(str.c_str());
}
void compiler::compile_declareNode(node * n){
    char buf[4096];
    snprintf(buf,sizeof(buf),
             "    struct meshlang_module_%s tmp_%d_o;\n"
             "    struct meshlang_module_%s * tmp_%d_p = &tmp_%d_o;\n"
             "    meshlang_module_init_%s(tmp_%d_p);\n",
             n->type->name.c_str(),n->id,
             n->type->name.c_str(),n->id,n->id,
             n->type->name.c_str(),n->id);
    dumpCode(buf);
}

void compiler::compile_add_if(node * n , node * t , node * f){
    char buf[1024];
    if(t==f)
        snprintf(buf,sizeof(buf),"  goto nodeMk%d;\n" , t->id);
    else
        snprintf(buf,sizeof(buf),"    if(tmp_%d_o.result)\n      goto nodeMk%d;\n    else\n      goto nodeMk%d;\n" , n->id , t->id , f->id);
    dumpCode(buf);
}
void compiler::compile_add_return(){
    dumpCode("    return;\n");
}

void compiler::compile_add_ifFalseReturn(node * n){
    char buf[1024];
    snprintf(buf,sizeof(buf),"    if(tmp_%d_o.result==0)\n      return;\n" , n->id);
    dumpCode(buf);
}
void compiler::compile_add_ifTrueReturn(node * n){
    char buf[1024];
    snprintf(buf,sizeof(buf),"    if(tmp_%d_o.result)\n      return;\n" , n->id);
    dumpCode(buf);
}

}

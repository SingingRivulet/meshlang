# MeshLang  
简易可视化编程语言（代码生成器）  
## 简介  
本工具使用qt编写，可通过绘制流程图的方式生成c语言代码  

![img](./shot.png)  
=============================================
目前效果  
```
struct meshlang_module_start{
  int b;
  int a;
};
void meshlang_module_init_start(struct meshlang_module_start * m){
  int result;
  m->b = 0;
  m->a = 0;
}
void meshlang_module_exec_start(struct meshlang_module_start * tmp_1_p){
    tmp_1_p->result = 0;
    struct meshlang_module_main tmp_2_o;
    struct meshlang_module_main * tmp_2_p = &tmp_2_o;
    meshlang_module_init_main(tmp_2_p);
    meshlang_module_exec_main(tmp_2_p);
    if(tmp_2_o.result)
      goto nodeMk5;
    else
      goto nodeMk3;
  nodeMk5:
    struct meshlang_module_abc tmp_6_o;
    struct meshlang_module_abc * tmp_6_p = &tmp_6_o;
    meshlang_module_init_abc(tmp_6_p);
    struct meshlang_module_test tmp_5_o;
    struct meshlang_module_test * tmp_5_p = &tmp_5_o;
    meshlang_module_init_test(tmp_5_p);
    meshlang_module_exec_test(tmp_5_p);
    tmp_6_o.a=tmp_5_o.a;
    if(tmp_5_o.result==0)
      return;
    struct meshlang_module_cde tmp_7_o;
    struct meshlang_module_cde * tmp_7_p = &tmp_7_o;
    meshlang_module_init_cde(tmp_7_p);
    meshlang_module_exec_abc(tmp_6_p);
    tmp_7_o.b=tmp_6_o.b;
    if(tmp_6_o.result)
      return;
    meshlang_module_exec_cde(tmp_7_p);
    return;
  nodeMk3:
    struct meshlang_module_test tmp_3_o;
    struct meshlang_module_test * tmp_3_p = &tmp_3_o;
    meshlang_module_init_test(tmp_3_p);
    meshlang_module_exec_test(tmp_3_p);
    if(tmp_3_o.result==0)
      return;
    struct meshlang_module_def tmp_8_o;
    struct meshlang_module_def * tmp_8_p = &tmp_8_o;
    meshlang_module_init_def(tmp_8_p);
    struct meshlang_module_test tmp_4_o;
    struct meshlang_module_test * tmp_4_p = &tmp_4_o;
    meshlang_module_init_test(tmp_4_p);
    meshlang_module_exec_test(tmp_4_p);
    tmp_8_o.b=tmp_4_o.a;
    if(tmp_4_o.result==0)
      return;
    meshlang_module_exec_def(tmp_8_p);
    return;
}
```

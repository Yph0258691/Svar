#include "Svar.h"
#include "Registry.h"

using namespace GSLAM;

int sample_json(Svar config){
    Svar i=1;
    Svar d=2.;
    Svar s="hello world";
    Svar b=false;
    Svar v={1,2,"s",false};
    Svar m={{"i",1},{"d",d},{"n",nullptr}};

    if(i.is<int>()){
        int it=i.as<int>();
    }

    if(d.is<double>()){
        d=d+3.;
    }

    if(v.isArray()){
        for(auto a:v) std::cout<<a<<std::endl;
    }

    if(m.isObject()){
        for(std::pair<std::string,Svar> it:m)
            std::cout<<it.first<<":"<<it.second<<std::endl;
    }

    std::string json=m.dump_json();
    std::cout<<json<<std::endl;

    m=Svar::parse_json(json);
    return 0;
}

void c_func_demo(Svar arg){
    std::cout<<"Calling a c_func with arg "<<arg<<std::endl;
}

int sample_func(){
    Svar c_func=c_func_demo;
    c_func("I love Svar");

    Svar lambda=Svar::lambda([](std::string arg){std::cout<<arg<<std::endl;});
    lambda("Using a lambda");

    Svar member_func(&SvarBuffer::md5);
    std::cout<<"Obtain md5 with member function: "
            <<member_func(SvarBuffer(3)).as<std::string>()<<std::endl;

    return 0;
}

int sample_cppclass()
{
    // 1. define a class with c++
    struct MyClass{
        MyClass(int b=2):a(b){}
        static MyClass create(){return MyClass();}
        void print(){
            std::cerr<<a;
        }

        int a;
    };

    // 2. define the class members to Svar
    Class<MyClass>("DemoClass")// rename to DemoClass?
            .construct<int>()
            .def("print",&MyClass::print)
            .def_static("create",&MyClass::create)
            .def_readwrite("a",&MyClass::a);

    // 3. use the class with c++ or other languages
    Svar DemoClass=SvarClass::instance<MyClass>();

    Svar inst=DemoClass(3);
    inst.call("print");

    Svar inst2=DemoClass.call("create");
    int  a=inst2["a"].as<int>();
    std::cout<<a;

    return 0;
}

int sample_svarclass(){// users can also define a class without related c++ class
    // 1. Directly define a class in Svar
    SvarClass helloClass("hello",typeid(SvarObject));
    helloClass.def_static("print",[](Svar rh){
        std::cerr<<"print:"<<rh<<std::endl;
    })
    .def("printSelf",[](Svar self){
        std::cerr<<"printSelf:"<<self<<std::endl;
    })
    .def("__init__",[helloClass](){
        Svar ret={{"a",100}};
        ret.as<SvarObject>()._class=helloClass;
        return ret;
    });

    Svar cls(helloClass);
    Svar inst=cls();
    std::cerr<<helloClass;
    inst.call("print","hello print.");
    inst.call("printSelf");
    return 0;
}

int sample_module(Svar config){
    Svar sampleModule=Registry::load("sample_module");

    Svar ApplicationDemo=sampleModule["ApplicationDemo"];

    Svar instance=ApplicationDemo("zhaoyong");
    std::cout<<instance.call("gslam_version");

    std::cout<<ApplicationDemo.as<SvarClass>();
    return 0;
}

int sample(Svar config){
    bool json=config.arg("json",false,"run json sample");
    bool func=config.arg("func",false,"run function usage sample");
    bool cppclass=config.arg("cppclass",false,"run cpp class sample");
    bool svarclass=config.arg("svarclass",false,"run svar class sample");
    bool module=config.arg("module",false,"show how to use a svar module");

    if(config.get("help",false)) return config.help();

    if(json) return sample_json(config);
    if(func) return sample_func();
    if(cppclass) return sample_cppclass();
    if(svarclass) return sample_svarclass();
    if(module) return sample_module(config);

    return 0;
}

REGISTER_SVAR_MODULE(sample){
    svar["apps"]["sample"]={sample,"This is a collection of sample to show Svar usages"};
}

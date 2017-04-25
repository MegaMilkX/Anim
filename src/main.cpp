#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

template<typename Ret>
inline void Call(Ret(*fn)(), float* values)
{ (*fn)(); }

template<typename Ret, typename Arg0>
inline void Call(Ret(*fn)(Arg0), float* values)
{ (*fn)(values[0]); }

template<typename Ret, typename Arg0, typename Arg1>
inline void Call(Ret(*fn)(Arg0, Arg1), float* values)
{ (*fn)(values[0], values[1]); }

template<typename Ret, typename Arg0, typename Arg1, typename Arg2>
inline void Call(Ret(*fn)(Arg0, Arg1, Arg2), float* values)
{ (*fn)(values[0], values[1], values[2]); }

template<typename Ret, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
inline void Call(Ret(*fn)(Arg0, Arg1, Arg2, Arg3), float* values)
{ (*fn)(values[0], values[1], values[2], values[3]); }

template<typename Ret, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline void Call(Ret(*fn)(Arg0, Arg1, Arg2, Arg3, Arg4), float* values)
{ (*fn)(values[0], values[1], values[2], values[3], values[4]); }

template<typename Ret, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
inline void Call(Ret(*fn)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), float* values)
{ (*fn)(values[0], values[1], values[2], values[3], values[4], values[5]); }



template<typename Class, typename Ret>
inline void CallMember(Ret(Class::*fn)(), Class* _this, float* values)
{ (_this->*fn)(); }

template<typename Class, typename Ret, typename Arg0>
inline void CallMember(Ret(Class::*fn)(Arg0), Class* _this, float* values)
{ (_this->*fn)(values[0]); }

template<typename Class, typename Ret, typename Arg0, typename Arg1>
inline void CallMember(Ret(Class::*fn)(Arg0, Arg1), Class* _this, float* values)
{ (_this->*fn)(values[0], values[1]); }

template<typename Class, typename Ret, typename Arg0, typename Arg1, typename Arg2>
inline void CallMember(Ret(Class::*fn)(Arg0, Arg1, Arg2), Class* _this, float* values)
{ (_this->*fn)(values[0], values[1], values[2]); }

template<typename Class, typename Ret, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
inline void CallMember(Ret(Class::*fn)(Arg0, Arg1, Arg2, Arg3), Class* _this, float* values)
{ (_this->*fn)(values[0], values[1], values[2], values[3]); }

template<typename Class, typename Ret, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline void CallMember(Ret(Class::*fn)(Arg0, Arg1, Arg2, Arg3, Arg4), Class* _this, float* values)
{ (_this->*fn)(values[0], values[1], values[2], values[3], values[4]); }

template<typename Class, typename Ret, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
inline void CallMember(Ret(Class::*fn)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), Class* _this, float* values)
{ (_this->*fn)(values[0], values[1], values[2], values[3], values[4], values[5]); }

struct IFunctor
{
    virtual ~IFunctor() {}
    virtual void operator()(float* values)
    {
        
    }
};

template<typename Ret, typename... Args>
struct Functor : public IFunctor{
    Functor(Ret(*fn)(Args...)):fn(fn){}
    virtual void operator()(float* values){
        Call<Ret, Args...>(fn, values);
    }
private:
    Ret(*fn)(Args...);
};

template<typename Class, typename Ret, typename... Args>
struct FunctorMember : public IFunctor{
    FunctorMember(Ret(Class::*fn)(Args...), Class* _this):fn(fn), _this(_this) {}
    virtual void operator()(float* values){
        CallMember<Class, Ret, Args...>(fn, _this, values);
    }
private:
    Ret(Class::*fn)(Args...);
    Class* _this;
};

struct Keyframe
{
    Keyframe(): frame(0) {}
    Keyframe(int frame): frame(frame) {}
    void operator=(float value)
    { this->value = value; }
    bool operator<(const Keyframe& other)
    { return frame < other.frame; }
    int frame;
    float value;
};

class Curve
{
public:
    Curve(): _func(0), length(0) {}
    ~Curve() { if(_func) delete _func; }
    
    Curve& operator[](const std::string& name)
    {
        Curve& c = curves[name];
        curveValues.resize(curves.size());
        return c;
    }
    
    Keyframe& operator[](int frame)
    {
        if(frame > length) length = frame;
        for(unsigned i = 0; i < keyframes.size(); ++i)
        {
            if(keyframes[i].frame == frame)
            {
                return keyframes[i];
            }
        }
        
        keyframes.push_back(Keyframe(frame));
        std::sort(keyframes.begin(), keyframes.end());
        return operator[](frame);
    }
    
    template<typename Ret, typename... Args>
    void OnEvaluate(Ret(*fn)(Args...))
    {
        if(_func) delete _func;
        _func = new Functor<Ret, Args...>(fn);
    }
    
    template<typename Class, typename Ret, typename... Args>
    void OnEvaluate(Ret(Class::*fn)(Args...), Class* thisPtr)
    {
        if(_func) delete _func;
        _func = new FunctorMember<Class, Ret, Args...>(fn, thisPtr);
    }
    
    float Evaluate(int time)
    {
        std::map<std::string, Curve>::iterator it =
            curves.begin();
        unsigned i = 0;
        for(it; it != curves.end(); ++it)
        {
            curveValues[i++] = it->second.Evaluate(time);
        }
        
        if(length)
        {           
            int time_overflow = time / length * length;
            time = time - time_overflow;
            
            Keyframe* k0, *k1;
            
            for(int i = keyframes.size() - 1; i >= 0; --i)
            {
                k0 = &keyframes[i];
                if(i == keyframes.size() - 1)
                    k1 = &keyframes[0];
                else
                    k1 = &keyframes[i + 1];
                if(k0->frame <= time)
                    break;
            }
            
            value = k0->value;
        }
        
        if(_func)
            _func->operator()(curveValues.data());
        
        return value;
    }
    
    void Print()
    {
        if(curves.empty())
        {
            std::cout << value << " ";
            return;
        }
            
        std::map<std::string, Curve>::iterator it =
            curves.begin();
        for(it; it != curves.end(); ++it)
        {
            std::cout << it->first << ": ";
            it->second.Print();
        }
        
        std::cout << std::endl;
    }
    
    float value;
private:
    IFunctor* _func;
    int length;
    std::vector<Keyframe> keyframes;
    std::vector<float> curveValues;
    std::map<std::string, Curve> curves;
};

struct Object
{
    void Ping() { std::cout << "LA" << std::endl; }
    void Position(float x, float y, float z)
    {
        std::cout 
            << "x: " 
            << x 
            << " y: " 
            << y 
            << " z: " 
            << z 
            << std::endl;
    }
};

void Test()
{
    std::cout << "FU" << std::endl;
}

int main()
{
    Curve anim;
    anim["Transform"]["Position"]["x"][0] = 100.0f;
    anim["Transform"]["Position"]["x"][10] = 160.0f;
    anim["Transform"]["Position"]["x"][20] = 40.0f;
    anim["Transform"]["Position"]["x"][30] = 100.0f;
    anim["Transform"]["Position"]["y"];
    anim["Transform"]["Position"]["z"];
    anim["Transform"]["Scale"]["x"];
    anim["Transform"]["Scale"]["y"];
    anim["Transform"]["Scale"]["z"];
    anim["Transform"]["Rotation"]["x"];
    anim["Transform"]["Rotation"]["y"];
    anim["Transform"]["Rotation"]["z"];
    
    
    Object obj;
    anim["Transform"]["Position"].OnEvaluate(&Object::Position, &obj);

    int time = 0;
    while(true)
    {
        time++;
        std::cout << "time: " << time << std::endl;
        anim.Evaluate(time);
        
        system("cls");
    }
    
    std::getchar();

    return 0;
}

#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/Types.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/exec/WorkletMapField.h>

struct ArbFunctorWorklet : dax::exec::WorkletMapField
{
    typedef void ControlSignature(Field(In), ExecObject(), Field(Out));
    typedef _3 ExecutionSignature(_1, _2);

    template<typename T, typename Functor>
    T operator()(T t, Functor const& f) const
    {
        return f.mult(t);
    }
};

class Functor1 : public dax::exec::ExecutionObjectBase
{
public:
    Functor1() : con(20) {}

    template<typename T>
    DAX_EXEC_EXPORT
    T operator()(const T& t) const
    {
        return t * t;
    }

    template<typename T>
    DAX_EXEC_EXPORT
    T mult100(const T& t) const
    {
        return t * 100;
    }

    template<typename T>
    DAX_EXEC_EXPORT
    T mult(const T& t) const
    {
        return t * con;
    }

protected:
    int con;

private:
};

int main(void)
{
    std::vector<dax::Id> in(10);
    for (unsigned int i = 0; i < 10; ++i)
        in[i] = i;
    dax::cont::ArrayHandle<dax::Id> input = dax::cont::make_ArrayHandle(in);
    dax::cont::ArrayHandle<dax::Id> output;
    dax::cont::Scheduler<> scheduler;
    Functor1 functor;
    scheduler.Invoke(ArbFunctorWorklet(), input, functor, output);

    std::vector<dax::Id> out(output.GetNumberOfValues());
    output.CopyInto(out.begin());
    std::cout << "out: ";
    for (unsigned int i = 0; i < out.size(); ++i)
        std::cout << out[i] << ", ";
    std::cout << std::endl;

    return 0;
}

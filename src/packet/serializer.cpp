#include <string>
#include <tuple>

//! \brief A pure virtual class for packet parser to inherit.
//! This is the adaptor to decode an upcoming string.
class Decoder
{
public:
    //! \brief The function of the decoder will be called for
    //! a certain amount of times defined by the binded function
    //! ReachEnd() can be used to check whether there still has
    //! remaining parameters in the string.
    virtual bool ReachEnd() = 0;
    //! \brief Because the number of times that the decoder parse
    //! the upcoming string is defined by the binded function, so
    //! ElementNumberEqual() should do an intergrity check for packet
    //! before performing the parser for an upcoming string.
    //! \param para_number the number of paramters of the binded function.
    virtual bool ElementNumberEqual(size_t para_number) = 0;

    //! \brief Get the next value, which should be bool, of the upcoming string
    virtual bool DecodeNextBool() = 0;

    //! \brief Get the next value, which should be int, of the upcoming string
    virtual int DecodeNextInt() = 0;

    //! \brief Get the next value, which should be double, of the upcoming string
    virtual double DecodeNextDouble() = 0;

    //! \brief Get the next value, which should be string, of the upcoming string
    virtual std::string DecodeNextString() = 0;

    // Todo decode object / array
};

template <typename... Args>
auto DecodeProcess(const Decoder& Dc, ) -> std::tuple<Args...>
{
}
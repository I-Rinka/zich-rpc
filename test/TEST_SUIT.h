#define TEST(t)                                                                                                                                          \
    (bool)(t);                                                                                                                                           \
    if (!(t))                                                                                                                                            \
    {                                                                                                                                                    \
        std::cout << "\033[1;31m[Failed]\033[0m at " << __FILE__ << ":" << __LINE__ << " in function \033[1;33m" << __FUNCTION__ << "()\033[0m" << endl; \
    }                                                                                                                                                    \
    else                                                                                                                                                 \
    {                                                                                                                                                    \
        std::cout << "\033[1;32m[Pass]\033[0m " << __FILE__ << ":" << __LINE__ << endl;                                                                  \
    }

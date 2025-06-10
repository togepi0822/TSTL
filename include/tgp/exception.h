#ifndef TSTL_INCLUDE_TGP_EXCEPTION_H
#define TSTL_INCLUDE_TGP_EXCEPTION_H

#include <tgp/config.h>

NAMESPACE_TGP_BEGIN

template<class Exception>
[[noreturn]] inline void TGP_TRY_THROW(Exception&& e) {
#ifdef TGP_HAS_EXCEPTIONS
    throw e;
#else
    static_cast<void>(e);
    std::terminate();
#endif
}

NAMESPACE_TGP_END

#endif // end of TSTL_INCLUDE_TGP_EXCEPTION_H

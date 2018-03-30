#ifndef BIP47_PAYMENT_CODE_HPP
#define BIP47_PAYMENT_CODE_HPP

#include <payment_code.hpp>

class contacts {
public:
    void add_contact(hd_public contact);
    void add_contact(payment_code contact);
    void payment_code(hd_public contact);
};

#endif

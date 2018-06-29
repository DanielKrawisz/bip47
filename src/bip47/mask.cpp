#include <bip47/payment_code.hpp>

namespace bip47
{

typedef libbitcoin::wallet::ec_public ec_public;

namespace low
{

typedef libbitcoin::long_hash mask;

const mask payment_code_mask(const ec_secret& designated, const ec_compressed& point, const outpoint& outpoint) {
    ec_compressed secret_point(point);
    
    // TODO figure out something better to do here if the operation fails. 
    if (!libbitcoin::ec_multiply(secret_point, designated)) throw 0;
    
    // TODO replace all the copying with something more efficient. 
    data op = outpoint.to_data();
    
    data splice(libbitcoin::ec_compressed_size + op.size());
    
    int n = 0;
    for (; n < libbitcoin::ec_compressed_size; n++) splice[n] = secret_point[n];
    
    for (int i = 0; i < op.size(); i++) {
        splice[n] = op[i];
        n++;
    }
    
    return libbitcoin::sha512_hash(splice);
}

const payment_code mask_payment_code(const low::payment_code& code, const mask mask) {
    payment_code data(code);
    
    for (int i = 0; i < libbitcoin::long_hash_size; i++) {
        data[i + 3] = code[i + 3] ^ mask[i];
    }
    
    return data;
}

const payment_code mask_payment_code(const low::payment_code& code, const ec_secret& designated, const ec_compressed& point, const outpoint& outpoint) {
    return mask_payment_code(code, payment_code_mask(designated, point, outpoint));
}

bool unmask_payment_code(payment_code& code, const ec_public& designated, const ec_secret& pk, const outpoint& outpoint) {
    ec_compressed secret_point(designated);
    
    if (!libbitcoin::ec_multiply(secret_point, pk)) return false;
    
    // TODO replace all the copying with something more efficient. 
    data op = outpoint.to_data();
    
    data splice(libbitcoin::ec_compressed_size + op.size());
    
    int n = 0;
    for (; n < libbitcoin::ec_compressed_size; n++) splice[n] = secret_point[n];
    
    for (int i = 0; i < op.size(); i++) {
        splice[n] = op[i];
        n++;
    }
    
    mask mask = libbitcoin::sha512_hash(splice);
    
    for (int i = 0; i < libbitcoin::long_hash_size; i++) {
        code[n] ^= mask[i];
        n++;
    }
    
    return true;
}

} // low

} // bip47

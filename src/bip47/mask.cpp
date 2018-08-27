#include <bip47/payment_code.hpp>

namespace bip47
{

typedef libbitcoin::wallet::ec_public ec_public;

namespace low
{

typedef libbitcoin::long_hash mask;

const bool payment_code_mask(mask& m, const ec_secret& designated, const ec_compressed& point, const outpoint& outpoint) {
    ec_compressed secret_point(point);
    
    if (!libbitcoin::ec_multiply(secret_point, designated)) return false;
    
    data op = outpoint.to_data();
    
    data splice(libbitcoin::ec_compressed_size + op.size());
    
    int n = 0;
    for (; n < libbitcoin::ec_compressed_size; n++) splice[n] = secret_point[n];
    
    for (int i = 0; i < op.size(); i++) {
        splice[n] = op[i];
        n++;
    }
    
    m = libbitcoin::sha512_hash(splice);
    return true;
}

const payment_code mask_payment_code(const low::payment_code& code, const mask mask) {
    payment_code data(code);
    
    for (int i = 0; i < libbitcoin::long_hash_size; i++) {
        data[i + 3] = code[i + 3] ^ mask[i];
    }
    
    return data;
}

const payment_code mask_payment_code(const low::payment_code& code, const ec_secret& designated, const ec_compressed& point, const outpoint& outpoint) {
    mask m;
    if (!payment_code_mask(m, designated, point, outpoint)) return null_payment_code;
    return mask_payment_code(code, m);
}

bool unmask_payment_code(payment_code& code, const ec_public& designated, const ec_secret& pk, const outpoint& outpoint) {
    ec_compressed secret_point(designated);
    
    if (!libbitcoin::ec_multiply(secret_point, pk)) return false;
    
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

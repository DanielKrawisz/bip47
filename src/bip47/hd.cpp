#include <bip47/hd.hpp>

using namespace std;

namespace bip47
{
    
const data_chunk hd_public::data() const {
    data_chunk data(libbitcoin::ec_compressed_size + libbitcoin::wallet::hd_chain_code_size);
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        data[n] = point[i];
        n++;
    }
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        data[n] = chain_code[i];
        n++;
    }
    
    return data;
}

const hd_public hd_public::from_data(data_chunk data) {
    if (data.size() != libbitcoin::ec_compressed_size + libbitcoin::wallet::hd_chain_code_size) 
        return hd_public();
    ec_compressed point;
    hd_chain_code chain_code;
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        point[i] = data[n];
        n++;
    }
    
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        chain_code[i] = data[n];
        n++;
    }
    return {point, chain_code};
}
    
const data_chunk hd_secret::data() const {
    data_chunk data(libbitcoin::ec_secret_size + libbitcoin::wallet::hd_chain_code_size);
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        data[n] = key[i];
        n++;
    }
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        data[n] = chain_code[i];
        n++;
    }
    
    return data;
}

const hd_secret hd_secret::from_data(data_chunk data) {
    if (data.size() != libbitcoin::ec_secret_size + libbitcoin::wallet::hd_chain_code_size) 
        return hd_secret();
    ec_secret key;
    hd_chain_code chain_code;
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        key[i] = data[n];
        n++;
    }
    
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        chain_code[i] = data[n];
        n++;
    }
    return {key, chain_code};
}

} // bip47

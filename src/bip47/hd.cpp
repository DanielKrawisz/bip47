#include <bip47/hd.hpp>

namespace bip47
{
    
namespace hd
{
    
const data_chunk data(const hd_public& h)
{
    data_chunk data(libbitcoin::ec_compressed_size + libbitcoin::wallet::hd_chain_code_size);
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        data[n] = h.Point[i];
        n++;
    }
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        data[n] = h.ChainCode[i];
        n++;
    }
    
    return data;
}

const data_chunk data(const hd_secret& h)
{
    data_chunk data(libbitcoin::ec_secret_size + libbitcoin::wallet::hd_chain_code_size);
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        data[n] = h.Secret[i];
        n++;
    }
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        data[n] = h.Pubkey.ChainCode[i];
        n++;
    }
    
    return data;
}
    
bool from_data(const data_chunk data, hd_public& h)
{
    if (data.size() != libbitcoin::ec_compressed_size + libbitcoin::wallet::hd_chain_code_size) 
        return false;
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        h.Point[i] = data[n];
        n++;
    }
    
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        h.ChainCode[i] = data[n];
        n++;
    }
    return h.valid();
}

bool from_data(const data_chunk data, hd_secret& h) {
    if (data.size() != libbitcoin::ec_secret_size + libbitcoin::wallet::hd_chain_code_size) 
        return false;
    int n = 0;
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        h.Secret[i] = data[n];
        n++;
    }
    
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        h.Pubkey.ChainCode[i] = data[n];
        n++;
    }
    
    h.Pubkey.Point = low::to_public(h.Secret);
    return h.valid();
}

} // hd

} // bip47

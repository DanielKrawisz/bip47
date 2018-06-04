#include <bip47/hd.hpp>
#include <bip47/low.hpp>

using namespace std;

namespace bip47
{

const ec_secret null_ec_secret = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const ec_secret null_hd_chain_code = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

data_chunk hd_public::data() const {
    data_chunk data(libbitcoin::ec_compressed_size + libbitcoin::wallet::hd_chain_code_size);
    auto it = begin(data);
    copy(begin(point), end(point), it);
    copy(begin(chain_code), end(chain_code), it);
    return data;
}

bool inline hd_public::valid() const {
    return point != libbitcoin::null_compressed_point;
}

bool inline hd_public::operator== (hd_public code) const {
    return point == code.point && chain_code == code.chain_code;
}

bool inline hd_public::operator!= (hd_public code) const {
    return point != code.point || chain_code != code.chain_code;
}
    
data_chunk hd_secret::data() const {
    data_chunk data(libbitcoin::ec_secret_size + libbitcoin::wallet::hd_chain_code_size);
    auto it = begin(data);
    copy(begin(key), end(key), it);
    copy(begin(chain_code), end(chain_code), it);
    return data;
}

const hd_secret hd_secret::from_data(data_chunk data) {
    if (data.size() != libbitcoin::ec_secret_size + libbitcoin::wallet::hd_chain_code_size) 
        return {null_ec_secret, null_hd_chain_code};
    ec_secret key;
    hd_chain_code chain_code;
    auto it = begin(data);
    copy(it, begin(data) + libbitcoin::ec_secret_size, begin(key));
    copy(it, end(data), begin(chain_code));
    return {key, chain_code};
}
    
const hd_public inline hd_secret::pubkey() const {
    return {low::to_public(key), chain_code};
}

bool inline hd_secret::valid() const {
    return key != null_ec_secret;
}

bool inline hd_secret::operator== (hd_secret code) const {
    return key == code.key && chain_code == code.chain_code;
}

const hd_public hd_public::from_data(data_chunk data) {
    if (data.size() != libbitcoin::ec_compressed_size + libbitcoin::wallet::hd_chain_code_size) 
        return {libbitcoin::null_compressed_point, null_hd_chain_code};
    ec_compressed point;
    hd_chain_code chain_code;
    auto it = begin(data);
    copy(it, begin(data) + libbitcoin::ec_secret_size, begin(point));
    copy(it, end(data), begin(chain_code));
    return {point, chain_code};
}

} // bip47

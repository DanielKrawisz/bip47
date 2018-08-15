#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>
#include <gtest/gtest.h>
#include "test_keys.hpp"

using namespace std;
using namespace bip47;

const libbitcoin::wallet::hd_key hd_pair::libbitcoin_hd_key_secret() {
    libbitcoin::wallet::hd_key k;
    int n = 0;
    for (; n < 13; n++) {
        k[n] = 0;
    }
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        k[n] = Secret.Pubkey.ChainCode[i];
        n++;
    }
    
    k[n] = 0;
    n++;
    
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        k[n] = Secret.Secret[i];
        n++;
    }
    
    return k;
}

const libbitcoin::wallet::hd_key hd_pair::libbitcoin_hd_key_pubkey() {
    libbitcoin::wallet::hd_key k;
    int n = 0;
    for (; n < 13; n++) {
        k[n] = 0;
    }
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        k[n] = Pubkey.ChainCode[i];
        n++;
    }
    
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        k[n] = Pubkey.Point[i];
        n++;
    }
    
    return k;
}

const libbitcoin::wallet::hd_private hd_pair::libbitcoin_hd_private() {    
    return libbitcoin::wallet::hd_private(libbitcoin_hd_key_secret());
}

const libbitcoin::wallet::hd_public hd_pair::libbitcoin_hd_public() {    
    return libbitcoin::wallet::hd_public(libbitcoin_hd_key_pubkey());
}

const hd_pair hd_from_string(string hd) {
    data_chunk data;
    libbitcoin::decode_base16(data, hd);
    hd_secret secret;
    hd::from_data(data, secret);
    return {secret, secret.Pubkey};
}

// A  bunch of randomly generated test hd keys in base 16.
const vector<hd_pair> test_keys{
	hd_from_string("07d501b57c3fa05394556fe8a43c3401c9d150fd3cf38f04ae2e9ac3e295258557f11bc85fef0dd46cc4011b6f18ea0f7f3fcf4e4d5207350435b4b87f71117f"),
	hd_from_string("54f23dd6170f35f6286717f7c558a7fefadf60469a5dd1ab5d3b8df3207e6e877582514bb99a1ca31d82f742271066e18a4f39ccbc49f695849dc8c39f3fec9f"),
	hd_from_string("9daeae4a4f494485d399d7d8cb1348b02d11e2891d23c4748b4a720941eace9c6cd1625d7b9cadb6d6f002f5daa99e3b87491a50ed0d02d0a91715dc2eb47434"),
	hd_from_string("d86cfa13727c5440e0b7e6f7abc57da6813f14422f8a9f9ad6e196689bec453116206e98b403912954ad9eb86cd2396f6430315c5919c0315ac79c840eed5cc4"),
	hd_from_string("c53fdbd4038e09de3f8c45f54006799b0507a15ff12174abae2e8b26a1c799cac1acd33a70b4f0987979a4938f2052b3105273185fde3a4b75e41399e2b6eb96"),
	hd_from_string("30a77d2c2eeb685d7229230c3c183d2adef4bdb1c1bc9db55a76d945fcc21d3718a9af328ccb67eae1e6d37ae9de10e3846863e76eb03b8426ddb2898c940dd1"),
	hd_from_string("5a0c5fbc0fbae86e40837dfc1730792b610d4f799d4d77312ff237cb24b74ff4843d7d6072de2d05edb539e5e22bc39f5e6314498c98c28bfe20a951b060fc43"),
	hd_from_string("0c3e5c0130576c385c53709853b3a9898d5c089c8f902104b60f38f2fc89092c41939511f57513d3ca8c0232b7c78a3e09dbbe3abcfd2804ec45769c6521d2a5"),
	hd_from_string("2e8f3e251f7f1997c07b63d40073ead473871744e6433305a4b71e422d2760c74f3e5da4eef2600c75fa07ce64c97aea538537f64d60c0ce887bfe5d6dde3e5f"),
	hd_from_string("b95970f79d41ccaf540c690b3b8b07f041458b7bf8bc9deca383036d0742864c22cb0c5560d432287cfc2e063262815d81479f0aa1f72acf94a232a65434b405"),
	hd_from_string("213a356a1982505d375c09411fbadfeefd6195eea9e9da7084f20177cb608b146ad0d9720ac92557e6176b95981b60108d4fc452f8d44de485d537cb5de82a47"),
	hd_from_string("027988c2bfff15287e15db363b6045915b9001cf24207b7b1f7b750b1a0d4e30332c1ccfae76a256ad7995b0af031139323f02184726e006a96aa9b4e6243b87"),
	hd_from_string("2c9a7a622b08aaa70d2c71be062b08a877624c65d1842bb458aef68c977fbb47140e3d36f422ae0adb44f1357e208fbf91f4fc3b8d93ed061d6a78c5146c0358"),
	hd_from_string("bcfd8af6fc2e8787ac7523bb568509d7ed6aa3b9f1a59b3753dbfdab287daf385e320e21f62190c0844f288df18bed7d8fe5415fbae8327560b1fb33a01bb997"),
	hd_from_string("6154c4a56bb5ea9a82fc4c037268a5ba09aeb065f32568482d692eff22d237e9d1a397698a3245b05f5f31b320f7d176b24a8d9fa50068f55666a97c23f68847"),
	hd_from_string("5b746138f7c980baa8a17f3d63b590c8f78470b1ec8904c5bcac82a8d6fba03f572385896a91367a173e7fee939decdedaf67806610a5b353de42addab5af64a"),
	hd_from_string("a8fede1655629479d753736854839366d87d652eb391e6823041d627972d7b628be94031f7c9b8ed34511d1fb958bccf41f665a7426cfc766181a78a3ae97f50"),
	hd_from_string("5de8832aa3158382980adaf95917263f32485b2ea1989583723858be57bf37f7c79950d771325077c5dd377d1cb913ae49d73402299b10d122d2f04ae88874d6"),
	hd_from_string("21d43c0b7b69ac69bc48e5247d7848810c3337b991de9fbb1ced2e09cca16436f920bb77ff7b38681e7674354799a1abd99d5664f235d1a33c81f6467b0de6ec"),
	hd_from_string("b02ae12d87488fbaefa5586965f71d7f1bf39105dba117e599c1a427c038f6026daa520051b49d9bba16dcda311194bd19e1e9b9d0b9e69f96569105ae3e37d5"),
	hd_from_string("101b8db2c0f2640685ded8d567f296a4c5ca51d0a85e4b9030402ae24212a904acfb7be29c8b420d19fec923cedaa2f45ff03cf484a7f783d818aac065a9f735"),
	hd_from_string("d72dae1ab0dea5268b3b6de07cd8b7d75def05c0af4aed6e0b6a9159c0b6b3149398d6d8f0305d565236c380d3e39a974d44c15e07ec561ce870a0a51943c580"),
	hd_from_string("561b8b98111080bd22bd332a6cd34628101cd2c31d38e60700118580872e76541dfb8ed0178784d24502608fc34ac186b88bec9bda14f8759ff50a22754beaea"),
	hd_from_string("8ea2f297418620abacbed5ae173cfd2fecff7e30a3b472f07092a4324af08740c81dc05d3a7a6023160b8cf6c62a6b26936d0ea8da102d15603629c155424d4d"),
	hd_from_string("995fe178e0cf7a60d585aeedf6a53f7f6a13ac923aafea408da1ed00f8fd62d7d3b82cce00d0740fe8ecbdfe34fafdaff97967ffcb601d63f3f85d5a8a29d62c"),
	hd_from_string("5f3962d1e8596e930d821f1501f69da28aee48d2a099de49fffb72dfe749c59f45bbd7f5dc575524149edba3457cc6d1cc1cd6ef884c1bd05a91540ff7613fdd"),
	hd_from_string("24cdd246c36ba07cb486748d9a8d4cd0676492d02358b722d096c69328fa0892889659bebe7663c7ce67443bb65022b7c04ede6771714f455062b0028e0ad215"),
	hd_from_string("d8533cd7e11581049ff5a987f2f0605376d3cba34ef7540430195ed4b77129ece79895790d67682e017b6056fafddb2268a76f6cb37b94cea8e86a68156746ed"),
	hd_from_string("4f82d6ff8ca6c4edd4200c85a4ee7027190deac7e79613fa333a82ca8ef55a2a72e2659eb650896ebb4408a93b4a38e04bec37959ed8201189135667fd7fae52"),
	hd_from_string("686372506b34175e4e572e378bed1256f2b9f55d8d7b0d787c9d9595307f218675ed0c647a4d0a14174ce4995d3b6f2e2f3e50a08ef6856ca07fb46633b36e20"),
	hd_from_string("982dc7fb6ea7dd89a269714a145e3c025cd07d105f3d00f93af00173901f4d7351e3b0d99a3b88e07ab9c7019450f01d9724c4667a1a1e72b74a850f8d264d25"),
	hd_from_string("554c3ba622470dd65c9ab2daac7bd10acabe046887c480e5c356b535c2089c008899e9926fee93195461c31909dc15bb0909b5b2b7aa118a4bed976cc049d8f5"),
	hd_from_string("4ef813c06295b64561cab05a346e51c3f10e6b910004215bcbeb7a6b1ae14ceb388e0e9b85abe417d54fe21d32f88f9de359e76cf656aebaabebec2eaa645db0"),
	hd_from_string("7e84f7517e5d52289588d1ba43df0bb249b4308075036e7acb245112907b7b5569081ca21cb4954d173196d29a8ec08da55d49f67fae8c3815424976057e22b9"),
	hd_from_string("289e0f246775a7f635d97f393065c9e33304953b54a8ad89e0aaedb0c5465469574a9628d61946eee276bf057eebbd1477a806366d0b0642ff9819a3bb80eb0f"),
	hd_from_string("818684142c0ee4198ad248d8c7b1594efcef5d627e19baf4c5b8f569499b677a40524cfae3a33dcd682fa10b005695d347d745f05fa7202ad7e38805f3b9aeb9"),
	hd_from_string("f24cfd563c17c0824efd9bdb6a28544861929a46dd1ca0970463ba79e9646f089441660ef3c7bd9ed19a07194ccd18b3395d673859236e7abf6e9f1ab5252364"),
	hd_from_string("4e9c6b1efc6834276709fc2df5eccaac0f1a4bfb98cdda673c7d1c7bac052f7ccadc31ae761c5a7c5a33e47cdd335757ea8c0679b99a73e83ce801b3ad59f951"),
	hd_from_string("8fe6d1b3eda280f65f4181c92823f76049a9d2af8f0580184f43e9c19e360cfb4cc2ca3ac611a9d2f142e91a244c5e751192a4e549c5e1bf18a0ad3c34b4cb30"),
	hd_from_string("0bd6e5856487a5bd593c5958c980fa203200999a3b6b523674273da2c3e17850bb4dcc345f1b3ddbafead019158af0e74cdcf7ae51d157e8ba18413fa9770e19")};

payment_code_version next_test_version(payment_code_version test_version) {
    return test_version % 3 + 1;
}

// This should be similar to the payment_code constructor except we don't check the version
// and we write it to a base 58 string. This is used to construct valid and invalid test cases.
string write_test_payment_code(payment_code_version version, const hd_public& pubkey, bool bitmessage_notification) {
    libbitcoin::byte_array<payment_code_size> code;
    
    code[0] = version;
    code[1] = bitmessage_notification;
    
    int n = 2;
    for (auto p : pubkey.Point) {
        code[n] = p;
        n++;
    }
    
    for (auto p : pubkey.ChainCode) {
        code[n] = p;
        n++;
    }
    
    for (; n < payment_code_size; n++) {
        code[n] = 0;
    }
    
    auto data = libbitcoin::to_chunk(code);
    libbitcoin::append_checksum(data);
    return libbitcoin::encode_base58(data);
}
    
test_payment_code::test_payment_code(payment_code_version version, const hd_secret pk, bool bitmessage_notification)
    :key(pk), code(write_test_payment_code(version, pk.Pubkey, bitmessage_notification)) {}

test_payment_code generate_valid_payment_code(hd_pair hd) {
    static payment_code_version test_version = 0;
    test_version = next_test_version(test_version);
    return test_payment_code(test_version, hd.Secret, false);
}

test_payment_code generate_invalid_payment_code(hd_pair hd) {
    return test_payment_code(0, hd.Secret, false);
}

typedef test_payment_code (*payment_code_generator)(hd_pair);

payment_code_generator generate_payment_code(bool valid) {
    if (valid) return generate_valid_payment_code;
    else return generate_invalid_payment_code;
}

const vector<test_payment_code> generate_test_payment_codes(bool valid, const vector<hd_pair>& test_keys) {
    vector<test_payment_code> test_secrets(test_keys.size());
    transform(begin(test_keys), end(test_keys), begin(test_secrets), generate_payment_code(valid));
    return test_secrets;
}

// Generate some version 2 payment codes from the hd keys given earlier. 
const vector<test_payment_code> valid_test_payment_codes = generate_test_payment_codes(true, test_keys);
const vector<test_payment_code> invalid_test_payment_codes = generate_test_payment_codes(false, test_keys);

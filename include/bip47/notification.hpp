#ifndef BIP47_NOTIFICATION_HPP
#define BIP47_NOTIFICATION_HPP

#include <vector>
#include <bip47/secret.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <abstractions/blockchain/blockchain.hpp>

namespace bip47
{

typedef libbitcoin::chain::output output;
typedef libbitcoin::chain::output_point outpoint;
typedef libbitcoin::chain::transaction transaction;
typedef libbitcoin::wallet::ec_public ec_public;

using blockchain = abstractions::blockchain<const outpoint, const output>;

    namespace notifications
    {

        namespace v1
        {

            bool is_notification_output(const output& output);

            bool valid(const transaction &tx);

            bool to(const transaction& tx, const address& notification_address);

            bool read(payment_code& out, const transaction& tx, const blockchain&, const notification_key& notification);

        } // v1

        namespace v2
        {

            bool valid(const transaction &tx);

            bool to(const transaction& tx, const payment_code_identifier& bob);

            bool read(payment_code& out, const transaction& tx, const blockchain& b, const secret& pc);
            
            const std::vector<output> notification_transaction_outputs(
                const payment_code& alice,
                const payment_code& bob,
                unsigned int amount,
                const ec_secret& designated, 
                const outpoint& prior);

        } // v2
        
    } // notifications

    namespace low
    {

        bool to(const output& output, const address& notification_address);

        inline const output pay_key_hash(const payment_code& to, unsigned int amount, address_format format) {
            return output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(notification_address(to, format))));
        }

        bool identifier_equals(const ec_compressed &x, const data_chunk& y);

        inline const address to_payment_address(const ec_secret& key, const address_format format) {
            return libbitcoin::wallet::ec_private(key, format).to_payment_address();
        }

        bool read_notification_payload(payment_code& out, const output& output);

        bool designated_pubkey_and_outpoint(ec_public& designated, outpoint& op, const blockchain&, const transaction& nt);

        bool unmask_payment_code(payment_code& code, const ec_public& designated, const ec_secret& pk, const outpoint& outpoint);

        namespace v2
        {

            bool is_notification_change_output_pattern(const libbitcoin::machine::operation::list& ops);

            const output notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount);

            inline bool is_notification_change_output(const output& output) {
                return is_notification_change_output_pattern(output.script().operations());
            }

            const bool is_notification_change_output_to(const output& output, const payment_code_identifier bob_id);

        } // v2

    } // low
    
    namespace notifications {
        
        namespace v1 
        {

            inline bool is_notification_output(const output& output) {
                const auto ops = output.script().operations();
                return libbitcoin::chain::script::is_null_data_pattern(ops) && ops[1].data().size() == 80;
            }

            inline bool valid(const transaction& tx)
            {
                for (const auto output : tx.outputs()) if (is_notification_output(output)) return true;
                return false;
            }

            inline bool to(const transaction& tx, const address& notification_address) {
                for (const auto output : tx.outputs()) if (low::to(output, notification_address)) return true;     
                return false;
            }
            
            inline const output notification_output(
                const payment_code& alice,
                const payment_code& bob,
                const outpoint& prior, 
                const ec_secret& designated) {
                return output(0, libbitcoin::chain::script(libbitcoin::chain::script::to_null_data_pattern(low::mask_payment_code(alice, designated, low::point(bob), prior))));
            }

            inline const std::vector<output> notification_transaction_outputs(
                const payment_code& alice,
                const payment_code& bob,
                unsigned int amount,
                const ec_secret& designated, 
                const outpoint& prior, 
                address_format format
            ) {
                return {low::pay_key_hash(bob, amount, format), 
                    notification_output(alice, bob, prior, designated)};
            }
            
        } // v1
        
        namespace v2 
        {

            inline const std::vector<output> notification_transaction_outputs(
                const payment_code& alice,
                const payment_code& bob,
                unsigned int amount,
                const ec_secret& designated, 
                const outpoint& prior
            ) {
                return { 
                    v1::notification_output(alice, bob, prior, designated),
                    low::v2::notification_change_output(alice.point(), bob, amount)};
            }
            
        } // v2
        
    } // notifications

} // bip47

#endif

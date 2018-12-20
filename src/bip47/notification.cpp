#include <bip47/notification.hpp>
#include <bip47/patterns.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace bip47
{

    namespace notifications
    {

        namespace v1
        {

            bool read(payment_code& out, const transaction& tx, const blockchain& b, const notification_key& notification) {
                // Is this a transaction to the notification address? 
                if (!to(tx, notification.notification_address)) return false;
                
                // Find designated pubkey. 
                ec_public designated;
                outpoint op;
                if (!low::designated_pubkey_and_outpoint(designated, op, b, tx)) return false;
                
                // Find a valid notification output. 
                for (const auto output : tx.outputs()) if (low::read_notification_payload(out, output)) {
                    // If this is not a version 1 payment code continue. 
                    if (out.version() != 1) continue;
                    
                    if (!low::unmask_payment_code(out, designated, notification.secret.Secret, op)) continue;
                    
                    if (!abstractions::valid(out.point())) continue;
                }
                
                return false;
            }

        } // v1

        namespace v2
        {

            bool valid(const transaction& tx) {
                if (!v1::valid(tx)) return false;
                for (auto output : tx.outputs()) {
                    if (!low::v2::is_notification_change_output(output)) continue;
                    else return true;
                }
                
                return false;
            }

            bool to(const transaction& tx, const payment_code_identifier& bob) {
                if (!v1::valid(tx)) return false;
                for (auto output : tx.outputs()) if (low::v2::is_notification_change_output_to(output, bob)) return true;
                return false;
            }

            bool read(payment_code& out, const transaction& tx, const blockchain& b, const secret& pc) {    
                // Find designated pubkey. 
                ec_public designated;
                outpoint op;
                if (!low::designated_pubkey_and_outpoint(designated, op, b, tx)) return false;
                
                // Find a valid notification output. 
                for (const auto output : tx.outputs()) if (low::v2::is_notification_change_output_to(output, pc.code.identifier())) {
                    // If this is not a version 2 payment code continue. 
                    if (out.version() != 2) continue;
                    
                    if (!low::unmask_payment_code(out, designated, pc.key.Secret, op)) continue;
                    
                    if (!abstractions::valid(out.point())) continue;
                }
                
                return false;
            }
            
        } // v2

        namespace v3
        {

            typedef libbitcoin::ec_uncompressed ec_uncompressed;

            bool is_notification_pattern(const libbitcoin::machine::operation::list& ops) {
                if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
                    return false;
                }
                
                // Are we talking about a 1 of 3 multisig? 
                if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
                if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 3) return false;
                
                // Are the first two keys compressed? 
                if (ops[1].data().size() != libbitcoin::ec_compressed_size) return false;
                if (ops[2].data().size() != libbitcoin::ec_compressed_size) return false;
                
                // Is the last key uncompressed? 
                if (ops[3].data().size() != libbitcoin::ec_uncompressed_size) return false;
                
                // TODO Is ordering important? 
                
                return true;
            }

            bool inline valid(const output& output) {
                return is_notification_pattern(output.script().operations());
            }

            bool to(const transaction& tx, const ec_compressed& bob_id)
            {
                for (auto output : tx.outputs()) if (valid(output)) {
                    auto ops = output.script().operations();
                    
                    if (low::identifier_equals(bob_id, ops[1].data())) return true;
                }
                
                return false;
            }

            bool read(ec_uncompressed& payload, const transaction& tx, const ec_compressed& bob_id)
            {
                for (auto output : tx.outputs()) if (valid(output)) {
                    auto ops = output.script().operations();
                    
                    if (low::identifier_equals(bob_id, ops[1].data())) {
                        for (int i = 0; i < payload.size(); i++) {
                            payload[i] = ops[3].data()[i];
                        }
                        std::copy(ops[3].data().begin(), ops[3].data().end(), payload.begin());
                        return true;
                    }
                }
                
                return false;
            }

        } // v3

    } // notifications

} // bip47

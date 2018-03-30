#include <bip47/secret.hpp>
#include <bip47/notification.hpp>

namespace bip47
{

secret::secret(int version, const hd_private& key, bool bitmessage_notification):key(key), code(payment_code(version, key.to_public(), bitmessage_notification)) {}
    
} // bip47

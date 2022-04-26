#include "Utils/encstr/cfb.hpp"
#include "Utils/encstr/ciphers/xtea_cipher.hpp"
#include "Utils/encstr/string.hpp"
using encoder = encstr::cfb_t<encstr::ciphers::xtea_cipher_t>;
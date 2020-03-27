require 'mkmf'

have_library("crypto", "CRYPTO_lock")
have_library("ssl", "SSL_connect")
create_makefile("tclink")

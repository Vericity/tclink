# DESCRIPTION

See the original TrustCommerce README and LICENSE in ext/tclink

# NOTE FOR THE CURRENT RELEASE

The ext/tclink directory is source from TrustCommerce for tclink
4.5.0. This release works with openssl 1.1.

Within ext/tclink the original source has been preserved with the
following changes to support building a gem that can be configured and
installed with native extensions:

1. Patrick Logan
⋅⋅1. Renamed the original extconf.rb to create_makefile.rb
⋅⋅2. Defined extconf.rb to run the shell command build.sh

Note that build.sh is the TrustCommerce script to configure a makefile
for the current environment. This has been successful on Linux. An
initial attempt failed on OSX but in theory this should work given a
configuration for openssl 1.1


# NOTE FROM PREVIOUS RELEASE

I forked this from a fork of the original repository whose creator writes:

"I packaged this up as a gem for my own uses. I am not affiliated with
Trust Commerce and have not modified any of the C code.

If you have trouble building this, you can download a tarball of the source
from Trust Commerce's vault.
"

--Phillip Birmingham
phillip.birmingham@fidelitylife.com

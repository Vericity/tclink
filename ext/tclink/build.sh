#!/bin/sh
#
# Build script for TCLink Ruby module
#

echo \#define TCLINK_VERSION \"4.5.0-Ruby-`uname -sm | tr ' ' -`\" > config.h

for i in "$@"; do
	case $i in
		-d=*|--ca-path=*)
		CA_PATH="${i#*=}"
		;;
	esac
done

if test -z "$CA_PATH"; then
	OPENSSL_CA_PATH=`openssl version -d | sed 's/[^"]*"\(.*\)"/\1/'`	# grab whatever between the double quotes 
	if test -n "$OPENSSL_CA_PATH"; then
		OPENSSL_CA_PATH="$OPENSSL_CA_PATH/certs"
		echo "openssl CA path: $OPENSSL_CA_PATH"
		
		for item in \
			"$OPENSSL_CA_PATH/ca-bundle.crt" \
			"$OPENSSL_CA_PATH/ca-certificates.crt" \
			"$OPENSSL_CA_PATH/ca-bundle.trust.crt" \
			"$OPENSSL_CA_PATH/tls-ca-bundle.pem" \
			"$OPENSSL_CA_PATH"
		do
			if test -f $item || test -d $item; then
				echo "Found $item"
				CA_PATH=$item
				break
			fi
		done
	fi	
fi

if test -z "$CA_PATH"; then
	for item in \
		"/etc/pki/tls/certs/ca-bundle.crt" \
		"/etc/ssl/certs/ca-certificates.crt" \
		"/etc/pki/tls/certs/ca-bundle.trust.crt" \
		"/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem" \
		"/etc/ssl/certs"
	do
		if test -f $item || test -d $item; then
			echo "Found $item" 
			CA_PATH=$item
			break
		fi
	done
fi
if test -z "$CA_PATH"; then
	echo "Cannot determine CA path. Please use --ca-path=<path> to specify ca-bundle file or CA directory."
	exit 2
fi

echo "CA path: $CA_PATH"
echo \#define TCLINK_CA_PATH \"$CA_PATH\" >> config.h

ERROR=0
ruby create_makefile.rb || ERROR=1

if [ $ERROR = 1 ]; then
	echo "Couldn't generate Makefile.  Check to make sure you have the Ruby development package installed."
	exit 1
fi

make clean &&	# necessary to pick up changes in config.h
make || exit

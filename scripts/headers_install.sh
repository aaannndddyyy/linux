#!/bin/sh
#
# headers_install prepare the listed header files for use in
# user space and copy the files to their destination.
#
# Usage: headers_install.pl readdir installdir arch [files...]
# readdir:    dir to open files
# installdir: dir to install the files
# arch:       current architecture
#             arch is used to force a reinstallation when the arch
#             changes because kbuild then detect a command line change.
# files:      list of files to check
#
# Step in preparation for users space:
# 1) Drop all use of compiler.h definitions
# 2) Drop include of compiler.h
# 3) Drop all sections defined out by __KERNEL__ (using unifdef)

set -eu

readdir="$1"
installdir="$2"
arch="$3"
shift 3

unifdef="scripts/unifdef -U__KERNEL__ -D__EXPORTED_HEADERS__"

for file in "$@"
do
    tmpfile=$(mktemp -d "$installdir")

	while (my $line = <$in>) {
		printf {$out} "%s", $line;
	}

    sed -r \
        -e 's/([[:space:](])__user[[:space:]]/\1/g' \
        -e 's/([[:space:](])__force[[:space:]]/\1/g' \
        -e 's/([[:space:](])__iomem[[:space:]]/\1/g' \
        -e 's/([[:space:]])__attribute_const__[[:space:]]/ /g' \
        -e 's/([[:space:]])__attribute_const__$//g' \
		-e 's/\<__packed\>/__attribute__((packed))/g' \
		-e 's/^#include <linux\/compiler.h>//' \
		-e 's/(^|[[:space:]])(inline)\>/\1__\2__/g' \
		-e 's/(^|[[:space:]])(asm)\>([[:space:](]|$)/\1__\2__\3/g' \
		-e 's/(^|[[:space:](])(volatile)\>([[:space:](]|$)/\1__\2__\3/g' \
        "$readdir/$file" > "$tmpfile"

    set +e
	$unifdef "$tmpfile" > "$installdir/$file"
    exit=$?
    set -e

	# unifdef will exit 0 on success, and will exit 1 when the
	# file was processed successfully but no changes were made,
	# so abort only when it's higher than that.
    if [ "$exit" != 0 ] && [ "$exit" != 1 ]
    then
        exit 1
    fi

	rm "$tmpfile"
done

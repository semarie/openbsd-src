#!/bin/sh
mode="$1"
shift

case "${mode}" in
pass)
	if "${@}" 2>&1; then
		exit 0
	else
		exit 1
	fi
	;;
fail)
	if "${@}" 2>&1; then
		exit 1
	else
		exit 0
	fi
	;;
*)
	echo "usage: run.sh [pass | fail] program args..." >&2
	exit 2
esac

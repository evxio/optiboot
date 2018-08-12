#!/usr/bin/env bash

LOCAL_TOOLS_DIR=$HOME/avr-tools

if [ -z "$TRAVIS_BUILD_DIR" ]; then
    echo "This script should be run by Travis-CI environment"
    echo "If you want to simulate Travis build, please set TRAVIS_BUILD_DIR"
    echo "envirinment variable to directory where your code lives"
    exit 1
fi


# download all compilers
$TRAVIS_BUILD_DIR/scripts/travis-fill-cache.sh

# prepare output dir
OUTPUT_DIR="$TRAVIS_BUILD_DIR/sizes-out"
OUTPUT_TABLE="$OUTPUT_DIR/sizes.txt"

mkdir -p "$OUTPUT_DIR"

COMPILERS=$(cat $TRAVIS_BUILD_DIR/docs/arduino-gcc-versions.md |grep -i "| yes |"|cut -f 2 -d '|')
COMPILERS="$COMPILERS microchip"

# table header
echo -n "| target \ compiler |" >"$OUTPUT_TABLE"
for compiler in $COMPILERS; do
  echo -n " $compiler |" >>"$OUTPUT_TABLE"
done
echo >>"$OUTPUT_TABLE"
# table header separator
echo -n "|-|" >>"$OUTPUT_TABLE"
for compiler in $COMPILERS; do
    echo -n "-|">>"$OUTPUT_TABLE"
done
echo >>"$OUTPUT_TABLE"

# build everything
cat $TRAVIS_BUILD_DIR/.travis.yml|grep "    - OPTIBOOT_TARGET="|cut -f 2- -d '=' \
  |tr -d '"'|sort|while read target; do
    echo -n "| $target |" >>"$OUTPUT_TABLE"
    for compiler in $COMPILERS; do
	echo "Checking size for $target @ $compiler"
	size=$($TRAVIS_BUILD_DIR/scripts/travis-build.sh $compiler $target 2>/dev/null|grep -A 2 avr-size|tail -n1|awk '{ print $1;}')
	if [[ -z "$size" ]]; then
	    size="x"
	fi
	echo -n " $size |" >>"$OUTPUT_TABLE"
    done
    echo >>"$OUTPUT_TABLE"
done

echo "========= OUTPUT SIZES START ============="
cat "$OUTPUT_TABLE"
echo "========== OUTPUT SIZES END =============="

#!/bin/bash

system=$(uname -s)
machine=$(uname -m)

# Fix virtual system on windows
for s in Windows CYGWIN MINGW MSYS UWIN; do
	if [[ "$system" == "$s"* ]]; then
		system=Windows
		break
	fi
done

echo "System is $system:$machine"


### Get premake binary
version=5.0.0-alpha13
location=.
download_base=https://github.com/premake/premake-core/releases/download/v$version/premake-$version

# Windows
if [[ "$system" == "Windows" ]]; then
	file=$location/premake5.zip
	curl -L -o $file $download_base-windows.zip
	unzip -oqu $file -d $location/
	rm -f $file

# Linux x86*
elif [[ "$system" == "Linux" && "$machine" == "x86"* ]]; then
	file=$location/premake5.tar.gz
	curl -L -o $file $download_base-linux.tar.gz
	tar -xvzf $file -C $location/
	rm -f $file

# macOS
elif [[ "$system" == "Darwin" ]]; then
	file=$location/premake5.tar.gz
	curl -L -o $file $download_base-macosx.tar.gz
	tar -xvzf $file -C $location/
	rm -f $file

# Prebuilt binaries not available. Build from source.
else
	file=$location/premake5-src.zip
	curl -L -o $file $download_base-src.zip
	unzip -o $file -d $location/
	echo "Premake binaries unavailable for $system:$machine. Building from source.."
	make -f $location/Bootstrap.mak $system
	cp $location/bin/release/premake5 $location/
	rm -rf $location/premake-$version/
	rm -f $file
fi



### Initialize submodules
git submodule update --init
git submodule foreach --recursive "git submodule update --init"

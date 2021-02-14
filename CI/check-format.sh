#!/bin/bash
dirty=$(git ls-files --modified)

set +x
if [[ $dirty ]]; then
	echo "================================="
    echo "Files were not formatted properly"
    echo "$dirty"

		echo "================================="
		git config --global user.email "cpyarger@gmail.com"
		git config --global user.name "Christopher P Yarger"
		git add -A
		git commit -am "Clang formatted Files"
		git push
fi

#!/bin/bash

set -e

OSTYPE=$(uname)

if [ "${OSTYPE}" != "Darwin" ]; then
    echo "[obs-midi - Error] macOS package script can be run on Darwin-type OS only."
    exit 1
fi

echo "[obs-midi] Preparing package build"
export QT_CELLAR_PREFIX="$(/usr/bin/find /usr/local/Cellar/qt -d 1 | sort -t '.' -k 1,1n -k 2,2n -k 3,3n | tail -n 1)"

GIT_HASH=$(git rev-parse --short HEAD)
GIT_BRANCH_OR_TAG=$(git name-rev --name-only HEAD | awk -F/ '{print $NF}')

VERSION="$GIT_HASH-$GIT_BRANCH_OR_TAG"

FILENAME_UNSIGNED="obs-midi-$VERSION-Unsigned.pkg"
FILENAME="obs-midi-$VERSION.pkg"

echo "[obs-midi] Modifying obs-midi.so"
install_name_tool \
	-change /usr/local/opt/qt/lib/QtWidgets.framework/Versions/5/QtWidgets \
		@executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets \
	-change /usr/local/opt/qt/lib/QtGui.framework/Versions/5/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui \
	-change /usr/local/opt/qt/lib/QtCore.framework/Versions/5/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore \
	./build/obs-midi.so

# Check if replacement worked
echo "[obs-midi] Dependencies for obs-midi"
otool -L ./build/obs-midi.so

if [[ "$RELEASE_MODE" == "True" ]]; then
	echo "[obs-midi] Signing plugin binary: obs-midi.so"
	codesign --sign "$CODE_SIGNING_IDENTITY" ./build/obs-midi.so
else
	echo "[obs-midi] Skipped plugin codesigning"
fi

echo "[obs-midi] Actual package build"
packagesbuild ./CI/macos/obs-midi.pkgproj

echo "[obs-midi] Renaming obs-midi.pkg to $FILENAME"
mv ./release/obs-midi.pkg ./release/$FILENAME_UNSIGNED

if [[ "$RELEASE_MODE" == "True" ]]; then
	echo "[obs-midi] Signing installer: $FILENAME"
	productsign \
		--sign "$INSTALLER_SIGNING_IDENTITY" \
		./release/$FILENAME_UNSIGNED \
		./release/$FILENAME
	rm ./release/$FILENAME_UNSIGNED

	echo "[obs-midi] Submitting installer $FILENAME for notarization"
	zip -r ./release/$FILENAME.zip ./release/$FILENAME
	UPLOAD_RESULT=$(xcrun altool \
		--notarize-app \
		--primary-bundle-id "fr.palakis.obs-midi" \
		--username "$AC_USERNAME" \
		--password "$AC_PASSWORD" \
		--asc-provider "$AC_PROVIDER_SHORTNAME" \
		--file "./release/$FILENAME.zip")
	rm ./release/$FILENAME.zip

	REQUEST_UUID=$(echo $UPLOAD_RESULT | awk -F ' = ' '/RequestUUID/ {print $2}')
	echo "Request UUID: $REQUEST_UUID"

	echo "[obs-midi] Wait for notarization result"
	# Pieces of code borrowed from rednoah/notarized-app
	while sleep 30 && date; do
		CHECK_RESULT=$(xcrun altool \
			--notarization-info "$REQUEST_UUID" \
			--username "$AC_USERNAME" \
			--password "$AC_PASSWORD" \
			--asc-provider "$AC_PROVIDER_SHORTNAME")
		echo $CHECK_RESULT

		if ! grep -q "Status: in progress" <<< "$CHECK_RESULT"; then
			echo "[obs-midi] Staple ticket to installer: $FILENAME"
			xcrun stapler staple ./release/$FILENAME
			break
		fi
	done
else
	echo "[obs-midi] Skipped installer codesigning and notarization"
fi

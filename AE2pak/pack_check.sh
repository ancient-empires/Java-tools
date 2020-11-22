if [[ $# < 2 ]]; then
	echo "ERROR: Too few arguments."
	echo "Usage: bash $0 <path/to/1.pak> <path/to/filelist.log>"
	exit 1
fi

targetPak=$1
refPak=$1
fileListLOG=$2

tempPak=$(mktemp --suffix=.pak)
make pack pak="$tempPak" filelist="$fileListLOG"

# .pak validation
test -f "$refPak"
refPakExists=$?
if [[ $refPakExists == 0 ]]; then
	diff $tempPak $refPak
	diffCode=$?
	if [[ $diffCode != 0 ]]; then
		echo -e "ERROR: validation failed for .pak file \"$tempPak\""
		exit 1
	fi
fi

mv "$tempPak" "$targetPak"
echo -e "Generated .pak file \"$targetPak\""

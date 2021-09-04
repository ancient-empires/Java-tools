makeTarget=pack

if [[ $# < 2 ]]; then
    echo "ERROR: Too few arguments."
    echo "Usage: bash $0 <path/to/1.pak> <path/to/filelist.log>"
    exit 1
fi

targetPak=$1
refPak=$1
fileListLOG=$2

tempPak=$(mktemp --suffix=.pak)
make $makeTarget pak="$tempPak" filelist="$fileListLOG"

# .pak validation
test -f "$refPak"
if [[ $? == 0 ]]; then
    diff $tempPak $refPak
    if [[ $? != 0 ]]; then
        echo -e "ERROR: validation failed for .pak file \"$tempPak\""
        rm $tempPak
        exit 1
    fi
fi

mv -v "$tempPak" "$targetPak"
echo -e "Generated .pak file \"$targetPak\""

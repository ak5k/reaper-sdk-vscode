#/bin/bash
function realpath()
{
    f=$@
    if [ -d "$f" ]; then
        base=""
        dir="$f"
    else
        base="/$(basename "$f")"
        dir=$(dirname "$f")
    fi
    dir=$(cd "$dir" && /bin/pwd)
    echo "$dir$base"
}

cd "$(dirname "$(realpath "$0")")";

if ! command -v php &> /dev/null 
then
    brew install php
fi

cd ../../WDL/swell/sample_project
cp resource.h ../
cp sample_project.rc ../resource.rc
cd ..
php ./mac_resgen.php --force ./resource.rc
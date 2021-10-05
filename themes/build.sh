filepaths=""

for file in ./*; do 
    if [ -f "$file" ]; then 
        if [[ "$file" == *.cpp ]]; then
            filepaths+=" $file"
        fi
    fi 
done

if [ ! -d ./dist ]; then
    mkdir ./dist
fi

echo "$filepaths"
emcc -o dist/output.js $filepaths -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1
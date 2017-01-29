echo "Compiling file: $1."

g++ $1.cpp -o $1 -I /Library/Frameworks/SDL2.framework/Versions/A/Headers -stdlib=libc++ -framework SDL2 -F /Library/Frameworks; $1;

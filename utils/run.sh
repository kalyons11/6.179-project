echo "Compiling file: $1.";

g++ $1.cpp -o $1 -I /Library/Frameworks/SDL2.framework/Versions/A/Headers -I /Library/Frameworks/SDL2_image.framework/Versions/A/Headers -I /Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers -stdlib=libc++ -framework SDL2 -framework SDL2_image -framework SDL2_ttf -F /Library/Frameworks; $1;
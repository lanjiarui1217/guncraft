cd /home/lenovo/guncraft-ai
g++ -std=c++17 -I. main.cpp $(ls heads/*.cpp) -o guncraft 2>/tmp/build.log
echo exit=$?
grep -iE 'error' /tmp/build.log | head
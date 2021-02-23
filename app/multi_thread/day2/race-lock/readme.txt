gcc simple_race.c -fsanitize=thread -g -pthread

old gcc version:
不依赖gcc fsanitize
gcc simple_race.c -g -lpthread
valgrind --tool=helgrind ./a.out 

http://github.com/google/sanitizers/wiki/ThreadSanitizeCppManual
用sanitizers内存消耗增长5-10倍，执行时间增加2-20倍；

valgrid.org/docs/manual/hg-manual.html

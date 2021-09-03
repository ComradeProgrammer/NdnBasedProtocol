code=$(find src -name "*.cpp" -and ! -name "cityhash.cpp" -or -name "*.h" -and ! -name "cityhash.h"| xargs wc -l| tail -n 1)
test=$(find src -name "*.cc" | xargs wc -l| tail -n 1)
e2e=$(find test -name "*.py" -or -name "*.sh" | xargs wc -l| tail -n 1)
echo "c++ source code lines:${code}"
echo "c++ unit test  code lines:${test}"
echo "E2E mininet code lines:${e2e}"
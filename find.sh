
git checkout origin/main

until ctest -C Release -R Integer_test; do
  git checkout HEAD~1 -f
  cmake --build ~/openmind/build --config Release --target Integer_test
done

git show


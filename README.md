# openmind

The OpenMind project is an infrastructure framework which mission is to allow new AGI-related core system with requirements as follows:

1. accumulation of knowledge in universal distributed form
1. any universal simplified language or math form of communication context
1. deduces answers
1. allow input of new information
1. checks input for absence of contradictions
1. checks input for acceptability by absence of contradictions with existing 100%-certainty knowledge in its knowledge base
1. allow requests of known information
1. allow requests for deductions
1. knows complete context of self as 100% predictability model
1. can make modified self copy within required inherited constraints (morphing)
1. impossible modification or deletion of existing knowledge or constraints 

Compilation:

  Debian/Ubuntu:

    sudo apt install -y cmake  ninja-build g++-10 git cmake-curses-gui ninja-build cmake-qt-gui 
    sudo apt install -y libboost-all-dev libxss-dev libx11-dev libxcb-screensaver0-dev ocl-icd-opencl-dev libopengl-dev freeglut3-dev libleveldb-dev libvulkan-dev liblz4-dev libfmt-dev librocksdb-dev libpython3-all-dev libopencl-clang-dev libtbb-dev
    sudo apt install -y libfdb5-dev mono-devel 
    
    git clone https://github.com/ohhmm/openmind
    cd openmind
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DBOOST_INCLUDE_DIR=/usr/include -DOPENMIND_BUILD_TESTS=OFF
    make -j8

    As result you can see following libraries produced:
    FEATURED ./omnn/math/libmath.a - the libmath which describes Valuable and Variable polymorphic types
    DEPENDENCY ./lib/libleveldb.a - LevelDB used by libmath for transparent solutions caching
    FEATURED ./omnn/extrapolator/libextrapolator.a - deducing extrapolator based on libmath
    PREVIEW ./omnn/rt/libomnn.a - openmind neural network library
    PREVIEW ./omnn/ct/libct.a - compile-time lib
    FEATURED ./OpenMind/libOpenMind.a - asynchronous goal-oriented state machine

  Other package managers:

    sudo dnf install -y git-gui cmake-gui clang boost-devel boost-static leveldb-devel libxcb-devel libX11-devel libXScrnSaver-devel ocl-icd-devel

  Windows VSCode:

    ![Screenshot](build.png)

Features:

  Basic Goals Framework 
  
    Asynchronous Interface
    Tracking
    Dependencies
    Goal Generators
    Goal reaching system
    Facilities framework
    Results processing
    
  Generic context framework - is currently in work and at repository only partially.
  
    Basically its meant to be like Sufix tree (like Trie, but Sufix tree) but more generic upon finish.
    Its planned to make its nodes able to be the Context structures as well.

Plans:

    To complete Generic Context Framework
    Implement self learning Chatbot as OpenMind sample
    lobby of dynamic class generation to C++ standard
    Introduce Neural network support as part of current Facilities framework
    context based neural network serialization and swapping
  
Q&A:

  Plese, feel free to ask questions on this StackOverflow team: 

    https://stackoverflowteams.com/c/skrypt/questions

  
Please Donate to help reach this plans:

    Bountysource: <https://salt.bountysource.com/teams/open-mind>

Support: sergeikrivonos@gmail.com

Bitcoin: 17JNBmpsDxp1d4jcAh7qthSN4VRcN9EDxQ

Thank you very much, appreciate your support.


FROM gentoo/stage3:latest

RUN emerge --sync

RUN echo 'CFLAGS="-march=native -mtune=native -O3 -pipe"' >> /etc/portage/make.conf
RUN echo 'CXXFLAGS="${CFLAGS}"' >> /etc/portage/make.conf
RUN echo 'USE="${USE} -examples"' >> /etc/portage/make.conf
RUN echo 'USE="${USE} lto static-libs"' >> /etc/portage/make.conf
RUN echo 'USE="${USE} system system-bootstrap system-llvm"' >> /etc/portage/make.conf

RUN echo "dev-libs/boost **" > /etc/portage/package.accept_keywords/boost
RUN echo "dev-util/b2 **" >> /etc/portage/package.accept_keywords/boost
RUN echo "dev-util/boost-build **" >> /etc/portage/package.accept_keywords/boost
RUN echo "dev-libs/boost icu lzma python test" > /etc/portage/package.use/boost

#RUN echo "dev-util/cmake **" > /etc/portage/package.accept_keywords/cmake
RUN echo "dev-libs/icu examples static-libs" > /etc/portage/package.use/icu
RUN echo "dev-lang/mono minimal" > /etc/portage/package.use/mono

RUN echo "sys-devel/llvm **" > /etc/portage/package.accept_keywords/llvm
RUN echo "sys-devel/llvm-toolchain-symlinks **" >> /etc/portage/package.accept_keywords/llvm
RUN echo "sys-devel/llvmgold **" >> /etc/portage/package.accept_keywords/llvm

RUN echo "sys-devel/clang **" > /etc/portage/package.accept_keywords/clang
RUN echo "sys-devel/clang-common **" >> /etc/portage/package.accept_keywords/clang
RUN echo "sys-devel/clang-runtime **" >> /etc/portage/package.accept_keywords/clang
RUN echo "sys-devel/clang-toolchain-symlinks **" >> /etc/portage/package.accept_keywords/clang
RUN echo "sys-libs/compiler-rt **" >> /etc/portage/package.accept_keywords/clang
RUN echo "sys-libs/compiler-rt-sanitizers **" >> /etc/portage/package.accept_keywords/clang
RUN echo "sys-libs/libomp **" >> /etc/portage/package.accept_keywords/clang


RUN emerge --update \
    dev-vcs/git \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge --update \
    dev-libs/icu \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge --update \
    =dev-libs/boost-1.81.0-r1 \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge --update \
    -1o dev-util/cmake \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge --update \
    dev-util/cmake \
    -j1 && rm -rf /var/cache/distfiles

RUN emerge --update   kde-frameworks/extra-cmake-modules     -j`nproc` && rm -rf /var/cache/distfiles

RUN (emerge --update sys-devel/clang:17 -j`nproc` || emerge --update sys-devel/clang:17 -j1) && rm -rf /var/cache/distfiles

RUN emerge --update \
    lld \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge -j`nproc` leveldb && rm -rf /var/cache/distfiles

RUN emerge --update \
    app-arch/lz4 \
    dev-libs/icu \
    dev-libs/openssl \
    sys-process/htop \
    sys-libs/zlib \
    x11-libs/libX11 \
    x11-libs/libxcb \
    x11-libs/libXScrnSaver \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge --update \
    dev-libs/icu \
    -j`nproc` && rm -rf /var/cache/distfiles
    
RUN emerge --update \
    gdb \
    lldb \
    -j`nproc` && rm -rf /var/cache/distfiles

#RUN emerge --update =dev-libs/boost-1.81.0 -j`nproc`

# FoundationDB deps
RUN emerge --update dev-cpp/benchmark dev-cpp/gtest -j`nproc` && rm -rf /var/cache/distfiles
RUN emerge --update dev-lang/mono -j`nproc` && rm -rf /var/cache/distfiles

RUN emerge --update \
    app-misc/mc \
    bash-completion \
    eix \
    sys-process/htop \
    -j`nproc` && rm -rf /var/cache/distfiles

RUN echo 'ACCEPT_LICENSE="*"' >> /etc/make.conf
RUN echo "dev-util/intel-ocl-sdk **" > /etc/portage/package.accept_keywords/intel-ocl-sdk
RUN emerge --update \
    dev-util/intel-ocl-sdk \
    -j`nproc` && rm -rf /var/cache/distfiles 

RUN emerge --update \
    dev-util/vulkan-headers \
    media-libs/vulkan-loader \
    -j`nproc` && rm -rf /var/cache/distfiles 

RUN eix-update

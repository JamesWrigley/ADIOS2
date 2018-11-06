FROM ubuntu:16.04

ENV CMAKE_SH=cmake-3.12.3-Linux-x86_64.sh
ENV BASHRC=/home/adios/.bashrc

# Clone repos first because they take a while, and it's faster to rebuild if
# this layer doesn't change.
RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y install git && \
    cd /tmp && \
    git clone https://github.com/ornladios/ADIOS2.git && \
    git clone https://github.com/pnorbert/adiosvm.git

# Install packages
RUN apt-get -y install apt-file build-essential libtool \
    libtool-bin autoconf subversion gfortran pkg-config \
    openmpi-common openmpi-bin libopenmpi-dev libzmq5 libzmq3-dev \
    bzip2 libbz2-dev zlib1g zlib1g-dev python python-dev python-pip cargo \
    python-tk python-cheetah python-yaml libfftw3-dev vim && \
    python -m pip install --upgrade pip && \
    pip install mpi4py && \
    # CMake
    curl -O https://cmake.org/files/v3.12/$CMAKE_SH && \
    sh ./$CMAKE_SH --skip-license --prefix=/usr/ && \
    rm -f $CMAKE_SH && \
    # ZFP
    git clone https://github.com/LLNL/zfp.git /tmp/zfp && \
    cd /tmp/zfp && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/ && \
    make -j$(nproc) && \
    make install && \
    rm -rf /tmp/foo

# Compile and install ADIOS2, and compile Brusselator
RUN useradd -m -G sudo adios && \
    cd /home/adios && \
    ln -s /tmp/ADIOS2 /tmp/adiosvm . && \
    mkdir build && \
    # Compile ADIOS2
    cd build && \
    cmake ../ADIOS2 -DCMAKE_INSTALL_PREFIX=/opt/adios2 -DCMAKE_BUILD_TYPE=Debug && \
    make -j$(nproc) && \
    make install && \
    chown -R adios: /tmp/adiosvm /tmp/ADIOS2 && \
    echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/adios2/lib" >> $BASHRC && \
    # Compile Brusselator
    cd ../adiosvm/Tutorial/brusselator/src && \
    make analysis && \
    make simulation && \
    # Helper aliases
    echo "alias sv='export SstVerbose=1'" >> $BASHRC && \
    echo "alias usv='unset SstVerbose'" >> $BASHRC

# Install hyperfine
RUN cargo install hyperfine && \
    echo "export PATH=/root/.cargo/bin:$PATH" >> $BASHRC && \
    apt-get remove cargo && \
    apt -y autoremove

CMD ["bash"]

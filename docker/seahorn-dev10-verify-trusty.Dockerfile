#
# Minimal Dockerfile for SeaHorn and verifyTrusty environment
# produces a lightweight container with SeaHorn, trusty source code and compiled harness files
#

FROM seahorn/seahorn-llvm10:nightly

ENV SEAHORN=/home/usea/seahorn/bin/sea PATH="$PATH:/home/usea/seahorn/bin:/home/usea/bin"

## install required pacakges
USER root
RUN echo "Pulling Verify Trusty environment" && \
    # installing repo
    mkdir ~/bin && PATH=~/bin:$PATH && \
    apt-get update && \
    apt-get install --no-install-recommends -yqq \
        software-properties-common \
        sudo curl build-essential vim gdb git \
        python-dev python-setuptools python-pip libgraphviz-dev libc6-dev-i386 \
        bear libssl-dev zip

## Install latest cmake
RUN apt -y remove --purge cmake
RUN apt -y update
RUN apt -y install wget
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
RUN apt -y update
RUN apt -y install cmake

## install pyyaml parser
RUN pip3 install setuptools --upgrade && \
    pip3 install pyyaml 

## use local verify-trusty
USER usea
WORKDIR /home/usea
RUN mkdir verifyTrusty
COPY . verifyTrusty

## clony trusty repository (takes a long time)
WORKDIR /home/usea/verifyTrusty
RUN echo "Installing Trusty" && \
    mkdir /home/usea/bin/ && curl https://storage.googleapis.com/git-repo-downloads/repo > /home/usea/bin/repo && \
    chmod a+x /home/usea/bin/repo && \
    mkdir trusty && cd trusty && \
    python3 /home/usea/bin/repo init -u https://android.googlesource.com/trusty/manifest -b master && \
    python3 /home/usea/bin/repo sync -j32 

## To test that everything is working pre-generate bc files for our verification tasks
WORKDIR /home/usea/verifyTrusty
RUN mkdir build && cd build && cmake -DSEA_LINK=llvm-link-10 -DCMAKE_C_COMPILER=clang-10 -DCMAKE_CXX_COMPILER=clang++-10 -DSEAHORN_ROOT=../../seahorn ../ -GNinja

## set default user and wait for someone to login and start running verification tasks
USER usea

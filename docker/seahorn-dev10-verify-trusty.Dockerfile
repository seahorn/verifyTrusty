#
# Minimal Dockerfile for SeaHorn and verifyTrusty environment
# produces a lightweight container with SeaHorn, trusty source code and compiled harness files
#

FROM leonsou/verify-trusty:latest

ENV SEAHORN=/home/usea/seahorn/bin/sea PATH="$PATH:/home/usea/seahorn/bin:/home/usea/bin"

## set default user and wait for someone to login and start running verification tasks
USER usea

## use local verify-trusty
WORKDIR /home/usea/
COPY . verifyTrusty

## To test that everything is working pre-generate bc files for our verification tasks
WORKDIR /home/usea/verifyTrusty
RUN mkdir build && cd build && cmake -DSEA_LINK=llvm-link-10 -DCMAKE_C_COMPILER=clang-10 -DCMAKE_CXX_COMPILER=clang++-10 -DSEAHORN_ROOT=../../seahorn -DTRUSTY_TARGET=arm32 ../ -GNinja

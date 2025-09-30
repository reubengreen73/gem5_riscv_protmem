# I've used ubuntu as the base of my image to have a fairly complete
# environment available while working on the build process. A more
# minimal base image would probably work fine but I have not tried
# to do this.
FROM ubuntu

# There might be some unnecessary things or some redundancy here, I
# have not tried to make this list minimal.
RUN apt update; apt install -y \
libpng-dev autoconf autotools-dev pkg-config pre-commit python3-pip libexpat-dev patchutils gperf \
libboost-all-dev gawk libslirp-dev libelf-dev libgoogle-perftools-dev python3-venv libmpc-dev curl \
wget texinfo libtool libgmp-dev git libmpfr-dev scons libprotoc-dev python3 doxygen protobuf-compiler \
bc m4 automake python3-tk build-essential libhdf5-serial-dev clang-format python3-tomli mypy flex \
ninja-build libcapstone-dev zlib1g python3-dev libprotobuf-dev bison cmake python3-pydot zlib1g-dev \
libglib2.0-dev

WORKDIR /root/
ENTRYPOINT ["/bin/bash"]
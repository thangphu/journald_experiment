FROM ubuntu:18.04
CMD "/bin/bash"
ENV DEBIAN_FRONTEND noninteractive
RUN dpkg --add-architecture armhf
RUN printf \
"deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ bionic main multiverse restricted universe \n\
deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports/ bionic main multiverse restricted universe \n\
deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports/ bionic-updates main multiverse restricted universe \n\
deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ bionic-updates main multiverse restricted universe \n\
deb [arch=amd64] http://security.ubuntu.com/ubuntu/ bionic-security main multiverse restricted universe" \
> /etc/apt/sources.list
RUN cat /etc/apt/sources.list
RUN apt-get update -y && apt-get upgrade -y
RUN apt-get install -y curl git build-essential
RUN apt-get install -y \
    libc6-armhf-cross \
    libc6-dev-armhf-cross \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    ninja-build \
    binutils-arm-linux-gnueabihf \
    libstdc++6-armhf-cross
RUN apt-get install -y libsystemd-dev libsystemd-dev:armhf
# Add builder
ENV BUILDER_UID=1000
ENV BUILDER_GID=1000
ENV BUILDER_NAME=particle
RUN groupadd -g ${BUILDER_GID} ${BUILDER_NAME}
RUN useradd ${BUILDER_NAME} -u ${BUILDER_UID} -g ${BUILDER_GID} -ms /bin/bash
RUN apt-get install -y sudo
RUN echo "${BUILDER_NAME} ALL=(ALL) NOPASSWD: ALL" | tee -a /etc/sudoers
USER ${BUILDER_NAME}
WORKDIR /home/${BUILDER_NAME}


# FROM ubuntu:18.04
# CMD "/bin/bash"
# ENV DEBIAN_FRONTEND noninteractive
# RUN apt-get update -y && apt-get upgrade -y
# RUN dpkg --add-architecture armhf
# RUN apt-get install -y curl git build-essential
# RUN apt-get install -y \
#     libc6-armhf-cross \
#     libc6-dev-armhf-cross \
#     gcc-arm-linux-gnueabihf \
#     g++-arm-linux-gnueabihf \
#     ninja-build \
#     binutils-arm-linux-gnueabihf \
#     # gcc-4.7-arm-linux-gnueabihf \
#     # gcc-4.7-multilib-arm-linux-gnueabihf \
#     # libgcc1-armhf-cross \
#     # libsfgcc1-armhf-cross \
#     libstdc++6-armhf-cross \
#     # libstdc++6-4.7-dev-armhf-cross
#     clang-6.0 \
#     libsystemd-dev
# # Add builder
# ENV BUILDER_UID=1000
# ENV BUILDER_GID=1000
# ENV BUILDER_NAME=particle
# RUN groupadd -g ${BUILDER_GID} ${BUILDER_NAME}
# RUN useradd ${BUILDER_NAME} -u ${BUILDER_UID} -g ${BUILDER_GID} -ms /bin/bash
# RUN apt-get install -y sudo
# RUN echo "${BUILDER_NAME} ALL=(ALL) NOPASSWD: ALL" | tee -a /etc/sudoers
# USER ${BUILDER_NAME}
# # Install Rust toolchain
# # RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | bash -s -- -y
# # RUN ${HOME}/.cargo/bin/rustup default stable
# # RUN ${HOME}/.cargo/bin/rustup target add armv7-unknown-linux-gnueabihf
# # WORKDIR /home/${BUILDER_NAME}
# # # Move build script
# # COPY build_arm.sh build_arm.sh
# # # Enable cross compile
# # ENV PKG_CONFIG_PATH="/usr/lib/arm-linux-gnueabihf/pkgconfig"
# # ENV PKG_CONFIG_ALLOW_CROSS="true"
# # # CMD [ "build_arm.sh" ]
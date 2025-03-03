#!/bin/bash

# create a .env file with:
# REMOTE_IP="172.16.0.17"
# REMOTE_SSH_USER="pancake"
# REMOTE_SSH_KEY="~/.ssh/local/id"

# load environment variables
source .env

# copy files to result directory
echo -e "\e[1;35m☆★☆ Copying project files to remote ☆★☆\e[0m"
rsync -avz \
    --exclude '.*' --exclude 'build' --exclude 'test.sh' \
    -e "ssh -i '$REMOTE_SSH_KEY'" \
    . $REMOTE_SSH_USER@$REMOTE_IP:/tmp/bdrm

# configure the remote project

ssh -t -i $REMOTE_SSH_KEY $REMOTE_SSH_USER@$REMOTE_IP \
    "cd /tmp/bdrm && cmake -G Ninja -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug"

# build the project remotely
echo -e "\e[1;35m☆★☆ Building remote project ☆★☆\e[0m"
ssh -t -i $REMOTE_SSH_KEY $REMOTE_SSH_USER@$REMOTE_IP \
    "cd /tmp/bdrm && ninja -C build"

# run/debug the project remotely
if [ "$1" == "debug" ]; then
    echo -e "\e[1;35m☆★☆ Debugging remote project ☆★☆\e[0m"
    ssh -t -i $REMOTE_SSH_KEY $REMOTE_SSH_USER@$REMOTE_IP \
        "cd /tmp/bdrm && gdb build/bdrm"
    exit
fi

echo -e "\e[1;35m☆★☆ Executing remote project ☆★☆\e[0m"
ssh -t -i $REMOTE_SSH_KEY $REMOTE_SSH_USER@$REMOTE_IP \
    "cd /tmp/bdrm && ./build/bdrm; echo -e '\e[1;1mexit code: '\$?'\e[1;0m'"
exit

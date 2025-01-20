#!/bin/bash
readonly TARGET_IP="$1"
readonly TARGET_PORT="$2"
readonly PROGRAM="$3"
readonly PROGRAM_PATH="$4"
readonly TARGET_DIR="/home/root"

# Must match startsPattern in tasks.json
echo "Deploying to Target"

# Kill gdbserver on target and delete old binary
ssh root@${TARGET_IP} "sh -c '/usr/bin/killall -q gdbserver; rm -rf ${TARGET_DIR}/${PROGRAM} exit 0'"

# Send the program to the Target
scp ${PROGRAM_PATH}${PROGRAM} root@${TARGET_IP}:${TARGET_DIR}

# Must match endsPattern in tasks.json
echo "Starting GDB Server on Target"

# Start gdbserver on target
ssh -t root@${TARGET_IP} "sh -c 'cd ${TARGET_DIR}; gdbserver localhost:${TARGET_PORT} ${PROGRAM}'"
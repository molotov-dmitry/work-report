#!/bin/bash

ROOT_PATH="$(cd "$(dirname "$0")" && pwd)"
cd "${ROOT_PATH}" || exit 1

sudo mkdir -p /usr/local/bin
sudo cp -f report-builder /usr/local/bin/

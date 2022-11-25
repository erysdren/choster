#!/bin/bash
g++ -g -o ./build/cohostxx cohost.cpp -lcurl -l:libcjson.a -l:libnettle.a
#!/bin/bash
MACHINE_ID=0
if [ -f ".FirstCrawl" ]; then
    echo "Re-starting docker script..."
    ./CrawlerStart $MACHINE_ID 0
else
    echo "Starting docker script for first time..."
    touch .FirstCrawl
    ./CrawlerStart $MACHINE_ID 1
fi
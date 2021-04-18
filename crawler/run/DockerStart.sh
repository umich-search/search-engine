#!/bin/bash
MACHINE_ID=$1
if [ -f ".FirstCrawl" ]; then
    echo "Re-starting docker script..."
    sleep 10 # Prevent docker from killing container
    ./CrawlerStart $MACHINE_ID 0
else
    echo "Starting docker script for first time..."
    touch .FirstCrawl
    echo 0 > /disk-index/documentCount
    ./CrawlerStart $MACHINE_ID 1
fi
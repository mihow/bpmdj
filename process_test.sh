#!/bin/bash
grep actual TempoTestResults.txt |  awk -f process_test.awk
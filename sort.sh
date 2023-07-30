#!/bin/bash

cat data | awk '{print $3" "$1" "$2}' > data.1
cat data.1 | sort -k2,3 | uniq -f1 > data.2

# To get the unique values
cat data.2 | sort -k1 | awk '{print $2" "$3" "$1}' | uniq -f2 -dD > final_data
rm data.1 data.2
more final_data

# cat data | awk '{print $3" "$1" "$2}' > data.1
# cat data.1 | sort -k2,3 | uniq -f1 > data.2
# more data.2 | sort -k1 | awk '{print $2" "$3" "$1}' | uniq -f2 -dD | less
# more data.2 | sort -k1 | awk '{print $2" "$3" "$1}' | uniq -f2 -c | grep -v "^      1"  | sort -n -r -k1 | less
